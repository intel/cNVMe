/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2017
Controller.h - A header file for the NVMe Controller
*/

#pragma once

#include "Command.h"
#include "ControllerRegisters.h"
#include "PCIe.h"
#include "Types.h"
#include "Queue.h"

#define MAX_COMMAND_IDENTIFIER 0xFFFF
#define MAX_SUBMISSION_QUEUES  0xFFFF

using namespace cnvme;

namespace cnvme
{
	namespace controller
	{

		class Controller
		{
		public:
			/// <summary>
			/// Constructor for the controller
			/// </summary>
			Controller();

			/// <summary>
			/// Destructor for the controller
			/// </summary>
			~Controller();

			/// <summary>
			/// Gets the internal ControllerRegisters
			/// </summary>
			/// <returns>ControllerRegisters object</returns>
			cnvme::controller::registers::ControllerRegisters* getControllerRegisters();

			/// <summary>
			/// Gets the PCI (Bus) Express Registers 
			/// </summary>
			/// <returns></returns>
			pci::PCIExpressRegisters* getPCIExpressRegisters();

			/// <summary>
			/// Function to reset the controller
			/// Called by the ControllerRegisters as a callback	
			/// Note: This should probably be much more private a method. Only the controller registers should call this
			/// </summary>
			void controllerResetCallback();

			/// <summary>
			/// Wait for an iteration of the interrupt loop
			/// </summary>
			void waitForChangeLoop();

		private:

			/// <summary>
			/// The controller registers
			/// </summary>
			controller::registers::ControllerRegisters* ControllerRegisters;
			
			/// <summary>
			/// The PCI (Bus) Express Registers
			/// </summary>
			pci::PCIExpressRegisters* PCIExpressRegisters;

			/// <summary>
			/// Looping thread to watch for doorbell writes.
			/// </summary>
			LoopingThread DoorbellWatcher;

			/// <summary>
			/// Used to keep track of the non-deleted but created submission queues
			/// Vector of queue objects
			/// </summary>
			std::vector<Queue> ValidSubmissionQueues;

			/// <summary>
			/// Used to keep track of the non-deleted but created completion queues
			/// Vector of queue objects
			/// </summary>
			std::vector<Queue> ValidCompletionQueues;

			/// <summary>
			/// Used to keep track of CIDs that have been used
			/// </summary>
			std::map<UINT_16, std::set<UINT_16>> SubmissionQueueIdToCommandIdentifiers;

			/// <summary>
			/// Function to be called in loop looking for changes
			/// </summary>
			void checkForChanges();

			/// <summary>
			/// This call will take the command of the given submission queue, process the command and
			/// pass back completion via the completion queue doorbell.
			/// </summary>
			/// <param name="submissionQueue">The internal submission queue object for this command</param>
			void processCommandAndPostCompletion(Queue &submissionQueue);

			/// <summary>
			/// Returns a Queue matching the given id
			/// </summary>
			/// <param name="queues">vector of Queues</param>
			/// <param name="id">The queue id</param>
			/// <returns>Queue</returns>
			Queue *getQueueWithId(std::vector<Queue> &queues, UINT_16 id);

			/// <summary>
			/// Posts the given completion to the given queue.
			/// Fills in sqid, sqhd, cid.
			/// Also can flip the Phase Tag if needed.
			/// </summary>
			/// <param name="completionQueue">Queue to post to</param>
			/// <param name="completionEntry">Entry to post to the queue</param>
			/// <param name="command">The NVMe Command that is having its completion posted</param>
			void postCompletion(Queue &completionQueue, command::COMPLETION_QUEUE_ENTRY completionEntry, command::NVME_COMMAND* command);

			/// <summary>
			/// Returns true if the command id 
			/// </summary>
			/// <param name="commandId">The command ID we are checking for</param>
			/// <param name="submissionQueueId">The sub queue id the command was sent to</param>
			/// <returns>true if valid, False otherwise.</returns>
			bool isValidCommandIdentifier(UINT_16 commandId, UINT_16 submissionQueueId);

			/// <summary>
			/// Corresponds with the phase tag in the completion queue entry
			/// </summary>
			bool PhaseTag;
		};
	}
}
