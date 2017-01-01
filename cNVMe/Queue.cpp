/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
Queue.cpp- A implementation file for the NVMe Queues
*/

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
			Index = 0; // Queue start at 0
			LinkedMemoryAddress = 0;
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

		UINT_32 Queue::getIndex() const
		{
			return Index;
		}

		void Queue::setIndex(UINT_32 newIndex)
		{
			Index = newIndex;
		}

		UINT_64 Queue::getMemoryAddress()
		{
			return LinkedMemoryAddress;
		}
	}
}
