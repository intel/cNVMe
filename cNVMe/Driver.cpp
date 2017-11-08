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
			else if (s == NO_MATCHING_QUEUE_ID)
			{
				return "Didn't have a queue with the specified id to send the command to";
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
			UINT_32 adminSubmissionQueueByteSize = (controllerRegisters->AQA.ASQS + 1) * (UINT_32)pow(2, DEFAULT_SUBMISSION_QUEUE_ENTRY_SIZE);
			UINT_32 adminCompletionQueueByteSize = (controllerRegisters->AQA.ACQS + 1) * (UINT_32)pow(2, DEFAULT_COMPLETION_QUEUE_ENTRY_SIZE);

			ASSERT_IF(adminSubmissionQueueByteSize < 64, "The admin submission queue must be at least 64 bytes!");
			ASSERT_IF(adminCompletionQueueByteSize < 16, "The admin completion queue must be at least 16 bytes!");

			// Create the payloads for the admin queues
			Payload adminSubmissionQueuePayload(adminSubmissionQueueByteSize);
			Payload adminCompletionQueuePayload(adminCompletionQueueByteSize);

			// Make sure the payloads stay in scope.
			PayloadsInScope.push_back(adminSubmissionQueuePayload);
			PayloadsInScope.push_back(adminCompletionQueuePayload);

			// Place the memory addresses in the registers
			controllerRegisters->ASQ.ASQB = adminSubmissionQueuePayload.getMemoryAddress();
			controllerRegisters->ACQ.ACQB = adminCompletionQueuePayload.getMemoryAddress();

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

			// If we can return an invalid buffer size, do.
			if (driverCommandBufferSize < sizeof(DRIVER_COMMAND) || (driverCommandBufferSize < pDriverCommand->TransferDataSize + sizeof(DRIVER_COMMAND)))
			{
				LOG_ERROR("The provided buffer was not large enough");
				pDriverCommand->DriverStatus = BUFFER_NOT_LARGE_ENOUGH;
				return;
			}

			if (pDriverCommand->TransferDataDirection >= DATA_DIRECTION_MAX)
			{
				LOG_ERROR("Invalid data direction was provided");
				pDriverCommand->DriverStatus = INVALID_DATA_DIRECTION;
				return;
			}

			if (pDriverCommand->TransferDataSize == 0 && pDriverCommand->TransferDataDirection != NO_DATA)
			{
				LOG_ERROR("Transfer data size was 0 but the data direction is not no-data");
				pDriverCommand->DriverStatus = INVALID_DATA_LENGTH;
			}

			ASSERT("Wuh oh. Not implemented yet.");
		}
	}
}
