/*
###########################################################################################
// cNVMe - An Open Source NVMe Device Simulation - MIT License
// Copyright 2017 - Intel Corporation

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
############################################################################################
Controller.cpp - An implementation file for the NVMe Controller
*/

#define NVME_CALLER_IMPLEMENTATION(commandName) void Controller::commandName(NVME_COMMAND& command, COMPLETION_QUEUE_ENTRY& completionQueueEntryToPost)

#define ALL_NAMESPACES 0xFFFFFFFF
#define DEFAULT_NAMESPACE_SIZE 16384 // 16 kilobytes

#include "Command.h"
#include "Constants.h"
#include "Controller.h"
#include "PRP.h"
#include "Strings.h"

namespace cnvme
{
	namespace controller
	{
		Controller::Controller()
		{
			this->CommandResponseApiFilePath = "";

			PCIExpressRegisters = new pci::PCIExpressRegisters();
			PCIExpressRegisters->waitForChangeLoop();

			pci::header::PCI_HEADER* PciHeader = PCIExpressRegisters->getPciExpressRegisters().PciHeader;
			UINT_64 BAR0Address = (UINT_64)PciHeader->MLBAR.BA + ((UINT_64)PciHeader->MUBAR.BA << 18);
			ControllerRegisters = new controller::registers::ControllerRegisters(BAR0Address, this); // Put the controller registers in BAR0/BAR1
			ControllerRegisters->waitForChangeLoop();

#ifndef SINGLE_THREADED
			DoorbellWatcher = LoopingThread([&] {Controller::checkForChanges(); }, CHANGE_CHECK_SLEEP_MS);
			DoorbellWatcher.start();
#endif

			// Setup the IC with default values.
			memset(&this->IdentifyController, 0, sizeof(this->IdentifyController));
			resetIdentifyController();

			// Create default namespace
			this->NamespaceIdToActiveNamespace[1] = ns::Namespace(DEFAULT_NAMESPACE_SIZE);

			// Don't change running FW on reset
			this->ActiveFirmwareSlot = 1;
			this->FirmwareSlotToActivateOnReset = 0;

			// Create a 'Firmware Image' to save for the default firmware slot.
			Payload fwPayload;
			fwPayload.append(Payload((UINT_8*)FIRMWARE_EYE_CATCHER, sizeof(FIRMWARE_EYE_CATCHER)));
			fwPayload.append(Payload((UINT_8*)DEFAULT_FIRMWARE, sizeof(DEFAULT_FIRMWARE)));
			this->FirmwareSlotToFirmwareImagePayload[this->ActiveFirmwareSlot] = fwPayload;
		}

		Controller::~Controller()
		{
			DoorbellWatcher.end();

			// Delete Controller Registers first, because deleting the PCI registers first could lead to the ControllerRegisters loop segfaulting
			if (ControllerRegisters)
			{
				delete ControllerRegisters;
				ControllerRegisters = nullptr;
			}

			if (PCIExpressRegisters)
			{
				delete PCIExpressRegisters;
				PCIExpressRegisters = nullptr;
			}

			for (Queue* q : this->ValidSubmissionQueues)
			{
				delete q;
			}

			for (Queue* q : this->ValidCompletionQueues)
			{
				delete q;
			}
		}

		cnvme::controller::registers::ControllerRegisters* Controller::getControllerRegisters()
		{
			return ControllerRegisters;
		}

		pci::PCIExpressRegisters* Controller::getPCIExpressRegisters()
		{
			return PCIExpressRegisters;
		}

		void Controller::checkForChanges()
		{
			auto controllerRegisters = ControllerRegisters->getControllerRegisters();
			// Admin doorbell is right after the Controller Registers... though we may not have it yet
			controller::registers::QUEUE_DOORBELLS* doorbells = getControllerRegisters()->getQueueDoorbells();

			if (controllerRegisters->CSTS.RDY == 0)
			{
				return; // Not ready... Don't do anything.
			}

			if (controllerRegisters->ASQ.ASQB == 0)
			{
				return; // Don't have a admin submission queue address
			}

			// Now that we have a SQ address, make it valid
			if (ValidSubmissionQueues.size() == 0)
			{
				ValidSubmissionQueues.push_back(new Queue(controllerRegisters->AQA.ASQS + 1, ADMIN_QUEUE_ID, &doorbells[ADMIN_QUEUE_ID].SQTDBL.SQT, controllerRegisters->ASQ.ASQB));
			}
			else
			{
				// Check if ASQB matches ValidSubmissionQueues
				Queue* adminQueue = getQueueWithId(ValidSubmissionQueues, ADMIN_QUEUE_ID);
				ASSERT_IF(!adminQueue, "Couldn't find the admin submission queue! Though ValidSubmissionQueue size is not 0.");
				adminQueue->setMemoryAddress(controllerRegisters->ASQ.ASQB);
			}

			if (controllerRegisters->ACQ.ACQB == 0)
			{
				return; // Don't have a admin completion queue address
			}

			// Now that we have a CQ address, make it valid
			if (ValidCompletionQueues.size() == 0)
			{
				Queue* AdminCompletionQueue = new Queue(controllerRegisters->AQA.ACQS + 1, ADMIN_QUEUE_ID, &doorbells[ADMIN_QUEUE_ID].CQHDBL.CQH, controllerRegisters->ACQ.ACQB);
				AdminCompletionQueue->setMappedQueue(ValidSubmissionQueues[ADMIN_QUEUE_ID]); // Map CQ -> SQ
				ValidCompletionQueues.push_back(AdminCompletionQueue);

				Queue* adminSubQ = getQueueWithId(ValidSubmissionQueues, ADMIN_QUEUE_ID);
				ASSERT_IF(!adminSubQ, "Couldn't find the admin submission queue, to link it to the admin completion queue!");
				adminSubQ->setMappedQueue(ValidCompletionQueues[ADMIN_QUEUE_ID]); // Map SQ -> CQ
			}
			else
			{
				// Check if ACQB matches ValidCompletionQueues
				Queue* adminQueue = getQueueWithId(ValidCompletionQueues, ADMIN_QUEUE_ID);
				ASSERT_IF(!adminQueue, "Couldn't find the admin completion queue! Though ValidCompletionQueues size is not 0.");
				adminQueue->setMemoryAddress(controllerRegisters->ACQ.ACQB);
			}

			// Made it this far, we have at least the admin queue
			// This is round-robin right now
			size_t currentValidSubmissionQueuesSize = this->ValidSubmissionQueues.size();
			for (size_t idx = 0; idx < currentValidSubmissionQueuesSize; idx++)
			{
				// Using this instead of foreach since the ValidSubmission/Completion Queues can change at runtime.
				auto sq = this->ValidSubmissionQueues[idx];

				if (doorbells[sq->getQueueId()].SQTDBL.SQT != sq->getTailPointer())
				{
					if (!sq->setTailPointer(doorbells[sq->getQueueId()].SQTDBL.SQT)) // Set our internal Queue instance's tail
					{
						LOG_ERROR("Should trigger AER since the Tail pointer given was invalid"); // Stop early.
						continue;
					}
					else
					{
						sq->getMappedQueue()->setTailPointer(sq->getTailPointer());  // Set in internal CQ as well
					}

					while (sq->getHeadPointer() != sq->getTailPointer())
					{
						processCommandAndPostCompletion(*sq);
						sq->incrementAndGetHeadCloserToTail();
					}

					// If the number of queues changed... maybe one got deleted or lost?
					//  for safety, just break from this loop so that currentValidSubmissionQueuesSize gets updated on the next go.
					if (currentValidSubmissionQueuesSize != this->ValidSubmissionQueues.size())
					{
						break;
					}
				}
			}
		}

