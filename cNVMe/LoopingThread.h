/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
LoopingThread.h - A header file for the LoopingThread class
*/

#pragma once

#include "Types.h"

namespace cnvme
{
	/// <summary>
	/// This class can do something over and over and over
	/// </summary>
	class LoopingThread
	{
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="functionToLoop">The function to loop</param>
		/// <param name="sleepDuration">Time in ms to sleep between loop iterations</param>
		LoopingThread(std::function<void()> functionToLoop, UINT_64 sleepDuration);

		/// <summary>
		/// Destructor
		/// </summary>
		~LoopingThread();

		/// <summary>
		/// Starts the LoopingThread
		/// </summary>
		void start();

		/// <summary>
		/// Ends the LoopingThread
		/// </summary>
		void end();

		/// <summary>
		/// Says if the thread is running
		/// </summary>
		bool isRunning();

		/// <summary>
		/// Returns true upon a completion of a loopingFunction iteration.
		/// Returns false if the thread isn't running
		/// </summary>
		/// <returns></returns>
		bool waitForFlip();

	private:
		/// <summary>
		/// The function to loop
		/// </summary>
		std::function<void()> FunctionToLoop;

		/// <summary>
		/// Time in ms to sleep between loop iterations
		/// </summary>
		UINT_64 SleepDuration;

		/// <summary>
		/// The thread that will be running
		/// </summary>
		std::thread TheThread;

		/// <summary>
		/// Low level mutex to know if the function is looping
		/// </summary>
		std::mutex RunningMutex;

		/// <summary>
		/// Higher level wrapper for the RunningMutex
		/// </summary>
		std::unique_lock<std::mutex> RunningMutexLock;

		/// <summary>
		/// bool to say if we should keep looping or not
		/// </summary>
		bool ContinueLoop;

		/// <summary>
		/// This flips with each loop run
		/// </summary>
		bool Flipper;

		/// <summary>
		/// Used to wait for a flip to happen (a loop iteration to occur)
		/// </summary>
		std::condition_variable FlipCondition;

		/// <summary>
		/// A mutex for the FlipCondition
		/// </summary>
		std::mutex FlipMutex;

		/// <summary>
		/// The function that does the looping
		/// </summary>
		void loopingFunction();

	};
}
