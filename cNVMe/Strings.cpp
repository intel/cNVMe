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
			return "  " + rfill(abbreviation, 10) + " : " + rfill(description, 45) + " : " + std::to_string(value) + " (0x" + toHexString(value) + ") \n";
		}

		std::string toString(bool value)
		{
			if (value)
			{
				return "True";
			}

			return "False";
		}

		std::string indentLines(std::string str)
		{
			str = "  " + strings::replace(str, "\n", "\n  ");

			while (*str.rbegin() == ' ')
			{
				str.erase(str.size() - 1);
			}

			return str;
		}

		std::string replace(const std::string &s, const std::string find, const std::string replace)
		{
			std::string retStr = std::string(s);
			size_t pos = 0;
			while ((pos = retStr.find(find, pos)) != std::string::npos) 
			{
				retStr.replace(pos, find.length(), replace);
				pos += replace.length();
			}
			return retStr;
		}
	}
}