		void Controller::processCommandAndPostCompletion(Queue &submissionQueue)
		{
			Queue* theCompletionQueue = submissionQueue.getMappedQueue();
			if (!theCompletionQueue)
			{
				ASSERT("Submission Queue " + std::to_string(submissionQueue.getQueueId()) + " doesn't have a mapped completion queue. And yet it recieved a command.");
				return;
			}

			NVME_COMMAND* command = (NVME_COMMAND*)submissionQueue.getMemoryAddress();  // This is the address of the 64 byte command
			command += submissionQueue.getHeadPointer();                                // Make sure we get the correct command

			bool shouldWeProcessThisCommand = true;
			COMPLETION_QUEUE_ENTRY completionQueueEntryToPost = { 0 };

			// Allow passing to CRAPI
			if (!this->handledByCommandResponseApiFile(*command, completionQueueEntryToPost, submissionQueue.getQueueId()))
			{
				if (!isValidCommandIdentifier(command->DWord0Breakdown.CID, submissionQueue.getQueueId()))
				{
					completionQueueEntryToPost.SC = constants::status::codes::generic::COMMAND_ID_CONFLICT; // Command ID Conflict 
					completionQueueEntryToPost.DNR = 1;                                                     // Do not retry
					shouldWeProcessThisCommand = false;                                                     // Do not process this command later on
				}

				LOG_INFO("Controller got a command:\n" + command->toString());

				if (submissionQueue.getQueueId() == ADMIN_QUEUE_ID && shouldWeProcessThisCommand)
				{
					LOG_INFO("That was an Admin command!");

					// AdminCommandCallers goes from OpCode to Function to call. 
					//  All functions to call must have the same parameters and return value (no return since they are voids)
					auto itr = this->AdminCommandCallers.find(command->DWord0Breakdown.OPC);
					if (itr != this->AdminCommandCallers.end())
					{
						NVMeCaller caller = itr->second;
						(this->*caller)(*command, completionQueueEntryToPost);
					}
					else
					{
						// We don't have handling for this command
						LOG_INFO("Unknown Admin command recv'd by the controller.");
						completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_COMMAND_OPCODE; // Unsupported Opcode
						completionQueueEntryToPost.DNR = 1;                                                        // Do not retry
					}
				}
				else if (shouldWeProcessThisCommand) // NVM Command
				{
					LOG_INFO("That was an NVM command!");

					// AdminCommandCallers goes from OpCode to Function to call. 
					//  All functions to call must have the same parameters and return value (no return since they are voids)
					auto itr = this->NVMCommandCallers.find(command->DWord0Breakdown.OPC);
					if (itr != this->NVMCommandCallers.end())
					{
						NVMeCaller caller = itr->second;
						(this->*caller)(*command, completionQueueEntryToPost);
					}
					else
					{
						// We don't have handling for this command
						LOG_INFO("Unknown NVM command recv'd by the controller.");
						completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_COMMAND_OPCODE; // Unsupported Opcode
						completionQueueEntryToPost.DNR = 1;                                                        // Do not retry
					}
				}
			}
			postCompletion(*theCompletionQueue, completionQueueEntryToPost, command);
		}

		Queue* Controller::getQueueWithId(std::vector<Queue*> &queues, UINT_16 id)
		{
			for (size_t i = 0; i < queues.size(); i++)
			{
				if (queues[i]->getQueueId() == id)
				{
					return queues[i];
				}
			}

			LOG_INFO("Invalid queue id specified: " + std::to_string(id));
			return nullptr;
		}

		void Controller::postCompletion(Queue &completionQueue, COMPLETION_QUEUE_ENTRY completionEntry, NVME_COMMAND* command)
		{
			COMPLETION_QUEUE_ENTRY* completionQueueList = (COMPLETION_QUEUE_ENTRY*)MEMORY_ADDRESS_TO_8POINTER(completionQueue.getMemoryAddress());
			ASSERT_IF(completionQueueList == nullptr, "completionQueueList cannot be NULL");
			LOG_INFO("About to post completion to queue " + std::to_string(completionQueue.getQueueId()) + ". Head (just before moving): " +
				std::to_string(completionQueue.getHeadPointer()) + ". Tail: " + std::to_string(completionQueue.getTailPointer()));

			completionQueueList += completionQueue.getHeadPointer(); // Move pointer to correct index

			Queue* submissionQueue = completionQueue.getMappedQueue();
			ASSERT_IF(!submissionQueue, "Submission queue is NULL!");

			completionEntry.SQID = submissionQueue->getQueueId();
			completionEntry.SQHD = submissionQueue->getHeadPointer();
			completionEntry.CID = command->DWord0Breakdown.CID;

			bool phaseTag = false;
			auto node = QueueToPhaseTag.find(submissionQueue->getQueueId());
			if (node == QueueToPhaseTag.end())
			{
				QueueToPhaseTag[submissionQueue->getQueueId()] = false;    // start at false if not there... though it will flip to true below
			}
			else
			{
				phaseTag = QueueToPhaseTag[submissionQueue->getQueueId()]; // use existing
			}

			if (completionQueue.getHeadPointer() == 0) // need to flip
			{
				bool oldPhaseTag = QueueToPhaseTag[submissionQueue->getQueueId()];
				QueueToPhaseTag[submissionQueue->getQueueId()] = !oldPhaseTag;
				LOG_INFO("Inverting Phase Tag. Now Phase Tag == " + strings::toString(!oldPhaseTag));
			}
			phaseTag = QueueToPhaseTag[submissionQueue->getQueueId()]; // should be ready to be used (and flipped if needed).

			completionEntry.P = (UINT_16)phaseTag;

			UINT_32 completionQueueMemorySize = completionQueue.getQueueMemorySize();
			completionQueueMemorySize -= (completionQueue.getHeadPointer() * sizeof(COMPLETION_QUEUE_ENTRY)); // calculate new remaining memory size
			ASSERT_IF(completionQueueMemorySize < sizeof(COMPLETION_QUEUE_ENTRY), "completionQueueMemorySize must be greater than a single completion queue entry");
			memcpy_s(completionQueueList, completionQueueMemorySize, &completionEntry, sizeof(completionEntry)); // Post
			LOG_INFO(completionEntry.toString());

			completionQueue.incrementAndGetHeadCloserToTail(); // Move up CQ head

			// ring doorbell after placing data in completion queue.
			UINT_16* dbell = completionQueue.getDoorbell();
			dbell[0] = completionQueue.getHeadPointer();
		}

