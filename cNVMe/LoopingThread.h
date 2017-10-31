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
		/// Base constructor
		/// </summary>
		LoopingThread();

		/// <summary>
		/// Copy constructor
		/// </summary>
		/// <param name="other">Another LoopingThread to copy from</param>
		LoopingThread(const LoopingThread &other);

		/// <summary>
		/// Assignment operator
		/// </summary>
		/// <param name="other">Another LoopingThread to copy from</param>
		/// <returns>LoopingThread</returns>
		LoopingThread& operator=(const LoopingThread& other);

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
		/// true if the thread is running
		/// </summary>
		std::atomic<bool> IsRunning;

		/// <summary>
		/// bool to say if we should keep looping or not
		/// </summary>
		std::atomic<bool> ContinueLoop;

		/// <summary>
		/// This flips with each loop run
		/// </summary>
		std::atomic<bool> Flipper;

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
