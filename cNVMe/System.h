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
System.h - A header file for a System-specific calls for cNVMe
*/

#pragma once

#include "Types.h"

namespace cnvme
{
	namespace system
	{
		/// <summary>
		/// Returns the folder that cNVMe's binary data is in.
		/// If we are a DLL build, this is the folder the DLL is in
		/// </summary>
		std::string getCNVMeFolder();

		/// <summary>
		/// Returns the total capacity of the drive the executable is on
		/// </summary>
		UINT_64 getTotalLocalCapacityInBytes();

		/// <summary>
		/// Returns the total usable/unallocated capacity of the drive the executable is on
		/// </summary>
		UINT_64 getUnallocatedLocalCapacityInBytes();
	}
}
