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
			/// Get the head pointer
			/// </summary>
			/// <returns>Gets the head pointer</returns>
			UINT_32 getHeadPointer();

			/// <summary>
			/// Get the tail pointer
			/// </summary>
			/// <returns>Gets the tail pointer</returns>
			UINT_32 getTailPointer();

			/// <summary>
			/// Sets the tail pointer index
			/// </summary>
			/// <param name="newIndex">the new index</param>	 
			/// <returns>True if successful. False if the new index is out of bounds</returns>
			bool setTailPointer(UINT_32 newIndex);

			/// <summary>
			/// Add 1 to the Head Pointer to get it closer to the tail
			/// Will ASSERT if incremented past the tail.
			/// </summary>
			/// <returns>Distance to tail from the new head</returns>
			UINT_16 incrementAndGetHeadCloserToTail();

			/// <summary>
			/// Returns the address of the linked memory
			/// </summary>
			/// <returns>Address</returns>
			UINT_64 getMemoryAddress();

			/// <summary>
			/// Sets the memory address of the queue.
			/// </summary>
			void setMemoryAddress(UINT_64 memoryAddress);

			/// <summary>
			/// Returns the assumed size of the complete queue in bytes
			/// </summary>
			/// <returns>Size in Bytes</returns>
			UINT_32 getQueueMemorySize();

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
			/// Head pointer of the queue
			/// </summary>
			UINT_32 HeadPointer;

			/// <summary>
			/// Tail pointer of the queue
			/// </summary>
			UINT_32 TailPointer;

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