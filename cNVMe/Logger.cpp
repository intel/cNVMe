/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2017
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
			Status = status;
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

		void Logger::_assert(std::string funcName, std::string txt)
		{
			std::string finalTxt = "cNVMe ASSERT! " + funcName + "():" + std::to_string(__LINE__) + " - " + std::string(txt);
			cnvme::logging::theLogger.setStatus(finalTxt);
			if (AssertQuietThreads.find(std::this_thread::get_id()) == AssertQuietThreads.end()) // not a quiet thread
			{
				Mutex.lock();
				std::cerr << finalTxt << std::endl;
				Mutex.unlock();
			}

#ifdef _DEBUG // only throw on debug builds
			throw std::exception(finalTxt.c_str());
#endif // _DEBUG
		}

		void Logger::_assert_if(std::string funcName, bool condition, std::string txt)
		{
			if (condition)
			{
				_assert(funcName, txt);
			}
		}

		Logger theLogger;
	}
}
