/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2017
Queue.h - A header file for the NVMe Queues
*/

#pragma once

#include "Types.h"

namespace cnvme
{
	namespace controller
	{
		/// <summary>
		/// Represents either a submission or completion queue
		/// </summary>
		class Queue
		{
		public:
			/// <summary>
			/// Constructor
			/// </summary>	
			Queue();

			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="queueSize">The queue size</param>
			/// <param name="queueId">The queue id</param>
			/// <param name="doorbell">The queue doorbell register (tail or head)</param>	
			/// <param name="linkedMemoryAddress">The memory this queue uses for operations</param>
			Queue(UINT_32 queueSize, UINT_32 queueId, UINT_16* doorbell, UINT_64 linkedMemoryAddress);

			/// <summary>
			/// Destructor
			/// </summary>
			~Queue() = default;

			/// <summary>
			/// Returns the queue size
			/// </summary>
			/// <returns>queue size</returns>
			UINT_32 getQueueSize() const;

			/// <summary>
			/// Returns the queue id
			/// </summary>
			/// <returns>queue id</returns>
			UINT_32 getQueueId() const;

			/// <summary>
			/// Returns the raw doorbell
			/// </summary>
			/// <returns>pointer to doorbell</returns>
			UINT_16* getDoorbell();

			/// <summary>
			/// Gets the current index
			/// </summary>
			/// <returns>index</returns>
			UINT_32 getIndex() const;

			/// <summary>
			/// Sets the current index
			/// </summary>
			/// <param name="newIndex">the new index</param>
			void setIndex(UINT_32 newIndex);

			/// <summary>
			/// Returns the address of the linked memory
			/// </summary>
			/// <returns>Address</returns>
			UINT_64 getMemoryAddress();

			/// <summary>
			/// Gets a pointer to the mapped queue
			/// </summary>
			/// <returns>Pointer to mapped queue</returns>
			Queue* getMappedQueue();

			/// <summary>
			/// Set the mapped queue. If Queue is a sub queue, this is the completion queue.
			/// </summary>
			/// <param name="mappedQueue">Linked queue</param>
			void setMappedQueue(Queue* mappedQueueId);

		private:

			/// <summary>
			/// The max size of the queue
			/// </summary>
			UINT_32 QueueSize;

			/// <summary>
			/// The identifier for the queue
			/// </summary>
			UINT_32 QueueId;

			/// <summary>
			/// Pointer to the raw doorbell
			/// </summary>
			UINT_16* Doorbell;

			/// <summary>
			/// Current Index in the queue
			/// </summary>
			UINT_32 Index;

			/// <summary>
			/// The memory for this queue to use.
			/// Submission queues will expect the 64 byte CDB here
			/// Completion queues will expect to place completion here
			/// </summary>
			UINT_64 LinkedMemoryAddress;

			/// <summary>
			/// Queue that this queue is mapped to
			/// Example: If this is the admin submission queue, then this should be a pointer to the admin completion queue
			/// </summary>
			Queue* MappedQueue;
		};
	}
}