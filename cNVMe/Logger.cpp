/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2017
Logger.cpp - An implementation file for the Logging
*/

#include "Logger.h"

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
			if (level <= Level)
			{
				Mutex.lock();
				std::cerr << getCurrentTime() << " - " << txt << std::endl;
				Mutex.unlock();
			}
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

		std::string Logger::getCurrentTime()
		{
			char buffer[80] = "\0";
#ifdef _WIN32
			time_t rawtime;
			struct tm timeinfo = { 0 };


			time(&rawtime);
			localtime_s(&timeinfo, &rawtime);

			strftime(buffer, sizeof(buffer), "%d-%m-%Y %I:%M:%S", &timeinfo);
#endif
			return buffer;
		}

		Logger theLogger;
	}
}

