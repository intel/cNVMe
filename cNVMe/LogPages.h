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
LogPages.h - A header file for the the NVMe Get Log Page Command
*/

#pragma once

#include "Types.h"

namespace cnvme
{
	namespace log_pages
	{
		/// <summary>
		/// Firmware Slot Information Log for LID=0x03
		/// </summary>
		typedef struct _FIRMWARE_SLOT_INFO
		{
			struct
			{
				UINT_8 ActiveFirmwareSlot : 3;
				UINT_8 RSVD_3 : 1;
				UINT_8 FirmwareSlotForNextControllerReset : 3;
				UINT_8 RSVD_7 : 1;
			} AFI;

			UINT_8 RSVD_01_07[7];
			char FRS[7][8]; // 8 characters per FW revision, 7 total slots
			UINT_8 RSVD_64_511[448];
		} FIRMWARE_SLOT_INFO, *PFIRMWARE_SLOT_INFO;

		static_assert(sizeof(FIRMWARE_SLOT_INFO) == 512, "Firmware Slot Info is 512 bytes");
	}
}