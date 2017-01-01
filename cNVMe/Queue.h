/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
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
			Queue(UINT_32 queueSize, UINT_32 queueId, UINT_16* doorbell);

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

		};
	}
}