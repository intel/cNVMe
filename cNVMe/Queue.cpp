/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2017
Queue.cpp- A implementation file for the NVMe Queues
*/

#include "Command.h"
#include "Queue.h"

namespace cnvme
{
	namespace controller
	{
		Queue::Queue()
		{
			QueueSize = 0;
			QueueId = 0;
			Doorbell = nullptr;
			HeadPointer = 0; // Queue start at 0
			TailPointer = 0; // Queue start at 0
			LinkedMemoryAddress = 0;
			MappedQueue = nullptr;
		}

		Queue::Queue(UINT_32 queueSize, UINT_32 queueId, UINT_16* doorbell, UINT_64 linkedMemoryAddress) : Queue()
		{
			QueueSize = queueSize;
			QueueId = queueId;
			Doorbell = doorbell;
			LinkedMemoryAddress = linkedMemoryAddress;
		}

		UINT_32 Queue::getQueueSize() const
		{
			return QueueSize;
		}

		UINT_32 Queue::getQueueId() const
		{
			return QueueId;
		}

		UINT_16* Queue::getDoorbell()
		{
			return Doorbell;
		}

		UINT_32 Queue::getHeadPointer()
		{
			return HeadPointer;
		}

		UINT_32 Queue::getTailPointer()
		{
			return TailPointer;
		}

		bool Queue::setTailPointer(UINT_32 newIndex)
		{
			if (newIndex < getQueueSize())
			{
				TailPointer = newIndex;
				return true;
			}

			// Should trigger AER.
			return false;
		}

		UINT_16 Queue::incrementAndGetHeadCloserToTail()
		{
			ASSERT_IF(HeadPointer == TailPointer, "HeadPointer == TailPointer. Should not be incrementing.");

			HeadPointer++;
			HeadPointer %= getQueueSize();
			if (HeadPointer > TailPointer)
			{
				return getQueueSize() - HeadPointer + TailPointer; // Wrapped around
			}
			return TailPointer - HeadPointer;                      // Not wrapped around
		}

		UINT_64 Queue::getMemoryAddress()
		{
			return LinkedMemoryAddress;
		}

		void Queue::setMemoryAddress(UINT_64 memoryAddress)
		{
			LinkedMemoryAddress = memoryAddress;
		}

		UINT_32 Queue::getQueueMemorySize()
		{
			return getQueueSize() * sizeof(cnvme::command::COMPLETION_QUEUE_ENTRY);
		}

		Queue * Queue::getMappedQueue()
		{
			return MappedQueue;
		}

		void Queue::setMappedQueue(Queue* mappedQueue)
		{
			MappedQueue = mappedQueue;
		}
	}
}
