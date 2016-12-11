/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
Strings.cpp - An implementation file for the Strings (helper) functions
*/

#include "Strings.h"

#include <cstdio>
#include <regex>

namespace cnvme
{
	namespace strings
	{
		std::string rfill(std::string str, unsigned int width)
		{
			while (str.size() < width)
			{
				str += " ";
			}
			return str;
		}

		std::string toHexString(UINT_64 value)
		{
			char buffer[BUFSIZ] = "\0";

			snprintf(buffer, sizeof(buffer), "%llX", value);

			return buffer; // should be auto-null terminated
		}

		std::string toString(UINT_64 value, std::string abbreviation, std::string description)
		{
			return "  " + rfill(abbreviation, 6) + " : " + rfill(description, 35) + " : " + std::to_string(value) + " (0x" + toHexString(value) + ") \n";
		}
		std::string indentLines(std::string str)
		{
			str = "  " + std::regex_replace(str, std::regex("\n"), "\n  ");

			while (*str.rbegin() == ' ')
			{
				str.erase(str.size() - 1);
			}

			return str;
		}
	}
}