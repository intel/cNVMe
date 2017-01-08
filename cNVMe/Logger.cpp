/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2017
Logger.cpp - An implementation file for the Logging
*/

#include "Logger.h"

#include <ctime>
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
		}

		void Logger::setLevel(LOGGING_LEVEL level)
		{
			Level = level;
		}

		LOGGING_LEVEL Logger::getLevel() const
		{
			return Level;
		}

		void cnvme::logging::Logger::log(std::string txt, LOGGING_LEVEL level)
		{
			if (level <= Level)
			{
				Mutex.lock();
				std::cerr << getCurrentTime() << " - " <<  txt << std::endl;
				Mutex.unlock();
			}
		}

		std::string Logger::getCurrentTime()
		{
			time_t rawtime;
			struct tm timeinfo = { 0 };
			char buffer[80] = "\0";

			time(&rawtime);
			localtime_s(&timeinfo , &rawtime);

			strftime(buffer, sizeof(buffer), "%d-%m-%Y %I:%M:%S", &timeinfo);
			return buffer;
		}

		Logger theLogger;
	}
}

