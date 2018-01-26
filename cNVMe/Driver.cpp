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
Driver.cpp - An implementation file for the cNVMe 'Driver'
*/

#include "Driver.h"
#include "Identify.h"
#include "Strings.h"
#include "Tests.h"

#define ADMIN_QUEUE_SIZE 15	// This is 0 based

using namespace cnvme::tests;

namespace cnvme
{
	namespace driver
	{
		std::string statusToString(Status s)
		{
			if (s == SENT_SUCCESSFULLY)
			{
				return "Sent Successfully";
			}
			else if (s == NO_MATCHING_SUBMISSION_QUEUE)
			{
				return "Didn't have a submission queue with the specified id to send the command to";
			}
			else if (s == NO_LINKED_COMPLETION_QUEUE)
			{
				return "Didn't have a linked completion queue with for the specified submission queue id for the drive to post completion to";
			}
			else if (s == TIMEOUT)
			{
				return "The command timed out";
			}
			else if (s == BUFFER_NOT_LARGE_ENOUGH)
			{
				return "The passed in payload was not large enough";
			}
			else if (s == INVALID_DATA_DIRECTION)
			{
				return "The data direction given was invalid";
			}
			else if (s == INVALID_DATA_LENGTH)
			{
				return "The data length was invalid";
			}
			else if (s == INVALID_DATA_LENGTH_FOR_MANUAL_PRPS)
			{
				return "The data length was invalid since the user should be providing their own PRPs embedded in the DWs";
			}
			else if (s == INVALID_IO_QUEUE_MANAGEMENT_PC)
			{
				return "The PC field must be set to 1 as we do not support non physically contiguous queues";
			}
			else if (s == INVALID_IO_QUEUE_MANAGEMENT_IEN)
			{
				return "The IEN field must be set to 1 as we do not support disabled interrupt queues";
			}

			ASSERT("Status not found in statusToString()");
			return "Unknown";
		}

		std::string dataDirectionToString(DataDirection d)
		{
			if (d == NO_DATA)
			{
				return "No Data";
			}
			else if (d == READ)
			{
				return "Read";
			}
			else if (d == WRITE)
			{
				return "Write";
			}
			else if (d == BI_DIRECTIONAL)
			{
				return "Bi-Directional";
			}

			ASSERT("DataDirection not found in dataDirectionToString()");
			return "Unknown";
		}