		bool Controller::isValidCommandIdentifier(UINT_16 commandId, UINT_16 submissionQueueId)
		{
			if (SubmissionQueueIdToCommandIdentifiers.find(submissionQueueId) == SubmissionQueueIdToCommandIdentifiers.end())
			{
				// SQID hasn't been used yet?
				std::set<UINT_16> tmp = { commandId };
				SubmissionQueueIdToCommandIdentifiers[submissionQueueId] = tmp;
				return true;
			}
			else
			{
				// Have SQID in SubmissionQueueIdToCommandIdentifiers
				std::set<UINT_16> &thisSubqsCids = SubmissionQueueIdToCommandIdentifiers[submissionQueueId];

				if (thisSubqsCids.size() == MAX_COMMAND_IDENTIFIER)
				{
					LOG_INFO("Made it to the end of possible CIDs for SubQ " + std::to_string(submissionQueueId) + ". Resetting possibile CIDs.");
					thisSubqsCids.clear();
				}

				if (thisSubqsCids.find(commandId) == thisSubqsCids.end())
				{
					thisSubqsCids.insert(commandId);
					return true;
				}
			}

			LOG_ERROR("Invalid command identifier was sent (" + std::to_string(commandId) + "). Was it re-used?");
			return false;
		}

		void Controller::resetIdentifyController()
		{
			auto pciRegistersWrapper = this->getPCIExpressRegisters();
			if (pciRegistersWrapper)
			{
				auto pciRegisters = pciRegistersWrapper->getPciExpressRegisters();
				this->IdentifyController.VID = pciRegisters.PciHeader->ID.VID;
				this->IdentifyController.SSID = pciRegisters.PciHeader->ID.DID;
			}

			memcpy_s(&this->IdentifyController.SN, sizeof(this->IdentifyController.SN), DEFAULT_SERIAL, strlen(DEFAULT_SERIAL));
			memcpy_s(&this->IdentifyController.MN, sizeof(this->IdentifyController.MN), DEFAULT_MODEL, strlen(DEFAULT_MODEL));
			memcpy_s(&this->IdentifyController.FR, sizeof(this->IdentifyController.FR), DEFAULT_FIRMWARE, strlen(DEFAULT_FIRMWARE));

			auto controllerRegistersWrapper = this->getControllerRegisters();
			if (controllerRegistersWrapper)
			{
				auto controllerRegisters = controllerRegistersWrapper->getControllerRegisters();
				if (controllerRegisters)
				{
					memcpy_s(&this->IdentifyController.VER, sizeof(this->IdentifyController.VER), &controllerRegisters->VS, sizeof(controllerRegisters->VS));
				}
			}

			this->IdentifyController.MaxSubmissionQueueEntrySize = DEFAULT_SUBMISSION_QUEUE_ENTRY_SIZE;
			this->IdentifyController.RequiredSubmissionQueueEntrySize = DEFAULT_SUBMISSION_QUEUE_ENTRY_SIZE;

			this->IdentifyController.MaxCompletionQueueEntrySize = DEFAULT_COMPLETION_QUEUE_ENTRY_SIZE;
			this->IdentifyController.RequiredCompletionQueueEntrySize = DEFAULT_COMPLETION_QUEUE_ENTRY_SIZE;

			this->IdentifyController.NN = DEFAULT_MAX_NAMESPACES;
			this->IdentifyController.AVSCC = 1; // All VU commands must have DW10 be the NUMD

			// Optional Commands Supported
			this->IdentifyController.FormatNVMSupported = true;
			this->IdentifyController.FirmwareDownloadAndCommitSupported = true;

			// Optional Features Supported
			this->IdentifyController.FirmwareActivationWithoutResetSupported = true;
			this->IdentifyController.NumberOfFirmwareSlots = constants::commands::identify::sizes::MAX_FW_SLOTS; // Support 7 FW slots

			// Also I'm not setting the power state to anything. It lets us get away with all 0s for not reported. Wow.
		}

		Payload Controller::getNamespaceListFromMap(std::map<UINT_32, ns::Namespace> namespaceMap, UINT_32 startingNsid, COMPLETION_QUEUE_ENTRY& completionQueueEntryToPost)
		{
			Payload transferPayload(constants::commands::identify::sizes::MAX_NSID_IN_NAMESPACE_LIST * sizeof(UINT_32));

			UINT_32 maxNsid = namespaceMap.rbegin()->first;

			if (startingNsid > maxNsid)
			{
				completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_NAMESPACE_OR_FORMAT;
				completionQueueEntryToPost.DNR = 1;
			}
			else
			{
				UINT_32 counter = 0;
				UINT_32* outputNsids = (UINT_32*)transferPayload.getBuffer();

				for (auto &nsidToNs : namespaceMap)
				{
					if (counter == constants::commands::identify::sizes::MAX_NSID_IN_NAMESPACE_LIST)
					{
						break; // we've hit the max
					}

					if (nsidToNs.first >= startingNsid)
					{
						*outputNsids = nsidToNs.first;
						outputNsids++;
						counter++;
					}
				}
			}

			return transferPayload;
		}

		std::map<UINT_32, ns::Namespace> Controller::getAllocatedNamespaceMap() const
		{
			std::map<UINT_32, ns::Namespace> tmp;
			for (auto &i : this->NamespaceIdToActiveNamespace)
			{
				tmp[i.first] = i.second;
			}
			for (auto &i : this->NamespaceIdToInactiveNamespace)
			{
				tmp[i.first] = i.second;
			}
			return tmp;
		}

