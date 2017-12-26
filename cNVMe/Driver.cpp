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

			ASSERT("Status not found in statusToString()");
			return "Unknown";
		}

		std::string dataDirectionToString(DataDirection d)
		{
			if (d = NO_DATA)
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
			auto controllerRegisters = TheController.getControllerRegisters()->getControllerRegisters();

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
			UINT_16* adminSubmissionQueueDoorbell = &TheController.getControllerRegisters()->getQueueDoorbells()->SQTDBL.SQT;
			UINT_16* adminCompletionQueueDoorbell = &TheController.getControllerRegisters()->getQueueDoorbells()->CQHDBL.CQH;

			// Place the memory addresses in the registers
			controllerRegisters->ASQ.ASQB = adminSubmissionQueuePayload.getMemoryAddress();
			controllerRegisters->ACQ.ACQB = adminCompletionQueuePayload.getMemoryAddress();

			// Make Queue objects to hang onto
			Queue adminSubmissionQueue(ONE_BASED_FROM_ZERO_BASED(controllerRegisters->AQA.ASQS), ADMIN_QUEUE_ID, adminSubmissionQueueDoorbell, controllerRegisters->ASQ.ASQB);
			Queue adminCompletionQueue(ONE_BASED_FROM_ZERO_BASED(controllerRegisters->AQA.ACQS), ADMIN_QUEUE_ID, adminCompletionQueueDoorbell, controllerRegisters->ACQ.ACQB);

			// Add Queue objects to our container
			SubmissionQueues[ADMIN_QUEUE_ID] = adminSubmissionQueue;
			CompletionQueues[ADMIN_QUEUE_ID] = adminCompletionQueue;

			// Link the Queue objects to each other
			SubmissionQueues[ADMIN_QUEUE_ID].setMappedQueue(&CompletionQueues[ADMIN_QUEUE_ID]);
			CompletionQueues[ADMIN_QUEUE_ID].setMappedQueue(&SubmissionQueues[ADMIN_QUEUE_ID]);

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
				}
			}

			ASSERT_IF(!controllerWentReady, "Controller did not ready up in time!");

			// If we made it here, the controller is enabled and we are ready for action!
		}

		Driver::~Driver()
		{
			// nothing to do here
		}

		void Driver::sendCommand(UINT_8* driverCommandBuffer, UINT_32 driverCommandBufferSize)
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

			// If we don't have a submission queue that matches, fail now
			auto submissionQueueItr = SubmissionQueues.find(pDriverCommand->QueueId);
			if (submissionQueueItr == SubmissionQueues.end())
			{
				LOG_ERROR("Couldn't find a submission queue with the id: " + std::to_string(pDriverCommand->QueueId));
				pDriverCommand->DriverStatus = NO_MATCHING_SUBMISSION_QUEUE;
				return;
			}

			// If we don't know what completion queue that submission queue maps to, fail now.
			auto pCompletionQueue = submissionQueueItr->second.getMappedQueue();
			if (!pCompletionQueue)
			{
				LOG_ERROR("Couldn't find a linked completion queue for a submission queue with the id: " + std::to_string(pDriverCommand->QueueId));
				pDriverCommand->DriverStatus = NO_LINKED_COMPLETION_QUEUE;
				return;
			}

			// here goes nothing... send the command!
			auto pSubmissionQueue = &submissionQueueItr->second;

			// Add the CID to the command
			pDriverCommand->Command.DWord0Breakdown.CID = getCommandIdForSubmissionQueueIdViaIncrementIfNeeded(pSubmissionQueue->getQueueId());

			// create a prps object (even if we don't use it)
			//  should stay in scope till command is done or we time out.
			PRP prps(cnvme::Payload(pDriverCommand->TransferData, pDriverCommand->TransferDataSize), TheController.getControllerRegisters()->getMemoryPageSize());
			if (pDriverCommand->TransferDataDirection != NO_DATA)
			{
				pDriverCommand->Command.DPTR.DPTR1 = prps.getPRP1();
				pDriverCommand->Command.DPTR.DPTR2 = prps.getPRP2();
			}

			// Get a pointer to the location to place the command
			UINT_8* theRawSubmissionQueue = MEMORY_ADDRESS_TO_8POINTER(pSubmissionQueue->getMemoryAddress());
			NVME_COMMAND* nvmeCommand = (NVME_COMMAND*)theRawSubmissionQueue;
			nvmeCommand += pSubmissionQueue->getTailPointer(); // Make sure we increment to the correct command.

			// Copy the command into the submission queue
			LOG_INFO("About to copy a command with an opcode of 0x" + cnvme::strings::toHexString(pDriverCommand->Command.DWord0Breakdown.OPC) + \
				" to submission queue 0x" + cnvme::strings::toHexString(pDriverCommand->QueueId) + " and CID of 0x" + cnvme::strings::toHexString(pDriverCommand->Command.DWord0Breakdown.CID));

			memcpy_s(nvmeCommand, sizeof(NVME_COMMAND), &pDriverCommand->Command, sizeof(pDriverCommand->Command));

			// Move the tail pointer up and ring the doorbell.
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
			}
		}

		UINT_16 Driver::getCommandIdForSubmissionQueueIdViaIncrementIfNeeded(UINT_16 submissionQueueId)
		{
			auto entry = SubmissionQueueIdToCurrentCommandIdentifiers.find(submissionQueueId);
			UINT_16 retVal = 0;

			// New Submission Queue?
			if (entry == SubmissionQueueIdToCurrentCommandIdentifiers.end())
			{
				SubmissionQueueIdToCurrentCommandIdentifiers[submissionQueueId] = 0; // Add new set for this queue
			}
			// Existing Submission Queue?
			else
			{
				(*entry).second++;
			}

			return SubmissionQueueIdToCurrentCommandIdentifiers[submissionQueueId];
		}
	}
}