		Driver::Driver()
		{
			// We have a controller... it is not running.
			auto controllerRegisters = this->TheController.getControllerRegisters()->getControllerRegisters();

			ASSERT_IF(!controllerRegisters, "The controller registers were somehow NULL!");

			// Force it to have an admin queue with 16 entries
			controllerRegisters->AQA.ACQS = ADMIN_QUEUE_SIZE; // 0-based
			controllerRegisters->AQA.ASQS = ADMIN_QUEUE_SIZE; // 0-based

			// Calculate the size of the admin queues
			UINT_32 adminSubmissionQueueByteSize = ONE_BASED_FROM_ZERO_BASED(controllerRegisters->AQA.ASQS) * (UINT_32)pow(2, DEFAULT_SUBMISSION_QUEUE_ENTRY_SIZE);
			UINT_32 adminCompletionQueueByteSize = ONE_BASED_FROM_ZERO_BASED(controllerRegisters->AQA.ACQS) * (UINT_32)pow(2, DEFAULT_COMPLETION_QUEUE_ENTRY_SIZE);

			ASSERT_IF(adminSubmissionQueueByteSize < 64, "The admin submission queue must be at least 64 bytes!");
			ASSERT_IF(adminCompletionQueueByteSize < 16, "The admin completion queue must be at least 16 bytes!");

			// Create the payloads for the admin queues
			Payload adminSubmissionQueuePayload(adminSubmissionQueueByteSize);
			Payload adminCompletionQueuePayload(adminCompletionQueueByteSize);

			// Set the payloads to all 0xFF (to not catch bad CIDs of 0)
			memset(adminSubmissionQueuePayload.getBuffer(), 0xFF, adminSubmissionQueuePayload.getSize());
			memset(adminCompletionQueuePayload.getBuffer(), 0xFF, adminCompletionQueuePayload.getSize());

			// Make sure the payloads stay in scope.
			adminSubmissionQueuePayload.setDeleteOnScopeLoss(false);
			adminCompletionQueuePayload.setDeleteOnScopeLoss(false);

			// Get pointers to the doorbells for the admin queues
			UINT_16* adminSubmissionQueueDoorbell = &this->TheController.getControllerRegisters()->getQueueDoorbells()->SQTDBL.SQT;
			UINT_16* adminCompletionQueueDoorbell = &this->TheController.getControllerRegisters()->getQueueDoorbells()->CQHDBL.CQH;

			// Place the memory addresses in the registers
			controllerRegisters->ASQ.ASQB = adminSubmissionQueuePayload.getMemoryAddress();
			controllerRegisters->ACQ.ACQB = adminCompletionQueuePayload.getMemoryAddress();

			// Make Queue objects to hang onto
			Queue* adminSubmissionQueue = new Queue(ONE_BASED_FROM_ZERO_BASED(controllerRegisters->AQA.ASQS), ADMIN_QUEUE_ID, adminSubmissionQueueDoorbell, controllerRegisters->ASQ.ASQB);
			Queue* adminCompletionQueue = new Queue(ONE_BASED_FROM_ZERO_BASED(controllerRegisters->AQA.ACQS), ADMIN_QUEUE_ID, adminCompletionQueueDoorbell, controllerRegisters->ACQ.ACQB);

			// Add Queue objects to our container
			this->SubmissionQueues[ADMIN_QUEUE_ID] = adminSubmissionQueue;
			this->CompletionQueues[ADMIN_QUEUE_ID] = adminCompletionQueue;

			// Link the Queue objects to each other
			this->SubmissionQueues[ADMIN_QUEUE_ID]->setMappedQueue(this->CompletionQueues[ADMIN_QUEUE_ID]);
			this->CompletionQueues[ADMIN_QUEUE_ID]->setMappedQueue(this->SubmissionQueues[ADMIN_QUEUE_ID]);

			// Enable the controller
			controllerRegisters->CC.EN = 1;

			// Wait for CSTS.RDY to go to 1
			UINT_64 numberOfSecondsMaxToWait = (controllerRegisters->CAP.TO / 2);
			UINT_64 maxWaitTime = helpers::getTimeInMilliseconds() + (numberOfSecondsMaxToWait * 1000);

			bool controllerWentReady = false;
			while (helpers::getTimeInMilliseconds() < maxWaitTime)
			{
				if (controllerRegisters->CSTS.RDY)
				{
					controllerWentReady = true;
					break;
				}
			}

			ASSERT_IF(!controllerWentReady, "Controller did not ready up in time!");

			// If we made it here, the controller is enabled and we are ready for action!
		}

		Driver::~Driver()
		{
			this->deleteAllIoQueues();

			// Delete admin queue
			delete[]MEMORY_ADDRESS_TO_64POINTER(this->SubmissionQueues[0]->getMemoryAddress());
			delete[]MEMORY_ADDRESS_TO_64POINTER(this->CompletionQueues[0]->getMemoryAddress());
			delete this->SubmissionQueues[0];
			delete this->CompletionQueues[0];
		}