		bool Controller::handledByCommandResponseApiFile(NVME_COMMAND& nvmeCommand, COMPLETION_QUEUE_ENTRY& completionQueueEntry, UINT_16 SQID)
		{
			if (this->CommandResponseApiFilePath.size() != 0)
			{
				// get the folder from the file:
				std::string folderPath = this->CommandResponseApiFilePath.substr(0, this->CommandResponseApiFilePath.find_last_of("\\/"));
				std::string commandBinPath = folderPath + "/command.bin";
				std::string dataPayloadBinPath = folderPath + "/data_payload.bin";
				std::string completionBinPath = folderPath + "/completion.bin";

				UINT_32 assumedSectorSize = DEFAULT_SECTOR_SIZE; // default

				// Technically we are limiting the user here if they try to fake IO to fake namespaces... 
				//  I don't really see a way around that since the driver would have had difficulty calculating a transfer size as well.
				//   If we can't figure it out, assume its 512 since that is a normal (and small) value.
				auto namespaceIdToNamespaceObject = this->NamespaceIdToActiveNamespace.find(nvmeCommand.NSID);
				if (namespaceIdToNamespaceObject != this->NamespaceIdToActiveNamespace.end())
				{
					auto identifyNamespace = namespaceIdToNamespaceObject->second.getIdentifyNamespaceStructure();

					// calculate sector size per spec
					assumedSectorSize = (UINT_32)std::pow(2, identifyNamespace.LBAF[identifyNamespace.FLBAS.CurrentLBAFormat].LBADS);
				}

				UINT_64 transferSizeInBytes = nvmeCommand.getTransferSizeBytes(SQID == ADMIN_QUEUE_ID, assumedSectorSize);

				// grab data from PRPs
				PRP prps(nvmeCommand.DPTR.DPTR1, nvmeCommand.DPTR.DPTR2, (size_t)transferSizeInBytes, this->getControllerRegisters()->getMemoryPageSize());
				auto transferData = prps.getPayloadCopy();

				// Write command binary
				// These brackets force the ofstreams to go out of scope and get closed.
				{
					std::ofstream commandOfstream(commandBinPath, std::ios::out | std::ios::binary);
					commandOfstream.write((char*)&nvmeCommand, sizeof(nvmeCommand));

					// Write data binary
					std::ofstream dataPayloadOfstream(dataPayloadBinPath, std::ios::out | std::ios::binary);
					dataPayloadOfstream.write((char*)transferData.getBuffer(), transferData.getSize());

					// Write completion binary
					std::ofstream completionOfstream(completionBinPath, std::ios::out | std::ios::binary);
					completionOfstream.write((char*)&completionQueueEntry, sizeof(completionQueueEntry));
				}

				int retCode = system(("\"" + this->CommandResponseApiFilePath + "\" " + std::to_string(SQID)).c_str());

				// Initialize this here to read of we get ASSERT_FROM_CRAPI
				std::ifstream dataPayloadIstream(dataPayloadBinPath, std::ios::in | std::ios::binary);

				switch (retCode)
				{
				case constants::crapi::CRAPI_HANDLED:
					break;
				case constants::crapi::CNVME_HANDLED:
					return false;
				case constants::crapi::ASSERT_FROM_CRAPI:
					dataPayloadIstream.read((char*)transferData.getBuffer(), transferData.getSize());
					ASSERT("CRAPI ASSERT: " + std::string((char*)transferData.getBuffer(), strnlen((char*)transferData.getBuffer(), transferData.getSize())));
				default:
					ASSERT("Unknown CRAPI return code: " + std::to_string(retCode));
				}

				// If we get here then the CRAPI-F handled the command, and returned the correct return code to denote that.

				std::ifstream commandIstream(commandBinPath, std::ios::in | std::ios::binary);
				commandIstream.read((char*)&nvmeCommand, sizeof(nvmeCommand));

				// Read data binary
				dataPayloadIstream.read((char*)transferData.getBuffer(), transferData.getSize());

				// Read completion binary
				std::ifstream completionIstream(completionBinPath, std::ios::in | std::ios::binary);
				completionIstream.read((char*)&completionQueueEntry, sizeof(completionQueueEntry));

				// Put read-in data into PRPs
				prps.placePayloadInExistingPRPs(transferData);

				return true;
			}

			return false;
		}

		void Controller::replaceRunningFirmwareWithOneInSlot(UINT_8 firmwareSlot)
		{
			auto fwSlotToPayload = this->FirmwareSlotToFirmwareImagePayload.find(firmwareSlot);
			ASSERT_IF(fwSlotToPayload == this->FirmwareSlotToFirmwareImagePayload.end(), "Attempting to replace running FW with invalid FW Slot");

			auto &fwPayload = fwSlotToPayload->second;

			ASSERT_IF(memcmp(fwPayload.getBuffer(), FIRMWARE_EYE_CATCHER, sizeof(FIRMWARE_EYE_CATCHER)) != 0, "Attempting to activate an invalid FW image (missing eye catcher). We commited the image without verifying the eye catcher.");
			
			UINT_8 firmwareNameSize = sizeof(identify::structures::IDENTIFY_CONTROLLER::FR);
			ASSERT_IF(fwPayload.getSize() < firmwareNameSize + sizeof(FIRMWARE_EYE_CATCHER), "FW Payload is too small for it to fit the firmware name and eye catcher");

			// Copy the firmware name/revision to IC.FR since we are activating this one.
			// The firmware revision/name is the last 8 bytes of the binary data.
			std::string firmwareName((char*)fwPayload.getBuffer() + (fwPayload.getSize() - firmwareNameSize), firmwareNameSize);
			memcpy_s(&this->IdentifyController.FR, sizeof(this->IdentifyController.FR), firmwareName.c_str(), firmwareName.size());

			this->ActiveFirmwareSlot = firmwareSlot;
		}

		NVME_CALLER_IMPLEMENTATION(adminIdentify)
		{
			UINT_32 memoryPageSize = ControllerRegisters->getMemoryPageSize();

			// Do we have a PRP?
			if (command.DPTR.DPTR1)
			{
				PRP prp(command.DPTR.DPTR1, command.DPTR.DPTR2, memoryPageSize, memoryPageSize);
				auto transferPayload = prp.getPayloadCopy();
				transferPayload.clear();

				if (command.DW10_Identify.CNS == constants::commands::identify::cns::CONTROLLER)                // Identify Controller
				{
					memcpy_s(transferPayload.getBuffer(), transferPayload.getSize(), &IdentifyController, sizeof(IdentifyController));
				}
				else if (command.DW10_Identify.CNS == constants::commands::identify::cns::NAMESPACE_ACTIVE)     // Identify Namespace (active)
				{
					auto namespaceSelected = this->NamespaceIdToActiveNamespace.find(command.NSID);
					if (namespaceSelected != this->NamespaceIdToActiveNamespace.end())
					{
						LOG_INFO("Grabbing Identify Namespace for NSID " + std::to_string(namespaceSelected->first));
						auto identifyNamespaceStructure = namespaceSelected->second.getIdentifyNamespaceStructure();
						memcpy_s(transferPayload.getBuffer(), transferPayload.getSize(), &identifyNamespaceStructure, sizeof(identifyNamespaceStructure));
					}
					else
					{
						// See Figure 106 of NVMe 1.3 for details:
						//   If the specified namespace is not an active NSID, then the controller returns a zero filled data structure.
					}
				}
				else if (command.DW10_Identify.CNS == constants::commands::identify::cns::NAMESPACES_ALLOCATED) // Identify Namespace (allocated)
				{
					auto namespaceMap = this->getAllocatedNamespaceMap();
					auto namespaceSelected = namespaceMap.find(command.NSID);
					if (namespaceSelected != namespaceMap.end())
					{
						LOG_INFO("Grabbing Identify Namespace for NSID " + std::to_string(namespaceSelected->first));
						auto identifyNamespaceStructure = namespaceSelected->second.getIdentifyNamespaceStructure();
						memcpy_s(transferPayload.getBuffer(), transferPayload.getSize(), &identifyNamespaceStructure, sizeof(identifyNamespaceStructure));
					}
					else
					{
						// Invalid namespace specified
						completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_NAMESPACE_OR_FORMAT;
						completionQueueEntryToPost.DNR = 1;
					}
				}
				else if (command.DW10_Identify.CNS == constants::commands::identify::cns::NAMESPACES_ACTIVE)    // Idenitfy Namespace Active List
				{
					transferPayload = this->getNamespaceListFromMap(this->NamespaceIdToActiveNamespace, command.NSID, completionQueueEntryToPost);
				}
				else if (command.DW10_Identify.CNS == constants::commands::identify::cns::NAMESPACES_ALL)       // Identify Namespace All List
				{
					// make map with active/inactive
					std::map<UINT_32, ns::Namespace> tmp = this->getAllocatedNamespaceMap();

					transferPayload = this->getNamespaceListFromMap(tmp, command.NSID, completionQueueEntryToPost);
				}
				else if (command.DW10_Identify.CNS == constants::commands::identify::cns::NAMESPACE_DESCRIPTOR) // Identify Namespace Descriptor List
				{
					auto namespaceSelected = this->NamespaceIdToActiveNamespace.find(command.NSID);
					if (namespaceSelected != this->NamespaceIdToActiveNamespace.end())
					{
						LOG_INFO("Grabbing Namespace Descriptor List for NSID " + std::to_string(namespaceSelected->first));
						transferPayload = namespaceSelected->second.getIdentifyNamespaceDescriptorList();
					}
					else
					{
						// Invalid namespace specified
						completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_NAMESPACE_OR_FORMAT;
						completionQueueEntryToPost.DNR = 1;
					}
				}
				else
				{
					// I don't know what you wanted.
					completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_FIELD_IN_COMMAND;
					completionQueueEntryToPost.DNR = 1;
				}

				if (completionQueueEntryToPost.succeeded())
				{
					prp.placePayloadInExistingPRPs(transferPayload);
				}
			}
			else
			{
				// No PRP? Huh? Fail.
				completionQueueEntryToPost.SC = constants::status::codes::generic::PRP_OFFSET_INVALID;
				completionQueueEntryToPost.DNR = 1;
			}
		}

