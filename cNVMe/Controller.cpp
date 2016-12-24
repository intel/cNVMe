#include "Controller.h"
/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
Controller.cpp - An implementation file for the NVMe Controller
*/

#define ADMIN_QUEUE_ID 0

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

			// Todo: in the end, we'll need some form of media bank
		}

		Controller::~Controller()
		{
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

			if (controllerRegisters->CSTS.RDY == 0)
			{
				return;
			}

			if (controllerRegisters->AQA.ACQS == 0 || controllerRegisters->AQA.ASQS == 0)
			{
				return; // Don't have admin completion / submission queue
			}

			if (controllerRegisters->ASQ.ASQB == 0)
			{
				return; // Don't have a admin submission queue address
			}

			// Now that we have a SQ address, make it valid
			if (ValidSubmissionQueues.size() == 0)
			{
				ValidSubmissionQueues[ADMIN_QUEUE_ID] = 0;
			}

			if (controllerRegisters->ACQ.ACQB == 0)
			{
				return; // Don't have a admin completion queue address
			}

			// Now that we have a CQ address, make it valid
			if (ValidCompletionQueues.size() == 0)
			{
				ValidCompletionQueues[ADMIN_QUEUE_ID] = 0;
			}

			// Made it this far, we have at least the admin queue

			// Admin doorbell is right after the Controller Registers
			controller::registers::QUEUE_DOORBELLS* doorbells = ((controller::registers::QUEUE_DOORBELLS*)ControllerRegisters->getControllerRegisters()) \
				+ sizeof(registers::CONTROLLER_REGISTERS);

			for (auto sq : ValidSubmissionQueues)
			{
				UINT_16 sqid = sq.first;
				UINT_16 sqidHead = sq.second;
				if (doorbells[sqid].SQTDBL.SQT != sqidHead) // Head moved, doorbell has rung
				{
					// Todo... make sure the SQT isn't larger than AQA.ASQS
					// Todo... probably need a way to know some queue attributes. Like Queue size, queue tail/head.
					//    Right now we have the Valid...Queues maps, need to make them map from queueId to attributes.

					ValidSubmissionQueues[sqid] = doorbells[sqid].SQTDBL.SQT;
					processCommandAndPostCompletion(sqid); // This calls the command
				}
			}
		}

		void Controller::processCommandAndPostCompletion(UINT_16 submissionQueueId)
		{
			auto controllerRegisters = getControllerRegisters()->getControllerRegisters();

			if (submissionQueueId == 0)
			{
				// Admin command
				UINT_64 adminSubQAddr = (UINT_64)controllerRegisters->ASQ.ASQB;
				UINT_32* subQPointer = (UINT_32*)adminSubQAddr;

				LOG_INFO("Admin Command Recv'd");
				for (int i = 0; i < 16; i++)
				{
					// todo... class for NVMe Command. 64 byte / 16 DWords

					LOG_INFO("DWord " + std::to_string(i) + ": 0x" + strings::toHexString(subQPointer[i]));
				}

				// Todo: post completion and actually process the command

			}
			else
			{
				// NVM command
				assert(0); // Don't send me NVM commands yet.
			}
		}

		void Controller::controllerResetCallback()
		{
			LOG_INFO("Recv'd a controllerResetCallback request.")
			bool haveAdminSubQueue = (ValidSubmissionQueues.find(ADMIN_QUEUE_ID) != ValidSubmissionQueues.end());
			bool haveAdminComQueue = (ValidCompletionQueues.find(ADMIN_QUEUE_ID) != ValidCompletionQueues.end());

			// Reset to just admin queues if we have it
			ValidSubmissionQueues.clear();
			if (haveAdminSubQueue)
			{
				ValidSubmissionQueues[ADMIN_QUEUE_ID] = 0;
			}
			ValidCompletionQueues.clear();
			if (haveAdminComQueue)
			{
				ValidCompletionQueues[ADMIN_QUEUE_ID] = 0;
			}
		}

		void Controller::waitForChangeLoop()
		{
#ifndef SINGLE_THREADED
			DoorbellWatcher.waitForFlip();
			DoorbellWatcher.waitForFlip();  // 2 flips to prevent the case where something gets changed in the middle of a flip
#else
			checkForChanges();
#endif
		}
	}
}
