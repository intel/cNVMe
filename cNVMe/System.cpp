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
System.cpp - An implementation file for a System-specific calls for cNVMe
*/

#include "System.h"

#ifdef _WIN32
#include <Windows.h>
#else // Linux
#include <unistd.h>
#include <sys/statvfs.h>
#endif // _WIN32

#define MAX_FILE_PATH 2048

namespace cnvme
{
	namespace system
	{
		std::string getExecutableFolder()
		{
			char filename[MAX_FILE_PATH] = "\0";
			INT_32 len = 0;

#ifdef _WIN32
			len = (INT_32)GetModuleFileNameA(NULL, filename, MAX_FILE_PATH);
#else // Linux
			len = (INT_32)readlink("/proc/self/exe", filename, MAX_FILE_PATH);
#endif // _WIN32
			
			ASSERT_IF(len < 0, "Failed to get the executable folder");
			std::string retStr = std::string(filename, len);
			return retStr.substr(0, retStr.find_last_of("\\/"));
		}

		UINT_64 getTotalLocalCapacityInBytes()
		{
			UINT_64 retBytes = 0;
#ifdef _WIN32
			GetDiskFreeSpaceExA(getExecutableFolder().c_str(), NULL, (PULARGE_INTEGER)&retBytes, NULL);
#else // Linux
			struct statvfs buf = { 0 };
			statvfs(getExecutableFolder().c_str(), &buf);
			retBytes = buf.f_blocks * buf.f_frsize;
#endif // _WIN32

			return retBytes;
		}

		UINT_64 getUnallocatedLocalCapacityInBytes()
		{
			UINT_64 retBytes = 0;
#ifdef _WIN32
			GetDiskFreeSpaceExA(getExecutableFolder().c_str(), NULL, NULL, (PULARGE_INTEGER)&retBytes);
#else // Linux
			struct statvfs buf = { 0 };
			statvfs(getExecutableFolder().c_str(), &buf);
			retBytes = buf.f_bfree  * buf.f_frsize;
#endif // _WIN32

			return retBytes;
		}
	}
}