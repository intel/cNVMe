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
Driver.h - A header file for the cNVMe 'Driver'
*/

#pragma once

#include "Controller.h"
#include "Queue.h"
#include "Types.h"

namespace cnvme
{
	namespace driver
	{
		/// <summary>
		/// Status used to denote if the driver sent the command or not
		/// </summary>
		enum Status
		{
			SENT_SUCCESSFULLY,
			NO_MATCHING_SUBMISSION_QUEUE,
			NO_LINKED_COMPLETION_QUEUE,
			TIMEOUT,
			BUFFER_NOT_LARGE_ENOUGH,
			INVALID_DATA_DIRECTION,
			INVALID_DATA_LENGTH,
			INVALID_DATA_LENGTH_FOR_MANUAL_PRPS,
			INVALID_IO_QUEUE_MANAGEMENT_PC,
			INVALID_IO_QUEUE_MANAGEMENT_IEN,
		};

		/// <summary>
		/// Converts a status to a string representation
		/// </summary>
		/// <param name="s">Status to convert</param>
		/// <returns>string</returns>
		std::string statusToString(Status s);

		/// <summary>
		/// Enum used to denote the direction of data transfer
		/// </summary>
		enum DataDirection
		{
			NO_DATA,
			READ,
			WRITE,
			BI_DIRECTIONAL,
			MANUAL_PRPS, // used if the driver should ignore checks for data length since the user is handling PRPs
			DATA_DIRECTION_MAX,
		};

		/// <summary>
		/// Converts the DataDirection to a string
		/// </summary>
		std::string dataDirectionToString(DataDirection d);

#ifdef _WIN32
#pragma warning (push)
#pragma warning	(disable: 4200)	// Disable 0-sized array warning.
#endif
		/// <summary>
		/// Structure used to send a command via the driver
		/// </summary>
		typedef struct DRIVER_COMMAND
		{
			Status DriverStatus;						// Filled out by driver
			UINT_16 Timeout;							// Filled out by user
			UINT_16 QueueId;							// Filled out by user
			NVME_COMMAND Command;						// Filled out by user (modified by the driver for PRPs, etc)
			COMPLETION_QUEUE_ENTRY CompletionQueueEntry;// Filled out by the driver
			DataDirection TransferDataDirection;		// Filled out by the user
			UINT_32 TransferDataSize;					// Filled out by the user
			UINT_8 TransferData[0];						// Filled out by the driver on reads, user on writes, both on bidirectional
		} DRIVER_COMMAND, *PDRIVER_COMMAND;
#ifdef _WIN32
#pragma warning(pop) // Disable 0-sized array warning.
#endif

		/// <summary>
		/// Production Driver class used by the DLL (and everything other than internal testing)
		/// </summary>
		class Driver
		{
		public:
			/// <summary>
			/// Constructor for a driver
			/// </summary>
			Driver();

			/// <summary>
			/// Destructor for a driver
			/// </summary>
			~Driver();

			/// <summary>
			/// Used to send a command to the underlying controller
			/// </summary>
			/// <param name="driverCommandBuffer">Pointer to the filled out DRIVER_COMMAND structure</param>
			/// <param name="driverCommandBufferSize">Size of the data pointed to in driverCommandBuffer</param>
			void sendCommand(UINT_8* driverCommandBuffer, size_t driverCommandBufferSize);

			/// <summary>
			/// Issues a controller reset (CC.EN->0) and will wait for CC.EN->1.
			/// </summary>
			/// <returns>true on success, False on failure</returns>
			bool controllerReset();

			/// <summary>
			/// Used to set the CRAPI-F file for CRAPI
			/// </summary>
			/// <param name="filePath">path to the file</param>
			void setControllerCommandResponseProcessingFile(std::string filePath);

		private:
			/// <summary>
			/// The controller that this driver is connected to
			/// </summary>
			controller::Controller TheController;

			/// <summary>
			/// Map from id to submission queue
			/// </summary>
			std::map<UINT_16, controller::Queue*> SubmissionQueues;

			/// <summary>
			/// Map from id to completion queue
			/// </summary>
			std::map<UINT_16, controller::Queue*> CompletionQueues;

