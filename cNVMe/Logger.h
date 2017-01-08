/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2017
Logger.h - A header file for the Logging
*/

#pragma once

#include <mutex>
#include <string>

// Macros to make this easier to work with
#define LOG_ERROR(txt) cnvme::logging::theLogger.log(std::string(__func__) + "():" \
+ std::to_string(__LINE__) + " - [" + cnvme::logging::Logger::loggingLevelToString(cnvme::logging::ERROR) + "] - " + txt, cnvme::logging::ERROR);
#define LOG_INFO(txt) cnvme::logging::theLogger.log(std::string(__func__) + "():" \
+ std::to_string(__LINE__) + " - [" + cnvme::logging::Logger::loggingLevelToString(cnvme::logging::INFO) + "] - " + txt, cnvme::logging::INFO);
#define LOG_SET_LEVEL(level) cnvme::logging::theLogger.setLevel((cnvme::logging::LOGGING_LEVEL)level);

namespace cnvme
{
	namespace logging
	{
		/// <summary>
		/// Used to know the level of logging
		/// </summary>
		enum LOGGING_LEVEL
		{
			SILENT,
			ERROR,
			INFO,
		};

		class Logger
		{
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			Logger();

			/// <summary>
			/// Function to set the logging level
			/// </summary>
			/// <param name="level">Logging level</param>
			void setLevel(LOGGING_LEVEL level);

			/// <summary>
			/// Gets the current logging level
			/// </summary>
			/// <returns>The logging level</returns>
			LOGGING_LEVEL getLevel() const;

			/// <summary>
			/// Logs the given text if the level is higher than required for the stream
			/// </summary>
			/// <param name="txt"></param>
			void log(std::string txt, LOGGING_LEVEL level);

			/// <summary>
			/// Helper to get a string from the logging level
			/// </summary>
			/// <param name="level">level to convert to string</param>
			/// <returns>String of the level</returns>
			static std::string loggingLevelToString(LOGGING_LEVEL level)
			{
				if (level == ERROR)
				{
					return "Error";
				}
				else if (level == INFO)
				{
					return "Info";
				}
				return "Unknown";
			}

		private:
			/// <summary>
			/// Current level of logging
			/// </summary>
			LOGGING_LEVEL Level;

			/// <summary>
			/// Gets the current time as a string
			/// </summary>
			std::string getCurrentTime();

			/// <summary>
			/// Mutex for printing
			/// </summary>
			std::mutex Mutex;
		};

		/// <summary>
		/// Global logger, used for logging
		/// </summary>
		extern Logger theLogger;
	}
}