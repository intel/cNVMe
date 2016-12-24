/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
Controller.h - A header file for the NVMe Controller
*/

#pragma once

#include "ControllerRegisters.h"
#include "PCIe.h"
#include "Types.h"

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
			/// Map from queue id to current tail
			/// </summary>
			std::map<UINT_16, UINT_16> ValidSubmissionQueues;

			/// <summary>
			/// Used to keep track of the non-deleted but created qcompletion queues
			/// Map from queue id to current head
			/// </summary>
			std::map<UINT_16, UINT_16> ValidCompletionQueues;

			/// <summary>
			/// Function to be called in loop looking for changes
			/// </summary>
			void checkForChanges();

			/// <summary>
			/// This call will take the command of the given submission queue id and ring the completion doorbell upon completion
			/// </summary>
			/// <param name="submissionQueueId">The submission queue id</param>
			void processCommandAndPostCompletion(UINT_16 submissionQueueId);
		};
	}
}
