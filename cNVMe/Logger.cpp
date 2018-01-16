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
Logger.cpp - An implementation file for the Logging
*/

#include "Logger.h"
#include "Types.h"

#include <ctime>
#include <time.h>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace cnvme
{
	namespace logging
	{
		Logger::Logger()
		{
			Level = SILENT;
			AssertLoud = true;
			clearStatus();
		}

		void Logger::setLevel(LOGGING_LEVEL level)
		{
			Level = level;
		}

		LOGGING_LEVEL Logger::getLevel() const
		{
			return Level;
		}

		void Logger::log(std::string txt, LOGGING_LEVEL level)
		{
			Mutex.lock();
			if (level <= Level && HiddenThreads.find(std::this_thread::get_id()) == HiddenThreads.end())
			{
				std::cerr << getCurrentTime() << " - " << txt << std::endl;
			}
			Mutex.unlock();
		}

		void Logger::setStatus(std::string status)
		{
			Mutex.lock();
			Status = status;
			Mutex.unlock();
		}

		std::string Logger::getStatus()
		{
			return Status;
		}

		void Logger::clearStatus()
		{
			setStatus(CLEARED_STATUS);
		}

		void Logger::setAssertQuiet(bool assertQuiet, const std::thread::id& threadId)
		{
			Mutex.lock();
			if (assertQuiet)
			{
				AssertQuietThreads.insert(threadId);
			}
			else
			{
				auto itr = AssertQuietThreads.find(threadId);
				if (itr != AssertQuietThreads.end())
				{
					AssertQuietThreads.erase(itr);
				}
			}
			Mutex.unlock();
		}

		void Logger::addHiddenThread(const std::thread::id& hiddenThread)
		{
			Mutex.lock();
			HiddenThreads.insert(hiddenThread);
			Mutex.unlock();
		}

		void Logger::removeHiddenThread(const std::thread::id& hiddenThread)
		{
			Mutex.lock();
			auto itr = HiddenThreads.find(hiddenThread);
			if (itr != HiddenThreads.end())
			{
				HiddenThreads.erase(itr);
			}
			Mutex.unlock();
		}

		std::string Logger::getCurrentTime()
		{
			char buffer[80] = "\0";

			time_t rawtime;
			struct tm timeinfo = { 0 };

			time(&rawtime);
#ifdef _WIN32
			// This isn't even spec compliant or match cppreference.com... just a sketchy Microsoft extension ... 'secure'... lol
			localtime_s(&timeinfo, &rawtime);
#else // Linux
			memcpy_s(&timeinfo, sizeof(timeinfo), localtime(&rawtime), sizeof(tm));
#endif 
			strftime(buffer, sizeof(buffer), "%d-%m-%Y %I:%M:%S", &timeinfo);

			return buffer;
		}

		void Logger::_assert(std::string funcName, std::string txt, unsigned long long line)
		{
			std::string finalTxt = "cNVMe ASSERT! " + funcName + "():" + std::to_string(line) + " - " + std::string(txt);
			cnvme::logging::theLogger.setStatus(finalTxt);
			Mutex.lock();
			if (AssertQuietThreads.find(std::this_thread::get_id()) == AssertQuietThreads.end()) // not a quiet thread
			{
				std::cerr << finalTxt << std::endl;
			}
			Mutex.unlock();


#ifdef _DEBUG // only throw on debug builds
			throw std::runtime_error(finalTxt);
#endif // _DEBUG
		}

		void Logger::_assert_if(std::string funcName, bool condition, std::string txt, unsigned long long line)
		{
			if (condition)
			{
				_assert(funcName, txt, line);
			}
		}

		Logger theLogger;
	}
}