		NVME_CALLER_IMPLEMENTATION(adminCreateIoCompletionQueue)
		{
			auto controllerRegistersPointer = ControllerRegisters->getControllerRegisters();
			ASSERT_IF(!controllerRegistersPointer, "The controller registers are NULL... oh no!");

			// Check for the Physically Contiguous field. Also make sure the interrupt is enabled. If not, fail the command.
			if (command.DW11_CreateIoCompletionQueue.PC != controllerRegistersPointer->CAP.CQR || !command.DW11_CreateIoCompletionQueue.IEN)
			{
				completionQueueEntryToPost.DNR = 1; // Do Not Retry
				completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_FIELD_IN_COMMAND;
				return;
			}

			// Check to make sure we were give something that looks like a valid address
			if (command.DPTR.DPTR1 == 0)
			{
				completionQueueEntryToPost.DNR = 1; // Do Not Retry
				completionQueueEntryToPost.SC = constants::status::codes::generic::PRP_OFFSET_INVALID;
				return;
			}

			// Check if the queue exists. If it does already, fail the command
			if (this->getQueueWithId(this->ValidCompletionQueues, command.DW10_CreateIoQueue.QID) != nullptr)
			{
				completionQueueEntryToPost.DNR = 1; // Do Not Retry
				completionQueueEntryToPost.SCT = constants::status::types::COMMAND_SPECIFIC;
				completionQueueEntryToPost.SC = constants::status::codes::specific::INVALID_QUEUE_IDENTIFIER;
				return;
			}

			// Check if the queue size is valid.
			if (command.DW10_CreateIoQueue.QSIZE > controllerRegistersPointer->CAP.MQES)
			{
				completionQueueEntryToPost.DNR = 1; // Do Not Retry
				completionQueueEntryToPost.SCT = constants::status::types::COMMAND_SPECIFIC;
				completionQueueEntryToPost.SC = constants::status::codes::specific::INVALID_QUEUE_SIZE;
				return;
			}

			// We don't handle actual interupt vectors, so you can't get that status back.

			// Checks passed. Hold onto the queue.

			auto doorbells = this->ControllerRegisters->getQueueDoorbells();
			doorbells += command.DW10_CreateIoQueue.QID; // find our doorbell

			Queue* q = new Queue(ONE_BASED_FROM_ZERO_BASED(command.DW10_CreateIoQueue.QSIZE),
				command.DW10_CreateIoQueue.QID,
				(UINT_16*)&(doorbells->CQHDBL), // doorbell
				command.DPTR.DPTR1
			);
			this->ValidCompletionQueues.push_back(q);

			LOG_INFO("Held onto completion queue with an id of " + std::to_string(command.DW10_CreateIoQueue.QID));
		}

		NVME_CALLER_IMPLEMENTATION(adminCreateIoSubmissionQueue)
		{
			auto controllerRegistersPointer = ControllerRegisters->getControllerRegisters();
			ASSERT_IF(!controllerRegistersPointer, "The controller registers are NULL... oh no!");

			// Check for the Physically Contiguous field. If not, fail the command.
			if (command.DW11_CreateIoCompletionQueue.PC != controllerRegistersPointer->CAP.CQR)
			{
				completionQueueEntryToPost.DNR = 1; // Do Not Retry
				completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_FIELD_IN_COMMAND;
				return;
			}

			// Check to make sure we were give something that looks like a valid address
			if (command.DPTR.DPTR1 == 0)
			{
				completionQueueEntryToPost.DNR = 1; // Do Not Retry
				completionQueueEntryToPost.SC = constants::status::codes::generic::PRP_OFFSET_INVALID;
				return;
			}

			// Check if the queue exists. If it does already, fail the command
			if (this->getQueueWithId(this->ValidSubmissionQueues, command.DW10_CreateIoQueue.QID) != nullptr)
			{
				completionQueueEntryToPost.DNR = 1; // Do Not Retry
				completionQueueEntryToPost.SCT = constants::status::types::COMMAND_SPECIFIC;
				completionQueueEntryToPost.SC = constants::status::codes::specific::INVALID_QUEUE_IDENTIFIER;
				return;
			}

			// Check if the queue size is valid.
			if (command.DW10_CreateIoQueue.QSIZE > controllerRegistersPointer->CAP.MQES)
			{
				completionQueueEntryToPost.DNR = 1; // Do Not Retry
				completionQueueEntryToPost.SCT = constants::status::types::COMMAND_SPECIFIC;
				completionQueueEntryToPost.SC = constants::status::codes::specific::INVALID_QUEUE_SIZE;
				return;
			}

			// Make sure we have a CQ available for mapping
			Queue* mappedCompletionQueue = this->getQueueWithId(this->ValidCompletionQueues, command.DW11_CreateIoSubmissionQueue.CQID);
			if (mappedCompletionQueue == nullptr)
			{
				completionQueueEntryToPost.DNR = 1; // Do Not Retry
				completionQueueEntryToPost.SCT = constants::status::types::COMMAND_SPECIFIC;
				completionQueueEntryToPost.SC = constants::status::codes::specific::COMPLETION_QUEUE_INVALID;
				return;
			}

			// That completion queue is already mapped... we can't map to this SQ.
			if (mappedCompletionQueue->getMappedQueue())
			{
				completionQueueEntryToPost.DNR = 1; // Do Not Retry
				completionQueueEntryToPost.SCT = constants::status::types::COMMAND_SPECIFIC;
				completionQueueEntryToPost.SC = constants::status::codes::specific::INVALID_QUEUE_IDENTIFIER;
			}

			// Checks passed. Hold onto the queue.

			auto doorbells = this->ControllerRegisters->getQueueDoorbells();
			doorbells += command.DW10_CreateIoQueue.QID; // find our doorbell

			Queue* subQ = new Queue(ONE_BASED_FROM_ZERO_BASED(command.DW10_CreateIoQueue.QSIZE),
				command.DW10_CreateIoQueue.QID,
				(UINT_16*)&(doorbells->CQHDBL), // doorbell
				command.DPTR.DPTR1
			);
			this->ValidSubmissionQueues.push_back(subQ);

			subQ->setMappedQueue(mappedCompletionQueue); // SQ -> CQ
			mappedCompletionQueue->setMappedQueue(subQ); // CQ -> SQ

			LOG_INFO("Held onto submission queue with an id of " + std::to_string(command.DW10_CreateIoQueue.QID));
		}

