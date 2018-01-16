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

#define MAX_FILE_PATH 4096

namespace cnvme
{
	namespace system
	{
		std::string getCNVMeFolder()
		{
			char filename[MAX_FILE_PATH] = "\0";
			INT_32 len = 0;
			std::string retStr = "";

#ifdef _WIN32
			HMODULE mod = NULL; // for exe builds
#if DLL_BUILD
			// for DLL builds, get the location of the DLL
			mod = GetModuleHandleA("cNVMe.dll"); // todo: if i change this to be like Linux and have 32/64 in naming, change this.
#endif // DLL_BUILD
			len = (INT_32)GetModuleFileNameA(mod, filename, MAX_FILE_PATH);
	   // ^ _WIN32
#else // Linux

#if DLL_BUILD
			// for DLL builds, get the location of the DLL
			std::string cmd = "pmap -p -q " + std::to_string(getpid()) + " | grep -i cnvme | awk \'{print $4}\' | tail -1";
			FILE* fp = popen(cmd.c_str(), "r");
			ASSERT_IF(fp == NULL, "popen() failed!");

			// get command output
			while (fgets(filename, MAX_FILE_PATH, fp) != NULL) 
			{
				retStr += std::string(filename);
			}
			pclose(fp);
			len = strlen(filename);      // set len so the rest of the code works
			*filename = *retStr.c_str(); // point the filename to this so the rest of the code works

	  // ^ DLL_BUILD
#else // EXE_BUILD
			len = (INT_32)readlink("/proc/self/exe", filename, MAX_FILE_PATH);
	  // ^ EXE_BUILD
#endif // DLL_BUILD

#endif // _WIN32
			
			ASSERT_IF(len < 0, "Failed to get the executable folder");
			retStr = std::string(filename, len);
			return retStr.substr(0, retStr.find_last_of("\\/"));
		}

		UINT_64 getTotalLocalCapacityInBytes()
		{
			UINT_64 retBytes = 0;
#ifdef _WIN32
			GetDiskFreeSpaceExA(getCNVMeFolder().c_str(), NULL, (PULARGE_INTEGER)&retBytes, NULL);
#else // Linux
			struct statvfs buf = { 0 };
			statvfs(getCNVMeFolder().c_str(), &buf);
			retBytes = buf.f_blocks * buf.f_frsize;
	  // ^ Linux
#endif // _WIN32

			return retBytes;
		}

		UINT_64 getUnallocatedLocalCapacityInBytes()
		{
			UINT_64 retBytes = 0;
#ifdef _WIN32
			GetDiskFreeSpaceExA(getCNVMeFolder().c_str(), NULL, NULL, (PULARGE_INTEGER)&retBytes);
#else // Linux
			struct statvfs buf = { 0 };
			statvfs(getCNVMeFolder().c_str(), &buf);
			retBytes = buf.f_bfree  * buf.f_frsize;
	  // ^ Linux
#endif // _WIN32

			return retBytes;
		}
	}
}