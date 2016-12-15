#include "LoopingThread.h"
/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
LoopingThread.cpp - An implementation file for the LoopingThread class
*/

namespace cnvme
{
	LoopingThread::LoopingThread(std::function<void()> functionToLoop, UINT_64 sleepDuration)
	{
		FunctionToLoop = functionToLoop;
		SleepDuration = sleepDuration;
		RunningMutexLock = std::unique_lock<std::mutex>(RunningMutex);
		ContinueLoop = false;
		Flipper = false;
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
		}
	}

	void LoopingThread::end()
	{
		if (isRunning())
		{
			ContinueLoop = false;

			RunningMutexLock.lock();  // Shouldn't pass till the loopingFunction() ends
			RunningMutexLock.unlock();

			TheThread.join();
		}
	}

	bool LoopingThread::isRunning()
	{
		return RunningMutexLock.owns_lock();
	}

	bool LoopingThread::waitForFlip()
	{
		if (!isRunning())
		{
			return false;
		}

		bool cachedFlipper = Flipper;

		while (Flipper == cachedFlipper)
		{
			std::unique_lock<std::mutex> flipLock(FlipMutex);
			FlipCondition.wait(flipLock);
		}

		return true;
	}

	void LoopingThread::loopingFunction()
	{
		RunningMutexLock.lock();

		while (ContinueLoop)
		{
			std::unique_lock<std::mutex> flipLock(FlipMutex);
			FunctionToLoop();

			Flipper = !Flipper;
			FlipCondition.notify_all();
			std::this_thread::sleep_for(std::chrono::milliseconds(SleepDuration));
		}

		RunningMutexLock.unlock();
	}

}