		NVME_CALLER_IMPLEMENTATION(adminDeleteIoCompletionQueue)
		{
			Queue* q = this->getQueueWithId(this->ValidCompletionQueues, command.DW10_DeleteIoQueue.QID);

			// You can't delete my admin queue!
			if (command.DW10_DeleteIoQueue.QID == ADMIN_QUEUE_ID || !q)
			{
				completionQueueEntryToPost.DNR = 1; // Do Not Retry
				completionQueueEntryToPost.SCT = constants::status::types::COMMAND_SPECIFIC;
				completionQueueEntryToPost.SC = constants::status::codes::specific::INVALID_QUEUE_IDENTIFIER;
				return;
			}

			// We are still mapped to a submission queue.
			// Submission queues must be deleted first
			if (q->getMappedQueue())
			{
				completionQueueEntryToPost.DNR = 1; // Do Not Retry
				completionQueueEntryToPost.SCT = constants::status::types::COMMAND_SPECIFIC;
				completionQueueEntryToPost.SC = constants::status::codes::specific::INVALID_QUEUE_DELETION;
				return;
			}

			// free the memory!
			delete q;

			// Remove from validity
			this->ValidCompletionQueues.erase(std::remove(this->ValidCompletionQueues.begin(), this->ValidCompletionQueues.end(), q), this->ValidCompletionQueues.end());
		}

		NVME_CALLER_IMPLEMENTATION(adminDeleteIoSubmissionQueue)
		{
			Queue* q = this->getQueueWithId(this->ValidSubmissionQueues, command.DW10_DeleteIoQueue.QID);

			// You can't delete my admin queue!
			if (command.DW10_DeleteIoQueue.QID == ADMIN_QUEUE_ID || !q)
			{
				completionQueueEntryToPost.DNR = 1; // Do Not Retry
				completionQueueEntryToPost.SCT = constants::status::types::COMMAND_SPECIFIC;
				completionQueueEntryToPost.SC = constants::status::codes::specific::INVALID_QUEUE_IDENTIFIER;
				return;
			}

			// don't let the completion queue map here anymore
			q->getMappedQueue()->setMappedQueue(nullptr);

			// free the memory!
			delete q;

			// Remove from validity
			this->ValidSubmissionQueues.erase(std::remove(this->ValidSubmissionQueues.begin(), this->ValidSubmissionQueues.end(), q), this->ValidSubmissionQueues.end());
			this->SubmissionQueueIdToCommandIdentifiers[command.DW10_DeleteIoQueue.QID].clear();
		}

		NVME_CALLER_IMPLEMENTATION(adminFirmwareCommit)
		{
			if (!this->IdentifyController.FirmwareDownloadAndCommitSupported)
			{
				completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_COMMAND_OPCODE;
				completionQueueEntryToPost.DNR = 1;
				return;
			}

			using namespace constants::commands::fw_commit::commit_action;

			if (command.DW10_FirmwareCommit.FS > this->IdentifyController.NumberOfFirmwareSlots)
			{
				completionQueueEntryToPost.SCT = constants::status::types::COMMAND_SPECIFIC;
				completionQueueEntryToPost.SC = constants::status::codes::specific::INVALID_FIRMWARE_SLOT;
				completionQueueEntryToPost.DNR = 1;
				return;
			}
			
			UINT_8 firmwareSlot = std::max((UINT_32)1, command.DW10_FirmwareCommit.FS); // If we get a slot of 0, we chose a slot... so we pick 1.

			if (command.DW10_FirmwareCommit.CA == REPLACE_IN_SLOT_NO_ACTIVATE || command.DW10_FirmwareCommit.CA == REPLACE_IN_SLOT_AND_ACTIVATE_ON_RESET)
			{
				UINT_32 currentDwOffset = 0;
				Payload completeFirmwareBinary;

				// Check if this->FirmwareImageDWordOffsetToData is valid and contiguous.
				for (auto &fwDwOffsetToData : this->FirmwareImageDWordOffsetToData)
				{
					if (fwDwOffsetToData.first != currentDwOffset)
					{
						LOG_INFO("Attempted to commit a FW to a FW slot without a contiguous FW binary");
						completionQueueEntryToPost.SCT = constants::status::types::COMMAND_SPECIFIC;
						completionQueueEntryToPost.SC = constants::status::codes::specific::INVALID_FIRMWARE_IMAGE;
						return;
					}
					currentDwOffset += fwDwOffsetToData.second.getSize() * sizeof(UINT_32);
					completeFirmwareBinary.append(fwDwOffsetToData.second);
				}

				if (completeFirmwareBinary.getSize() <  (sizeof(identify::structures::IDENTIFY_CONTROLLER::FR) + sizeof(FIRMWARE_EYE_CATCHER)) ||
					memcmp(completeFirmwareBinary.getBuffer(), FIRMWARE_EYE_CATCHER, sizeof(FIRMWARE_EYE_CATCHER) != 0))
				{
					LOG_INFO("Attempt to commit a FW to a slot with an invalid FW buffer");
					completionQueueEntryToPost.SCT = constants::status::types::COMMAND_SPECIFIC;
					completionQueueEntryToPost.SC = constants::status::codes::specific::INVALID_FIRMWARE_IMAGE;
					return;
				}

				this->FirmwareSlotToFirmwareImagePayload[firmwareSlot] = completeFirmwareBinary;

				if (command.DW10_FirmwareCommit.CA == REPLACE_IN_SLOT_AND_ACTIVATE_ON_RESET)
				{
					LOG_INFO("Updating to (new data in) slot " + std::to_string(firmwareSlot) + " after next reset");
					this->FirmwareSlotToActivateOnReset = firmwareSlot;

					// Note: Is this supposed to return fw activation requires reset? I don't think so since the intent is to setup to activate on reset.
				}
			}
			else if (command.DW10_FirmwareCommit.CA == ACTIVATE_GIVEN_SLOT_NOW)
			{
				if (this->FirmwareSlotToFirmwareImagePayload.find(firmwareSlot) == this->FirmwareSlotToFirmwareImagePayload.end())
				{
					LOG_INFO("Attempted to activate a FW slot without a downloaded FW");
					completionQueueEntryToPost.SCT = constants::status::types::COMMAND_SPECIFIC;
					completionQueueEntryToPost.SC = constants::status::codes::specific::INVALID_FIRMWARE_IMAGE;
					return;
				}

				if (this->IdentifyController.FirmwareActivationWithoutResetSupported)
				{
					LOG_INFO("Updating to slot " + std::to_string(firmwareSlot) + " now!");
					this->replaceRunningFirmwareWithOneInSlot(firmwareSlot);
				}
				else
				{
					LOG_INFO("Updating to slot " + std::to_string(firmwareSlot) + " after next reset");
					this->FirmwareSlotToActivateOnReset = firmwareSlot;
					completionQueueEntryToPost.SCT = constants::status::types::COMMAND_SPECIFIC;
					completionQueueEntryToPost.SC = constants::status::codes::specific::FIRMWARE_ACTIVATION_REQUIRES_RESET;
					return;
				}
			}
			else if (command.DW10_FirmwareCommit.CA == ACTIVATE_GIVEN_SLOT_ON_RESET)
			{
				if (this->FirmwareSlotToFirmwareImagePayload.find(firmwareSlot) == this->FirmwareSlotToFirmwareImagePayload.end())
				{
					LOG_INFO("Attempted to activate a FW slot without a downloaded FW");
					completionQueueEntryToPost.SCT = constants::status::types::COMMAND_SPECIFIC;
					completionQueueEntryToPost.SC = constants::status::codes::specific::INVALID_FIRMWARE_IMAGE;
					return;
				}

				LOG_INFO("Updating to slot " + std::to_string(firmwareSlot) + " after next reset");
				this->FirmwareSlotToActivateOnReset = firmwareSlot;

				// Note: Is this supposed to return fw activation requires reset? I don't think so since the intent is to setup to activate on reset.
			}
			else
			{
				// Invalid or unsupported commit action
				completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_FIELD_IN_COMMAND;
				completionQueueEntryToPost.DNR = 1;
			}
		}

