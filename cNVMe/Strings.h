/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
Strings.h - A header file for the Strings (helper) functions
*/

#pragma once

#include "Types.h"

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
		/// indents a given string by 2 spaces
		/// </summary>
		std::string indentLines(std::string str);
	}
}