			/// <summary>
			/// Used to keep track of CIDs that have been used
			/// </summary>
			std::map<UINT_16, UINT_16> SubmissionQueueIdToCurrentCommandIdentifiers;

			/// <summary>
			/// Will update SubmissionQueueIdToCurrentCommandIdentifiers and return the next CID.
			/// </summary>
			UINT_16 getCommandIdForSubmissionQueueIdViaIncrementIfNeeded(UINT_16 submissionQueueId);

			/// <summary>
			/// Returns True if we should use a raw pointer to contiguous memory instead of PRPs
			/// </summary>
			/// <param name="nvmeCommand">command to check</param>
			/// <param name="admin">true if this is an admin command</param>
			/// <returns>bool</returns>
			bool commandRequiresContiguousBufferInsteadOfPrp(NVME_COMMAND& nvmeCommand, bool admin);

			/// <summary>
			/// Deallocates all IO queues
			/// </summary>
			void deleteAllIoQueues();
		};

		/// <summary>
		/// struct returned by TestDriver public commands
		/// </summary>
		typedef struct _TEST_DRIVER_OUTPUT
		{
			COMPLETION_QUEUE_ENTRY CompletionQueueEntry;
			Payload OutputData;
		} TEST_DRIVER_OUTPUT, *PTEST_DRIVER_OUTPUT;

		/// <summary>
		/// Special version of Driver used by testing
		/// </summary>
		class TestDriver : public Driver
		{
		public:
			/// <summary>
			/// Perform a generic NVMe write command
			/// </summary>
			/// <param name="nvmeCommand">NVMe command</param>
			/// <param name="queueId">Queue Id</param>
			/// <param name="data">Data to write</param>
			/// <returns>TEST_DRIVER_OUTPUT</returns>
			TEST_DRIVER_OUTPUT writeCommand(NVME_COMMAND nvmeCommand, UINT_16 queueId, Payload& data);

			/// <summary>
			/// Perform a generic NVMe read command
			/// </summary>
			/// <param name="nvmeCommand">NVMe command</param>
			/// <param name="queueId">Queue Id</param>
			/// <param name="dataSize">Read data size in bytes</param>
			/// <returns>TEST_DRIVER_OUTPUT</returns>
			TEST_DRIVER_OUTPUT readCommand(NVME_COMMAND nvmeCommand, UINT_16 queueId, UINT_32 dataSize);

			/// <summary>
			/// Perform a generic NVMe non-data command
			/// </summary>
			/// <param name="nvmeCommand">NVMe command</param>
			/// <param name="queueId">Queue Id</param>
			/// <returns>TEST_DRIVER_OUTPUT</returns>
			TEST_DRIVER_OUTPUT nonDataCommand(NVME_COMMAND nvmeCommand, UINT_16 queueId);

			/// <summary>
			/// Used to test FW Image Download
			/// </summary>
			/// <param name="DWOffset">Offset for data in DWs</param>
			/// <param name="data">FW Image Download piece to send</param>
			/// <returns>TEST_DRIVER_OUTPUT</returns>
			TEST_DRIVER_OUTPUT firmwareImageDownload(UINT_32 DWOffset, Payload& data);

			/// <summary>
			/// Used to test FW Commit
			/// </summary>
			/// <param name="commitAction">commit action</param>
			/// <param name="firmwareSlot">targeted slot</param>
			/// <returns>TEST_DRIVER_OUTPUT</returns>
			TEST_DRIVER_OUTPUT firmwareCommit(UINT_8 commitAction, UINT_8 firmwareSlot);

			/// <summary>
			/// Used to test Identify
			/// </summary>
			/// <param name="CNS">Controller/Namespace structure field</param>
			/// <param name="NSID">Namespace id for command</param>
			/// <returns>TEST_DRIVER_OUTPUT</returns>
			TEST_DRIVER_OUTPUT identify(UINT_8 CNS, UINT_32 NSID);

			/// <summary>
			/// Returns the FW string obtained by identify controller
			/// </summary>
			/// <returns></returns>
			std::string getFirmwareString();
		};
	}
}