		void Driver::sendCommand(UINT_8* driverCommandBuffer, size_t driverCommandBufferSize)
		{
			// Make sure the buffer is large enough
			ASSERT_IF(driverCommandBufferSize < sizeof(Status), "The passed in buffer size wasn't even large enough to return a status");
			DRIVER_COMMAND* pDriverCommand = (DRIVER_COMMAND*)driverCommandBuffer;

			// If we should return an invalid buffer size, do.
			if (driverCommandBufferSize < sizeof(DRIVER_COMMAND) || (driverCommandBufferSize < pDriverCommand->TransferDataSize + sizeof(DRIVER_COMMAND)))
			{
				LOG_ERROR("The provided buffer was not large enough");
				pDriverCommand->DriverStatus = BUFFER_NOT_LARGE_ENOUGH;
				return;
			}

			// If the data direction is invalid, fail now
			if (pDriverCommand->TransferDataDirection >= DATA_DIRECTION_MAX)
			{
				LOG_ERROR("Invalid data direction was provided");
				pDriverCommand->DriverStatus = INVALID_DATA_DIRECTION;
				return;
			}

			// If the data length is invalid, fail now
			if (pDriverCommand->TransferDataSize == 0 && pDriverCommand->TransferDataDirection != NO_DATA)
			{
				LOG_ERROR("Transfer data size was 0 but the data direction is not no-data");
				pDriverCommand->DriverStatus = INVALID_DATA_LENGTH;
				return;
			}

			// If the user gave data but also wanted to passdown their own PRPs
			if (pDriverCommand->TransferDataDirection == MANUAL_PRPS && pDriverCommand->TransferDataSize != 0)
			{
				LOG_ERROR("The user specified that they wanted to create/use their own PRPs, and yet they gave the driver transfer data... Where would it go?");
				pDriverCommand->DriverStatus = INVALID_DATA_LENGTH_FOR_MANUAL_PRPS;
				return;
			}

			// If the user gave Create IO Completion Queue, we need to check the command for what the driver supports
			if (this->commandRequiresContiguousBufferInsteadOfPrp(pDriverCommand->Command, pDriverCommand->QueueId == ADMIN_QUEUE_ID))
			{
				if (pDriverCommand->Command.DW11_CreateIoCompletionQueue.PC != true)
				{
					LOG_ERROR("The user specified a non-contiguous completion queue. We don't support that.");
					pDriverCommand->DriverStatus = INVALID_IO_QUEUE_MANAGEMENT_PC;
					return;
				}

				if (pDriverCommand->Command.DWord0Breakdown.OPC == cnvme::constants::opcodes::admin::CREATE_IO_COMPLETION_QUEUE && \
					pDriverCommand->Command.DW11_CreateIoCompletionQueue.IEN != true)
				{
					LOG_ERROR("The user specified an interrupt-disabled queue. We don't support that.");
					pDriverCommand->DriverStatus = INVALID_IO_QUEUE_MANAGEMENT_IEN;
					return;
				}
			}

			// If we don't have a submission queue that matches, fail now
			auto submissionQueueItr = this->SubmissionQueues.find(pDriverCommand->QueueId);
			if (submissionQueueItr == this->SubmissionQueues.end())
			{
				LOG_ERROR("Couldn't find a submission queue with the id: " + std::to_string(pDriverCommand->QueueId));
				pDriverCommand->DriverStatus = NO_MATCHING_SUBMISSION_QUEUE;
				return;
			}

			// If we don't know what completion queue that submission queue maps to, fail now.
			auto pCompletionQueue = submissionQueueItr->second->getMappedQueue();
			if (!pCompletionQueue)
			{
				LOG_ERROR("Couldn't find a linked completion queue for a submission queue with the id: " + std::to_string(pDriverCommand->QueueId));
				pDriverCommand->DriverStatus = NO_LINKED_COMPLETION_QUEUE;
				return;
			}

			// here goes nothing... send the command!
			auto pSubmissionQueue = submissionQueueItr->second;

			// Add the CID to the command
			pDriverCommand->Command.DWord0Breakdown.CID = getCommandIdForSubmissionQueueIdViaIncrementIfNeeded(pSubmissionQueue->getQueueId());

			// create a prps object (even if we don't use it)
			//  should stay in scope till command is done or we time out.
			PRP prps;

			// create a contiguous buffer address. If not NULL will be used/deleted later
			UINT_64 contiguousBufferAddress = NULL;

			// If MANUAL_PRPS, let the user deal with the PRP magic.
			if (pDriverCommand->TransferDataDirection != MANUAL_PRPS)
			{
				if (this->commandRequiresContiguousBufferInsteadOfPrp(pDriverCommand->Command, pDriverCommand->QueueId == ADMIN_QUEUE_ID))
				{
					size_t allocationSize;
					if (pDriverCommand->Command.DWord0Breakdown.OPC == constants::opcodes::admin::CREATE_IO_COMPLETION_QUEUE)
					{
						allocationSize = ONE_BASED_FROM_ZERO_BASED(pDriverCommand->Command.DW10_CreateIoQueue.QSIZE) * sizeof(command::COMPLETION_QUEUE_ENTRY);
					}
					else if (pDriverCommand->Command.DWord0Breakdown.OPC == constants::opcodes::admin::CREATE_IO_SUBMISSION_QUEUE)
					{
						allocationSize = ONE_BASED_FROM_ZERO_BASED(pDriverCommand->Command.DW10_CreateIoQueue.QSIZE) * sizeof(command::NVME_COMMAND);
					}
					else
					{
						ASSERT("Invalid command for contiguous allocation.");
					}

					BYTE* contig = new BYTE[allocationSize];
					memset(contig, 0xFF, allocationSize); // Set to high CID
					contiguousBufferAddress = POINTER_TO_MEMORY_ADDRESS(contig);    // DONT FORGET TO FREE ME... later.
					pDriverCommand->Command.DPTR.DPTR1 = contiguousBufferAddress;   // Give drive new queue location
				}
				else
				{
					prps.constructFromPayloadAndMemoryPageSize(cnvme::Payload(pDriverCommand->TransferData, pDriverCommand->TransferDataSize), this->TheController.getControllerRegisters()->getMemoryPageSize());
					if (pDriverCommand->TransferDataDirection == READ || pDriverCommand->TransferDataDirection == WRITE || pDriverCommand->TransferDataDirection == BI_DIRECTIONAL)
					{
						pDriverCommand->Command.DPTR.DPTR1 = prps.getPRP1();
						pDriverCommand->Command.DPTR.DPTR2 = prps.getPRP2();
					}
				}
			}

			// Get a pointer to the location to place the command
			UINT_8* theRawSubmissionQueue = MEMORY_ADDRESS_TO_8POINTER(pSubmissionQueue->getMemoryAddress());
			NVME_COMMAND* nvmeCommand = (NVME_COMMAND*)theRawSubmissionQueue;
			nvmeCommand += pSubmissionQueue->getTailPointer(); // Make sure we increment to the correct command.

			// Copy the command into the submission queue
			LOG_INFO("About to copy a command with an opcode of 0x" + cnvme::strings::toHexString(pDriverCommand->Command.DWord0Breakdown.OPC) + \
				" to submission queue 0x" + cnvme::strings::toHexString(pDriverCommand->QueueId) + " and CID of 0x" + cnvme::strings::toHexString(pDriverCommand->Command.DWord0Breakdown.CID));

			memcpy_s(nvmeCommand, sizeof(NVME_COMMAND), &pDriverCommand->Command, sizeof(pDriverCommand->Command));

			// Move the tail pointer up and ring the doorbell. This is the 'sending' per-say.
			pSubmissionQueue->incrementTailPointerAndRingDoorbell();

			// The head pointer is not used here. Just the tail.

			// The command has been sent!! 

			// Watch for timeout and the completion queue 
			UINT_64 deathTime = helpers::getTimeInMilliseconds() + (pDriverCommand->Timeout * 1000);

			bool commandTimedOut = true;
			while (helpers::getTimeInMilliseconds() < deathTime)
			{
				bool foundCompletion = false;
				COMPLETION_QUEUE_ENTRY* pCompletionQueueEntry = (COMPLETION_QUEUE_ENTRY*)MEMORY_ADDRESS_TO_8POINTER(pCompletionQueue->getMemoryAddress());

				// Look for a completion queue entry with a matching Command ID
				for (UINT_16 completionEntryIndex = 0; completionEntryIndex < pCompletionQueue->getQueueSize(); completionEntryIndex++)
				{
					if (pCompletionQueueEntry->CID == pDriverCommand->Command.DWord0Breakdown.CID)
					{
						LOG_INFO("Found matching completion entry for CID " + strings::toHexString(pCompletionQueueEntry->CID) + " in CQE index " + \
							strings::toHexString(completionEntryIndex));

						memcpy_s(&pDriverCommand->CompletionQueueEntry, sizeof(pDriverCommand->CompletionQueueEntry), pCompletionQueueEntry, sizeof(COMPLETION_QUEUE_ENTRY));

						// copy data back if this was a read.
						if (pDriverCommand->TransferDataDirection == READ)
						{
							auto payloadOfReadData = prps.getPayloadCopy();
							memcpy_s(&pDriverCommand->TransferData, driverCommandBufferSize - sizeof(DRIVER_COMMAND), payloadOfReadData.getBuffer(), pDriverCommand->TransferDataSize);
						}

						pDriverCommand->DriverStatus = SENT_SUCCESSFULLY;
						foundCompletion = true;
						break;
					}
					pCompletionQueueEntry++; // Increment through the completion queue
				}

				if (foundCompletion)
				{
					commandTimedOut = false;
					break;
				}
			}

			if (commandTimedOut)
			{
				LOG_ERROR("The command timed out");
				pDriverCommand->DriverStatus = TIMEOUT;

				// its debatable if we should free memory on a timeout...
				// on the real (tm) driver they would do an NVMe Controller Reset and then deallocate everything.
				//  the command could be in progress or something..
				//   though right now this would leak on IO Queue Creation.
			}
			// We did the command and its a contiguous buffer cmd
			else if (pDriverCommand->TransferDataDirection != MANUAL_PRPS && this->commandRequiresContiguousBufferInsteadOfPrp(pDriverCommand->Command, pDriverCommand->QueueId == ADMIN_QUEUE_ID))
			{
				auto doorbells = this->TheController.getControllerRegisters()->getQueueDoorbells();
				doorbells += pDriverCommand->Command.DW10_CreateIoQueue.QID; // find our doorbell

				// Command failed if (SC | SCT) != 0. Free the memory!
				if (pDriverCommand->CompletionQueueEntry.SC | pDriverCommand->CompletionQueueEntry.SCT)
				{
					ASSERT_IF(contiguousBufferAddress == 0, "Somehow we sent a contiguous buffer address of 0. That could have killed the drive!");

					LOG_ERROR("Freeing memory for contigous queue buffer since our queue creation failed!");
					delete[]MEMORY_ADDRESS_TO_8POINTER(contiguousBufferAddress);
				}
				else if (pDriverCommand->Command.DWord0Breakdown.OPC == constants::opcodes::admin::CREATE_IO_COMPLETION_QUEUE)
				{
					LOG_INFO("Succeeded in creating IO Completion Queue " + std::to_string(pDriverCommand->Command.DW10_CreateIoQueue.QID) + " will hold onto memory.");

					this->CompletionQueues[pDriverCommand->Command.DW10_CreateIoQueue.QID] = new Queue(ONE_BASED_FROM_ZERO_BASED(pDriverCommand->Command.DW10_CreateIoQueue.QSIZE),
						pDriverCommand->Command.DW10_CreateIoQueue.QID, 
						(UINT_16*)&(doorbells->CQHDBL), // doorbell
						contiguousBufferAddress
					);
				}
				else if (pDriverCommand->Command.DWord0Breakdown.OPC == constants::opcodes::admin::CREATE_IO_SUBMISSION_QUEUE)
				{
					auto mappedCompletionQueueItr = this->CompletionQueues.find(pDriverCommand->Command.DW11_CreateIoSubmissionQueue.CQID);

					if (mappedCompletionQueueItr == this->CompletionQueues.end())
					{
						ASSERT("Somehow the Controller passed a command to create a submission queue with an unknown submission queue. The Driver can't map.");
						return; // if asserts are off (mercy on our soul)... move on.
					}

					LOG_INFO("Succeeded in creating IO Submission Queue " + std::to_string(pDriverCommand->Command.DW10_CreateIoQueue.QID) + " will hold onto memory.");

					Queue* subQ = new Queue(ONE_BASED_FROM_ZERO_BASED(pDriverCommand->Command.DW10_CreateIoQueue.QSIZE),
						pDriverCommand->Command.DW10_CreateIoQueue.QID, 
						(UINT_16*)&(doorbells->SQTDBL), // doorbell
						contiguousBufferAddress
					);
					this->SubmissionQueues[pDriverCommand->Command.DW10_CreateIoQueue.QID] = subQ;

					subQ->setMappedQueue(mappedCompletionQueueItr->second); // SQ -> CQ
					mappedCompletionQueueItr->second->setMappedQueue(subQ); // CQ -> SQ
				}
			}
			else if ((pDriverCommand->CompletionQueueEntry.SC | pDriverCommand->CompletionQueueEntry.SCT) == 0) // command passed
			{
				if (pDriverCommand->Command.DWord0Breakdown.OPC == constants::opcodes::admin::DELETE_IO_SUBMISSION_QUEUE)
				{
					LOG_INFO("Passed a command to delete IO Submission Queue: " + std::to_string(pDriverCommand->Command.DW10_DeleteIoQueue.QID));

					auto subQ = this->SubmissionQueues.find(pDriverCommand->Command.DW10_DeleteIoQueue.QID);
					if (subQ == this->SubmissionQueues.end())
					{
						LOG_INFO("The controller let us delete that queue... but the driver didn't know it existed.");
					}
					else
					{
						delete[] MEMORY_ADDRESS_TO_8POINTER(subQ->second->getMemoryAddress());
						delete subQ->second;
						this->SubmissionQueues.erase(subQ);
					}

					// Reset SQID->CID mapping
					this->SubmissionQueueIdToCurrentCommandIdentifiers[pDriverCommand->Command.DW10_DeleteIoQueue.QID] = 0;
				}
				else if (pDriverCommand->Command.DWord0Breakdown.OPC == constants::opcodes::admin::DELETE_IO_COMPLETION_QUEUE)
				{
					LOG_INFO("Passed a command to delete IO Completion Queue: " + std::to_string(pDriverCommand->Command.DW10_DeleteIoQueue.QID));

					auto compQ = this->CompletionQueues.find(pDriverCommand->Command.DW10_DeleteIoQueue.QID);
					if (compQ == this->CompletionQueues.end())
					{
						LOG_INFO("The controller let us delete that queue... but the driver didn't know it existed.");
					}
					else
					{
						delete[] MEMORY_ADDRESS_TO_8POINTER(compQ->second->getMemoryAddress());
						delete compQ->second;
						this->CompletionQueues.erase(compQ);
					}
				}
			}
		}

