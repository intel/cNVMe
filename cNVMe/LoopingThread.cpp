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
LoopingThread.cpp - An implementation file for the LoopingThread class
*/

#include "LoopingThread.h"

namespace cnvme
{
	LoopingThread::LoopingThread(std::function<void()> functionToLoop, UINT_64 sleepDuration) : LoopingThread::LoopingThread()
	{
		FunctionToLoop = functionToLoop;
		SleepDuration = sleepDuration;
	}

	LoopingThread::LoopingThread()
	{
		ContinueLoop = false;
		Flipper = false;
		IsRunning = false;
		SleepDuration = 0;
	}

	LoopingThread::LoopingThread(const LoopingThread & other) : LoopingThread::LoopingThread()
	{
		*this = other;
	}

	LoopingThread & LoopingThread::operator=(const LoopingThread & other)
	{
		// check for self-assignment
		if (&other == this)
		{
			return *this;
		}

		ContinueLoop = other.ContinueLoop.load();
		Flipper = other.Flipper.load();
		SleepDuration = other.SleepDuration;
		FunctionToLoop = other.FunctionToLoop;

		return *this;
	}

	LoopingThread::~LoopingThread()
	{
		end();
	}

	void LoopingThread::start()
	{
		if (!isRunning())
		{
			ContinueLoop = true;
			TheThread = std::thread(&LoopingThread::loopingFunction, this);
			IsRunning = true;
			waitForFlip(); // wait for one iteration, to make sure it is running
		}
	}

	void LoopingThread::end()
	{
		if (isRunning())
		{
			ContinueLoop = false;

			RunningMutex.lock();  // Shouldn't pass till the loopingFunction() ends
			IsRunning = false;
			RunningMutex.unlock();

			TheThread.join();
		}
	}

	bool LoopingThread::isRunning()
	{
		return IsRunning;
	}

	bool LoopingThread::waitForFlip()
	{
		// Get lock before any other flips can happen
		std::unique_lock<std::mutex> flipLock(FlipMutex);

		if (!isRunning())
		{
			return false;
		}

		bool cachedFlipper = Flipper.load();

		while (Flipper == cachedFlipper)
		{
			FlipCondition.wait(flipLock);
		}

		return true;
	}

	void LoopingThread::loopingFunction()
	{
		RunningMutex.lock();

		while (ContinueLoop)
		{
			{
				std::unique_lock<std::mutex> flipLock(FlipMutex);
				FunctionToLoop();
				Flipper = !Flipper;
				FlipCondition.notify_all();
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(SleepDuration));
		}

		RunningMutex.unlock();
	}

}