		NVME_CALLER_IMPLEMENTATION(adminFirmwareImageDownload)
		{
			if (!this->IdentifyController.FirmwareDownloadAndCommitSupported)
			{
				completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_COMMAND_OPCODE;
				completionQueueEntryToPost.DNR = 1;
				return;
			}

			UINT_32 minOffsetInDwords = command.DWord11;
			UINT_64 transferBytes = command.getTransferSizeBytes(true, 0);

			ASSERT_IF(transferBytes % sizeof(UINT_32) != 0, "transferBytes must be divisible by 4 for FW Image Download");

			UINT_64 maxOffsetInDwords = minOffsetInDwords + (transferBytes / sizeof(UINT_32));

			// Check to see if we should check for a granularity in the data transfer size
			using namespace constants::commands::fw_download::fwug;
			if (this->IdentifyController.FWUG != NO_INFO && this->IdentifyController.FWUG != NO_RESTRICTION)
			{
				UINT_64 fwUpdateGranularityBytes = FW_UPDATE_GRANULARITY_UNIT * this->IdentifyController.FWUG;

				ASSERT_IF(fwUpdateGranularityBytes == 0, "The calculated firmware update granularity cannot be 0!");

				if (transferBytes % fwUpdateGranularityBytes != 0)
				{
					LOG_INFO("Detected a FW update granularity violation!");
					// It seems weird to me that a FWUG violation leads to an overlapping range error. Though that's what Figure 81 of NVMe 1.3 says.
					completionQueueEntryToPost.SCT = constants::status::types::COMMAND_SPECIFIC;
					completionQueueEntryToPost.SC = constants::status::codes::specific::OVERLAPPING_RANGE;
					completionQueueEntryToPost.DNR = 1;
					return;
				}
			}

			// Clear all cached data on an offset of 0.
			if (minOffsetInDwords == 0)
			{
				this->FirmwareImageDWordOffsetToData.clear();
			}

			// Check for overlapping range. 
			for (auto &fwDwOffsetToPayload : this->FirmwareImageDWordOffsetToData)
			{
				auto &dWordOffset = fwDwOffsetToPayload.first;
				auto &payload = fwDwOffsetToPayload.second;

				UINT_64 minCachedOffset = dWordOffset;
				UINT_64 maxCachedOffset = dWordOffset + (payload.getSize() / sizeof(UINT_32));

				// If the range overlaps, fail the command now.
				if ((minOffsetInDwords > minCachedOffset && minOffsetInDwords < maxCachedOffset) || (maxOffsetInDwords < maxCachedOffset && maxOffsetInDwords > minCachedOffset))
				{
					completionQueueEntryToPost.SCT = constants::status::types::COMMAND_SPECIFIC;
					completionQueueEntryToPost.SC = constants::status::codes::specific::OVERLAPPING_RANGE;
					completionQueueEntryToPost.DNR = 1;
					return;
				}
			}

			PRP prps(command.DPTR.DPTR1, command.DPTR.DPTR2, (UINT_32)transferBytes, this->getControllerRegisters()->getMemoryPageSize());
			this->FirmwareImageDWordOffsetToData[minOffsetInDwords] = prps.getPayloadCopy();
		}

		NVME_CALLER_IMPLEMENTATION(adminFormatNvm)
		{
			// Make sure this optional command is supported
			if (!this->IdentifyController.FormatNVMSupported)
			{
				completionQueueEntryToPost.DNR = 1; // Do Not Retry
				completionQueueEntryToPost.SCT = constants::status::types::GENERIC_COMMAND;
				completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_COMMAND_OPCODE;
				return;
			}

			std::set<UINT_32> namespacesToFormat;
			bool shouldFormatAll = (command.NSID == ALL_NAMESPACES);

			// Should format all if IC says format all on crypto so and this is a crypto erase
			shouldFormatAll |= (command.DW10_Format.SES == constants::commands::format::ses::CRYPTOGRAPHIC_ERASE && this->IdentifyController.AllNamespacesErasedOnSecureErase);

			// Should format all if IC says format all on namespaces so and this is not a crypto erase
			shouldFormatAll |= (command.DW10_Format.SES != constants::commands::format::ses::CRYPTOGRAPHIC_ERASE && this->IdentifyController.FormatAppliesToAllNamespaces);

			// If we should format all namespaces, place all of them into the nameespaceToFormat table
			if (shouldFormatAll)
			{
				LOG_INFO("I should format all namespaces!");

				for (auto &i : this->NamespaceIdToActiveNamespace)
				{
					namespacesToFormat.insert(i.first);
				}
			}
			else
			{
				// Make sure we have the namespace.
				auto nsid = this->NamespaceIdToActiveNamespace.find(command.NSID);

				// If we don't have this namespace fail.
				if (nsid == this->NamespaceIdToActiveNamespace.end())
				{
					completionQueueEntryToPost.DNR = 1; // Do Not Retry
					completionQueueEntryToPost.SCT = constants::status::types::GENERIC_COMMAND;
					completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_NAMESPACE_OR_FORMAT;
					return;
				}

				namespacesToFormat.insert(nsid->first);
			}

			// Call format on all namespacesToFormat... if any fail... fail the command
			for (auto &nsid : namespacesToFormat)
			{
				completionQueueEntryToPost = this->NamespaceIdToActiveNamespace[nsid].formatNVM(command);

				if (completionQueueEntryToPost.SC != 0)
				{
					LOG_ERROR("Failed to format NSID " + std::to_string(nsid));
					break; // make sure we leave this loop now.
				}
			}
		}

		NVME_CALLER_IMPLEMENTATION(adminKeepAlive)
		{
			// nop. We do nothing here.
		}