		bool Driver::controllerReset()
		{
			auto CR = this->TheController.getControllerRegisters()->getControllerRegisters();
			auto timeoutMs = CR->CAP.TO * 500; // CAP.TO is in 500 millisecond intervals

			CR->CC.EN = 0; // Begin Reset
			auto deathTime = helpers::getTimeInMilliseconds() + timeoutMs;
			bool rdyTo0 = false;
			while (helpers::getTimeInMilliseconds() < deathTime)
			{
				if (CR->CSTS.RDY == 0)
				{
					rdyTo0 = true;
					break;
				}
			}

			FAIL_IF(rdyTo0 == false, "CSTS.RDY did not transition to 0 after CC.EN was set to 0");

			CR->CC.EN = 1; // Enable controller and wait till ready
			bool rdyTo1 = false;
			deathTime = helpers::getTimeInMilliseconds() + timeoutMs;
			while (helpers::getTimeInMilliseconds() < deathTime)
			{
				if (CR->CSTS.RDY == 1)
				{
					rdyTo1 = true;
					break;
				}
			}
			FAIL_IF(rdyTo1 == false, "CSTS.RDY did not transition to 1 after CC.EN was set to 1");

			LOG_INFO("Deleting all IO Queues");
			this->deleteAllIoQueues();
			LOG_INFO("Controller Reset succeeded!");

			return true;
		}

