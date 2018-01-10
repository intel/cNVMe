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
			resetIdentifyController();

			// create default namespace
			this->NamespaceIdToNamespace[1] = ns::Namespace(DEFAULT_NAMESPACE_SIZE);
		}

		Controller::~Controller()
		{
			DoorbellWatcher.end();

			if (PCIExpressRegisters)
			{
				delete PCIExpressRegisters;
				PCIExpressRegisters = nullptr;
			}

			if (ControllerRegisters)
			{
				delete ControllerRegisters;
				ControllerRegisters = nullptr;
			}

			for (Queue* q: this->ValidSubmissionQueues)
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

			Payload transferPayload;
			PRP prp;
			bool shouldWeProcessThisCommand = true;
			COMPLETION_QUEUE_ENTRY completionQueueEntryToPost = { 0 };

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
				auto itr = AdminCommandCallers.find(command->DWord0Breakdown.OPC);
				if (itr != AdminCommandCallers.end())
				{
					NVMeCaller caller = itr->second;
					(this->*caller)(*command, completionQueueEntryToPost);
				}
				else
				{
					// We don't have handling for this command
					LOG_INFO("Unknown command recv'd by the controller.");
					completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_COMMAND_OPCODE; // Unsupported Opcode
					completionQueueEntryToPost.DNR = 1;                                                        // Do not retry
				}
			}
			else if (shouldWeProcessThisCommand) // NVM Command
			{
				LOG_INFO("That was an NVM command!");

				// lol don't do anything yet

				completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_COMMAND_OPCODE; // Unsupported Opcode
				completionQueueEntryToPost.DNR = 1;                                                        // Do not retry
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

			LOG_ERROR("Invalid queue id specified: " + std::to_string(id));
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

			// Optional Commands Supported... none yet.
			// Also I'm not setting the power state to anything. It lets us get away with all 0s for not reported. Wow.
		}

		NVME_CALLER_IMPLEMENTATION(adminIdentify)
		{
			UINT_32 memoryPageSize = ControllerRegisters->getMemoryPageSize();

			// Do we have a PRP?
			if (command.DPTR.DPTR1)
			{
				PRP prp(command.DPTR.DPTR1, command.DPTR.DPTR2, memoryPageSize, memoryPageSize);
				auto transferPayload = prp.getPayloadCopy();

				if (command.DW10_Identify.CNS == constants::commands::identify::cns::CONTROLLER) // Identify Controller
				{
					memcpy_s(transferPayload.getBuffer(), transferPayload.getSize(), &IdentifyController, sizeof(IdentifyController));
				}
				else if (command.DW10_Identify.CNS == constants::commands::identify::cns::NAMESPACE_ACTIVE) // Identify Namespace
				{
					auto namespaceSelected = this->NamespaceIdToNamespace.find(command.NSID);
					if (namespaceSelected != this->NamespaceIdToNamespace.end())
					{
						LOG_INFO("Grabbing Identify Namespace for NSID " + std::to_string(namespaceSelected->first));
						auto identifyNamespaceStructure = namespaceSelected->second.getIdentifyNamespaceStructure();
						memcpy_s(transferPayload.getBuffer(), transferPayload.getSize(), &identifyNamespaceStructure, sizeof(identifyNamespaceStructure));
					}
					else
					{
						memset(transferPayload.getBuffer(), 0, transferPayload.getSize()); // Technically, this is ok if we don't have a namespace with the given NSID
						// See Figure 106 of NVMe 1.3 for details:
						//   If the specified namespace is not an active NSID, then the controller returns a zero filled data structure.
					}
				}
				else
				{
					// I don't know what you wanted.
					completionQueueEntryToPost.SC = constants::status::codes::generic::INVALID_FIELD_IN_COMMAND;
					completionQueueEntryToPost.DNR = 1;
				}
				prp.placePayloadInExistingPRPs(transferPayload);
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

		NVME_CALLER_IMPLEMENTATION(adminKeepAlive)
		{
			// nop. We do nothing here.
		}

		void Controller::controllerResetCallback()
		{
			LOG_INFO("Recv'd a controllerResetCallback request.");

			for (size_t i = ValidSubmissionQueues.size() - 1; i != -1; i--)
			{
				if (ValidSubmissionQueues[i]->getQueueId() != ADMIN_QUEUE_ID)
				{
					ValidSubmissionQueues.erase(ValidSubmissionQueues.begin() + i);
				}
			}

			for (size_t i = ValidCompletionQueues.size() - 1; i != -1; i--)
			{
				if (ValidCompletionQueues[i]->getQueueId() != ADMIN_QUEUE_ID)
				{
					ValidCompletionQueues.erase(ValidCompletionQueues.begin() + i);
				}
			}

			// Clear the SubQ to CID listing.
			this->SubmissionQueueIdToCommandIdentifiers.clear();

			// Clear phase tags.
			this->QueueToPhaseTag.clear();
		}

		void Controller::waitForChangeLoop()
		{
#ifndef SINGLE_THREADED
			DoorbellWatcher.waitForFlip();
#else
			checkForChanges();
#endif
	}

		const std::map<UINT_8, NVMeCaller> Controller::AdminCommandCallers = {
			{ cnvme::constants::opcodes::admin::CREATE_IO_COMPLETION_QUEUE, &cnvme::controller::Controller::adminCreateIoCompletionQueue},
			{ cnvme::constants::opcodes::admin::CREATE_IO_SUBMISSION_QUEUE, &cnvme::controller::Controller::adminCreateIoSubmissionQueue},
			{ cnvme::constants::opcodes::admin::IDENTIFY, &cnvme::controller::Controller::adminIdentify},
			{ cnvme::constants::opcodes::admin::KEEP_ALIVE, &cnvme::controller::Controller::adminKeepAlive}
		};
}
}