		NVME_CALLER_IMPLEMENTATION(nvmFlush)
		{
			// TODO: See if we can push this code higher up as it applies to all NVM commands.

			/*
			Section 6.1 of NVMe 1.3
			Unless otherwise noted, specifying an inactive namespace ID in a command that uses the namespace ID
			shall cause the controller to abort the command with status Invalid Field in Command. Specifying an invalid
			NSID in a command that uses the NSID field shall cause the controller to abort the command with status
			Invalid Namespace or Format.
			*/

			// We have nothing to flush as everything is always 'safe'.. right?

			auto namespacePair = this->NamespaceIdToActiveNamespace.find(command.NSID);
			if (namespacePair != this->NamespaceIdToActiveNamespace.end())
			{
				return; // We really don't have anything to do. We're done since this is an active NSID.
			}

			// not an active NSID
			completionQueueEntryToPost.DNR = 1; // Do Not Retry

			namespacePair = this->NamespaceIdToInactiveNamespace.find(command.NSID);
			if (namespacePair == this->NamespaceIdToInactiveNamespace.end())
			{
				// User specified an inactive NSID.
				completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_FIELD_IN_COMMAND;
			}

			// User specified an invalid NSID.
			completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_NAMESPACE_OR_FORMAT;
		}

		NVME_CALLER_IMPLEMENTATION(nvmRead)
		{
			// Make sure the namespace exists
			auto namespacePair = this->NamespaceIdToActiveNamespace.find(command.NSID);
			if (namespacePair == this->NamespaceIdToActiveNamespace.end())
			{
				completionQueueEntryToPost.DNR = 1; // Do Not Retry
				completionQueueEntryToPost.SCT = constants::status::types::GENERIC_COMMAND;
				completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_NAMESPACE_OR_FORMAT;
				return;
			}

			// Do we have a PRP?
			if (command.DPTR.DPTR1)
			{
				Payload readData;
				completionQueueEntryToPost = namespacePair->second.read(command, readData);
				PRP prps(command.DPTR.DPTR1, command.DPTR.DPTR2, readData.getSize(), ControllerRegisters->getMemoryPageSize());
				prps.placePayloadInExistingPRPs(readData);
			}
			else
			{
				// No PRP? Huh? Fail.
				completionQueueEntryToPost.SC = constants::status::codes::generic::PRP_OFFSET_INVALID;
				completionQueueEntryToPost.DNR = 1;
			}
		}

		NVME_CALLER_IMPLEMENTATION(nvmWrite)
		{
			// Make sure the namespace exists
			auto namespacePair = this->NamespaceIdToActiveNamespace.find(command.NSID);
			if (namespacePair == this->NamespaceIdToActiveNamespace.end())
			{
				completionQueueEntryToPost.DNR = 1; // Do Not Retry
				completionQueueEntryToPost.SCT = constants::status::types::GENERIC_COMMAND;
				completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_NAMESPACE_OR_FORMAT;
				return;
			}

			// Do we have a PRP?
			if (command.DPTR.DPTR1)
			{
				UINT_32 memoryPageSize = ControllerRegisters->getMemoryPageSize();
				completionQueueEntryToPost = namespacePair->second.write(command, memoryPageSize);
			}
			else
			{
				// No PRP? Huh? Fail.
				completionQueueEntryToPost.SC = constants::status::codes::generic::PRP_OFFSET_INVALID;
				completionQueueEntryToPost.DNR = 1;
			}
		}

		void Controller::controllerResetCallback()
		{
			LOG_INFO("Recv'd a controllerResetCallback request.");

			for (size_t i = ValidSubmissionQueues.size() - 1; i != -1; i--)
			{
				if (ValidSubmissionQueues[i]->getQueueId() != ADMIN_QUEUE_ID)
				{
					delete ValidSubmissionQueues[i];
					ValidSubmissionQueues.erase(ValidSubmissionQueues.begin() + i);
				}
			}

			for (size_t i = ValidCompletionQueues.size() - 1; i != -1; i--)
			{
				if (ValidCompletionQueues[i]->getQueueId() != ADMIN_QUEUE_ID)
				{
					delete ValidCompletionQueues[i];
					ValidCompletionQueues.erase(ValidCompletionQueues.begin() + i);
				}
			}

			// Clear the SubQ to CID listing.
			this->SubmissionQueueIdToCommandIdentifiers.clear();

			// Clear phase tags.
			this->QueueToPhaseTag.clear();

			// Clear FW Image Download Cache
			this->FirmwareImageDWordOffsetToData.clear();

			// If a FW slot is given to update to on reset, do it now.
			if (this->FirmwareSlotToActivateOnReset)
			{
				this->replaceRunningFirmwareWithOneInSlot(this->FirmwareSlotToActivateOnReset);
				this->FirmwareSlotToActivateOnReset = 0;
			}
		}

		void Controller::waitForChangeLoop()
		{
#ifndef SINGLE_THREADED
			DoorbellWatcher.waitForFlip();
#else
			checkForChanges();
#endif
		}

		void Controller::setCommandResponseFilePath(const std::string filePath)
		{
			LOG_INFO("Set CRAPI-F to " + filePath);
			this->CommandResponseApiFilePath = filePath;
		}

		const std::map<UINT_8, NVMeCaller> Controller::AdminCommandCallers = {
			{ cnvme::constants::opcodes::admin::CREATE_IO_COMPLETION_QUEUE, &cnvme::controller::Controller::adminCreateIoCompletionQueue},
			{ cnvme::constants::opcodes::admin::CREATE_IO_SUBMISSION_QUEUE, &cnvme::controller::Controller::adminCreateIoSubmissionQueue},
			{ cnvme::constants::opcodes::admin::DELETE_IO_COMPLETION_QUEUE, &cnvme::controller::Controller::adminDeleteIoCompletionQueue},
			{ cnvme::constants::opcodes::admin::DELETE_IO_SUBMISSION_QUEUE, &cnvme::controller::Controller::adminDeleteIoSubmissionQueue},
			{ cnvme::constants::opcodes::admin::FIRMWARE_COMMIT, &cnvme::controller::Controller::adminFirmwareCommit},
			{ cnvme::constants::opcodes::admin::FIRMWARE_IMAGE_DOWNLOAD, &cnvme::controller::Controller::adminFirmwareImageDownload},
			{ cnvme::constants::opcodes::admin::FORMAT_NVM, &cnvme::controller::Controller::adminFormatNvm},
			{ cnvme::constants::opcodes::admin::IDENTIFY, &cnvme::controller::Controller::adminIdentify},
			{ cnvme::constants::opcodes::admin::KEEP_ALIVE, &cnvme::controller::Controller::adminKeepAlive}
		};

		const std::map<UINT_8, NVMeCaller> Controller::NVMCommandCallers = {
			{ cnvme::constants::opcodes::nvm::FLUSH, &cnvme::controller::Controller::nvmFlush},
			{ cnvme::constants::opcodes::nvm::READ, &cnvme::controller::Controller::nvmRead},
			{ cnvme::constants::opcodes::nvm::WRITE, &cnvme::controller::Controller::nvmWrite}
		};
	}
}
