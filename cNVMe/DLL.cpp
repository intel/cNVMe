/*
###########################################################################################
// cNVMe - An Open Source NVMe Device Simulation - MIT License
// Copyright 2018 - Intel Corporation

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
DLL.cpp - Implementation file for DLL exports
*/

#ifdef DLL_BUILD
#include "DLL.h"

using namespace cnvme;
static driver::Driver* staticDriver;

typedef enum StatusCodes {
	NO_ERRORS,
	ALREADY_INITIALIZED,
	ALREADY_UNINITIALIZED
} StatusCodes;

char* getCharStarOfStringToSendOut(std::string retStr)
{
	// Add 1 for \0
	size_t allocatedRetStrSize = retStr.size() + 1;
	char* allocatedRetStr = new char[allocatedRetStrSize];
	memcpy_s(allocatedRetStr, allocatedRetStrSize, retStr.c_str(), retStr.size());
	allocatedRetStr[allocatedRetStrSize - 1] = 0; // terminating NULL
	return allocatedRetStr; // To the heavens above: Please free me.
}

long Initialize()
{
	if (!staticDriver)
	{
		staticDriver = new driver::Driver();
		return NO_ERRORS;
	}

	return ALREADY_INITIALIZED;
}

long SendCommand(UINT_8* driverCommandData, size_t driverCommandDataLength)
{
	if (staticDriver)
	{
		staticDriver->sendCommand(driverCommandData, driverCommandDataLength);
		return NO_ERRORS;
	}

	return ALREADY_UNINITIALIZED;
}

char* GetStatusString(long statusCode)
{
	std::string retStr = "Unknown Status";
	if (statusCode == NO_ERRORS)
	{
		retStr = "Completed Successfully";
	}
	else if (statusCode == ALREADY_INITIALIZED)
	{
		retStr = "The DLL was already initialized";
	}
	else if (statusCode == ALREADY_UNINITIALIZED)
	{
		retStr = "The DLL was already uninitialized";
	}

	return getCharStarOfStringToSendOut(retStr);
}

char* GetDriverStatusString(long statusCode)
{
	std::string retStr = driver::statusToString((driver::Status)statusCode);

	return getCharStarOfStringToSendOut(retStr);
}

long Uninitialze()
{
	if (staticDriver)
	{
		delete staticDriver;
		staticDriver = nullptr;
		return NO_ERRORS;
	}

	return ALREADY_UNINITIALIZED;
}

#endif // DLL_BUILD
