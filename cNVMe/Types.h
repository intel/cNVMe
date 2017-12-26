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
Types.h - A header file for all needed type includes
*/

// Project Includes
#include "Logger.h"
#include "Payload.h" // Needs to be before ifndef

#ifndef _TYPES_H // Some versions of G++ are not liking #pragma once mid file
#define _TYPES_H

// STL Includes
#include <algorithm>
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <list>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

// C Includes
#include <stdio.h>
#include <string.h>

// Unsigned values
typedef uint8_t  UINT_8;
typedef uint16_t UINT_16;
typedef uint32_t UINT_32;
typedef uint64_t UINT_64;

// Signed vales
typedef int8_t   INT_8;
typedef int16_t  INT_16;
typedef int32_t  INT_32;
typedef int64_t  INT_64;

// Cause I'm lazy
typedef UINT_8 BYTE;

// Type Macros
#define POINTER_TO_MEMORY_ADDRESS(p) (UINT_64)&(*p)
#define MEMORY_ADDRESS_TO_8POINTER(m) (UINT_8*)m
#define MEMORY_ADDRESS_TO_64POINTER(m) (UINT_64*)m

// Init macro
#define ALLOC_BYTE_ARRAY(name, size) BYTE* name = new BYTE[size]; memset(name, 0, size);

// Macro for code clarity
#define ONE_BASED_FROM_ZERO_BASED(number) (number + 1)

#ifndef _WIN32
typedef int errno_t; // Should already exist on Windows
// Lets see how spec compliant we can be!
inline errno_t memcpy_s(void *dest, size_t destSize, const void *src, size_t count) //todo: Types.cpp or Memory.h/.cpp?
{
	errno_t retVal = 0;
	if (count == 0)
	{
		retVal = 0;
	}
	else if (dest == NULL && count > 0)
	{
		retVal = EINVAL;
	}
	else if (src == NULL && count > 0)
	{
		retVal = EINVAL;
		memset(dest, 0, destSize); // seems weird but matches behavior from Microsoft's implementation
	}
	else if (destSize < count)
	{
		retVal = ERANGE;
		memset(dest, 0, destSize); // seems weird but matches behavior from Microsoft's implementation
	}

	if (retVal == 0)
	{
		memcpy(dest, src, count);
	}
	else
	{
		ASSERT("Invalid memcpy_s call... destSize (" + std::to_string(destSize) + "), srcSize (" + std::to_string(count) + ")");
	}

	return retVal;
}
#endif // _WIN32
#endif //_TYPES_H