		void Driver::setControllerCommandResponseProcessingFile(std::string filePath)
		{
			this->TheController.setCommandResponseFilePath(filePath);
		}

		UINT_16 Driver::getCommandIdForSubmissionQueueIdViaIncrementIfNeeded(UINT_16 submissionQueueId)
		{
			auto entry = this->SubmissionQueueIdToCurrentCommandIdentifiers.find(submissionQueueId);
			UINT_16 retVal = 0;

			// New Submission Queue?
			if (entry == this->SubmissionQueueIdToCurrentCommandIdentifiers.end())
			{
				this->SubmissionQueueIdToCurrentCommandIdentifiers[submissionQueueId] = 0; // Add new set for this queue
			}
			// Existing Submission Queue?
			else
			{
				(*entry).second++;
			}

			return this->SubmissionQueueIdToCurrentCommandIdentifiers[submissionQueueId];
		}

		bool Driver::commandRequiresContiguousBufferInsteadOfPrp(NVME_COMMAND& nvmeCommand, bool admin)
		{
			if (admin)
			{
				return nvmeCommand.DWord0Breakdown.OPC == constants::opcodes::admin::CREATE_IO_COMPLETION_QUEUE || nvmeCommand.DWord0Breakdown.OPC == constants::opcodes::admin::CREATE_IO_SUBMISSION_QUEUE;
			}
			return false;
		}

		void Driver::deleteAllIoQueues()
		{
			// Delete all IO Submission Queues
			for (auto &i : this->SubmissionQueues)
			{
				if (i.first != ADMIN_QUEUE_ID)
				{
					if (i.second->getMemoryAddress())
					{
						delete[]MEMORY_ADDRESS_TO_8POINTER(i.second->getMemoryAddress());
						i.second->setMemoryAddress(0);
					}
					delete i.second;
				}
			}

			// Delete all IO Completion Queues
			for (auto &i : this->CompletionQueues)
			{
				if (i.first != ADMIN_QUEUE_ID)
				{
					if (i.second->getMemoryAddress())
					{
						delete[]MEMORY_ADDRESS_TO_8POINTER(i.second->getMemoryAddress());
						i.second->setMemoryAddress(0);
					}
					delete i.second;
				}
			}
		}
	}
}
