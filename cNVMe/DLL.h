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
DLL.h - Header file for DLL exports
*/
#pragma once

#ifdef DLL_BUILD
#include "Driver.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT extern
#endif // _WIN32

/* Exported function declarations
*
* Rules:
* 1. All parameters must be primitive C types 
* 2. All functions other than GetStatusString/GetDriverStatusString() should return a long (status code)
* 3. That status code can be converted to a string via GetStatusString
* 4. All functions should be Title Case
*
*/

	/// <summary>
	/// Must be the first exported method called. Initializes the DLL.
	/// </summary>
	EXPORT long Initialize();

	/// <summary>
	/// Used to send a command to the driver.
	/// Takes in the data used to make up the DRIVER_COMMAND along with the size of that buffer
	/// </summary>
	EXPORT long SendCommand(UINT_8* driveCommandData, size_t driveCommandDataLength);

	/// <summary>
	/// Returns a NULL-delmitied char* for the status string of the given status code.
	/// It is the responsibility of caller to free the memory returned!
	/// </summary>
	EXPORT char* GetStatusString(long statusCode);

	/// <summary>
	/// Returns a NULL-delmitied char* for the driver status string of the given status code.
	/// It is the responsibility of caller to free the memory returned!
	/// </summary>
	EXPORT char* GetDriverStatusString(long statusCode);

	/// <summary>
	/// Should be the last method called. Uninitializes the DLL
	/// </summary>
	EXPORT long Uninitialze();

	/// <summary>
	/// Performs a controller reset
	/// </summary>
	EXPORT long ControllerReset();

#undef EXPORT
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DLL_BUILD