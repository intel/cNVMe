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
Strings.h - A header file for the Strings (helper) functions
*/

#pragma once

#include "Types.h"

/// <summary>
/// Used to get the params for strings::toString
/// </summary>
#define ToStringParams(abbreviation, description) \
	abbreviation, #abbreviation, description

namespace cnvme
{
	namespace strings
	{
		/// <summary>
		/// Fills a string with a given width
		/// </summary>
		/// <param name="str">given string to fill</param>
		/// <param name="width">width</param>
		/// <returns>Filled string</returns>
		std::string rfill(std::string str, unsigned int width);

		/// <summary>
		/// Converts the given value to a hex string (without to leading 0x)
		/// </summary>
		std::string toHexString(UINT_64 value);

		/// <summary>
		/// Takes a value, abbreviation, description and returns a nice looking string representation
		/// </summary> 
		std::string toString(UINT_64 value, std::string abbreviation, std::string description);
	
		/// <summary>
		/// Converts bool to string
		/// </summary>
		std::string toString(bool value);

		/// <summary>
		/// indents a given string by 2 spaces
		/// </summary>
		std::string indentLines(std::string str);

		/// <summary>
		/// Find and replace a string and return the result
		/// </summary>
		std::string replace(const std::string &s, const std::string find, const std::string replace);
	}
}