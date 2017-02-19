/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2017
Controller.cpp - An implementation file for the NVMe Controller
*/

#define ADMIN_QUEUE_ID 0

#include "Command.h"
#include "Controller.h"
#include "Strings.h"

using namespace cnvme::command;

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
				ValidSubmissionQueues.push_back(Queue(controllerRegisters->AQA.ASQS, ADMIN_QUEUE_ID, &doorbells[ADMIN_QUEUE_ID].SQTDBL.SQT, controllerRegisters->ASQ.ASQB));
			}

			if (controllerRegisters->ACQ.ACQB == 0)
			{
				return; // Don't have a admin completion queue address
			}

			// Now that we have a CQ address, make it valid
			if (ValidCompletionQueues.size() == 0)
			{
				Queue AdminCompletionQueue(controllerRegisters->AQA.ACQS, ADMIN_QUEUE_ID, &doorbells[ADMIN_QUEUE_ID].CQHDBL.CQH, controllerRegisters->ACQ.ACQB);
				AdminCompletionQueue.setMappedQueue(&ValidSubmissionQueues[ADMIN_QUEUE_ID]); // Map CQ -> SQ
				ValidCompletionQueues.push_back(AdminCompletionQueue);
				ValidSubmissionQueues[0].setMappedQueue(&ValidCompletionQueues[ADMIN_QUEUE_ID]); // Map SQ -> CQ
			}

			// Made it this far, we have at least the admin queue
			// This is round-robin right now
			for (auto &sq : ValidSubmissionQueues)
			{
				UINT_16 sqid = sq.getQueueId();
				UINT_16 sqidHead = sq.getIndex();
				UINT_16 setIndex = doorbells[sqid].SQTDBL.SQT;
				if (setIndex != sqidHead) // Index moved, doorbell has rung
				{
					if (setIndex > sq.getQueueSize())
					{
						assert(0); // Invalid doorbell rang. I think this would trigger an async event.
					}

					// Update my record of the Index
					sq.setIndex(setIndex);

					processCommandAndPostCompletion(sqid); // This calls the command
				}
			}
		}

		void Controller::processCommandAndPostCompletion(UINT_16 submissionQueueId)
		{
			auto controllerRegisters = getControllerRegisters()->getControllerRegisters();
			Queue* theSubmissionQueue = getQueueWithId(ValidSubmissionQueues, submissionQueueId);
			if (theSubmissionQueue == nullptr)
			{
				LOG_ERROR("Somehow we were unable to find a submission queue matching: " + std::to_string(submissionQueueId));
				return;
			}

			Queue* theCompletionQueue = theSubmissionQueue->getMappedQueue();
			if (theCompletionQueue == nullptr)
			{
				LOG_ERROR("Submission Queue " + std::to_string(submissionQueueId) + " doesn't have a mapped completion queue. And yet it recieved a command.");
				return;
			}

			UINT_8* subQPointer = (UINT_8*)theSubmissionQueue->getMemoryAddress(); // This is the address of the 64 byte command
			NVME_COMMAND* command = (NVME_COMMAND*)subQPointer;
			controller::registers::QUEUE_DOORBELLS* doorbells = getControllerRegisters()->getQueueDoorbells();

			if (submissionQueueId == ADMIN_QUEUE_ID)
			{
				// Admin command
				LOG_INFO(command->toString());

				switch (command->DWord0Breakdown.OPC)
				{
				case 0x18: //Keep Alive... no data should be easiest
					postCompletion(*theCompletionQueue, COMPLETION_QUEUE_ENTRY());
				}
			}
			else
			{
				// NVM command
				assert(0); // Don't send me NVM commands yet.
			}
		}

		Queue* Controller::getQueueWithId(std::vector<Queue> &queues, UINT_16 id)
		{
			for (size_t i = 0; i < queues.size(); i++)
			{
				if (queues[i].getQueueId() == id)
				{
					return &queues[i];
				}
			}

			LOG_ERROR("Invalid queue id specified: " + std::to_string(id));
			return nullptr;
		}

		void Controller::postCompletion(Queue &completionQueue, COMPLETION_QUEUE_ENTRY completionEntry)
		{
			completionQueue.setIndex(completionQueue.getIndex() + 1);

			Queue* submissionQueue = completionQueue.getMappedQueue();
			completionEntry.SQID = submissionQueue->getQueueId();
			completionEntry.SQHD = submissionQueue->getIndex();
			completionEntry.CID = submissionQueue->getIndex(); // This is Wrong. Fix later
			
			memcpy((void*)completionQueue.getMemoryAddress(), &completionEntry, sizeof(completionEntry));

			// ring doorbell after placing data in completion queue.
			UINT_16* dbell = completionQueue.getDoorbell();
			dbell[0]++;
		}

		void Controller::controllerResetCallback()
		{
			LOG_INFO("Recv'd a controllerResetCallback request.");

			for (size_t i = ValidSubmissionQueues.size() - 1; i != -1; i--)
			{
				if (ValidSubmissionQueues[i].getQueueId() != ADMIN_QUEUE_ID)
				{
					ValidSubmissionQueues.erase(ValidSubmissionQueues.begin() + i);
				}
			}

			for (size_t i = ValidCompletionQueues.size() - 1; i != -1; i--)
			{
				if (ValidCompletionQueues[i].getQueueId() != ADMIN_QUEUE_ID)
				{
					ValidCompletionQueues.erase(ValidCompletionQueues.begin() + i);
				}
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
