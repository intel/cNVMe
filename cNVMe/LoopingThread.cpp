/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
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
		if (!isRunning())
		{
			return false;
		}

		// Get lock before any other flips can happen
		std::unique_lock<std::mutex> flipLock(FlipMutex);

		bool cachedFlipper = Flipper;

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
			FunctionToLoop();

			{
				std::unique_lock<std::mutex> flipLock(FlipMutex);
				Flipper = !Flipper;
				FlipCondition.notify_all();
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(SleepDuration));
		}

		RunningMutex.unlock();
	}

}
