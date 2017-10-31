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
Command.h - A header file for the NVMe Command
*/

#pragma once

#include "Types.h"

namespace cnvme
{
	namespace command
	{
		/// <summary>
		/// Command DWord 0
		/// </summary>
		typedef struct DWORD0
		{
			UINT_32 OPC : 8; // Opcode
			UINT_32 FUSE : 2; // Fused Operation
			UINT_32 Reserved0 : 4; // Reserved
			UINT_32 PSDT : 2; // PRP or SGL for Data Transfer
			UINT_32 CID : 16; // Command Identifier

			std::string toString() const;
		}DWORD0, *PDWORD0;
		static_assert(sizeof(DWORD0) == 4, "DWORD0 should be 4 byte(s) in size.");

		/// <summary>
		/// Metadata pointer
		/// </summary>
		typedef struct MPTR
		{
			union
			{
				UINT_32 MPTR1; // Metadata Pointer 1
				UINT_32 DWord4;
			};

			union
			{
				UINT_32 MPTR2; // Metadata Pointer 2
				UINT_32 DWord5;
			};

			std::string toString() const;
		}MPTR, *PMPTR;
		static_assert(sizeof(MPTR) == 8, "MPTR should be 8 byte(s) in size.");

		/// <summary>
		/// Data pointer
		/// </summary>
		typedef struct DPTR
		{
			union
			{
				UINT_64 DPTR1; // Data Pointer 1
				struct
				{
					UINT_32 DWord6;
					UINT_32 DWord7;
				};
			};

			union
			{
				UINT_64 DPTR2; // Data Pointer 2
				struct
				{
					UINT_32 DWord8;
					UINT_32 DWord9;
				};
			};

			std::string toString() const;
		}DPTR, *PDPTR;
		static_assert(sizeof(DPTR) == 16, "DPTR should be 16 byte(s) in size.");


		/// <summary>
		/// Complete NVMe Command
		/// </summary
		typedef struct NVME_COMMAND
		{
			union
			{
				UINT_32 DWord0;
				DWORD0 DWord0Breakdown;
			};

			union
			{
				UINT_32 NSID; // Namespace Id
				UINT_32 DWord1;
			};

			UINT_32 DWord2; // Reserved
			UINT_32 DWord3; // Reserved

			union
			{
				MPTR MPTR; // Metadata Pointer
				UINT_64 CompleteMPTR;
			};

			union
			{
				DPTR DPTR; // Data Pointer
				UINT_64 SGL1[2]; // Scatter Gather List Entry 1 (placeholder)
			};

			UINT_32 DWord10; // Command Specific DW10
			UINT_32 DWord11; // Command Specific DW11
			UINT_32 DWord12; // Command Specific DW12
			UINT_32 DWord13; // Command Specific DW13
			UINT_32 DWord14; // Command Specific DW14
			UINT_32 DWord15; // Command Specific DW15

			std::string toString() const;
		}NVME_COMMAND, *PNVME_COMMAND;
		static_assert(sizeof(NVME_COMMAND) == 64, "NVME_COMMAND should be 64 byte(s) in size.");

		typedef struct COMPLETION_QUEUE_ENTRY
		{
			UINT_32 DWord0; // Command Specific
			UINT_32 DWord1; // Reserved

			union
			{
				struct
				{
					UINT_16 SQHD; //SQ Head Pointer
					UINT_16 SQID; //SQ Identifier;
				};
				UINT_32 DWord2;
			};

			union
			{
				struct
				{
					UINT_16 CID; // Command Identifier
					union
					{
						struct
						{
							UINT_16 P : 1; // Phase Tag
							UINT_16 SF : 15; // Status Field
						};
						struct
						{
							UINT_16 RSVD0 : 1; // Reserved
							UINT_16 SC : 8; // Status Code
							UINT_16 SCT : 3; // Status Code Type
							UINT_16 RSVD1 : 2; // Reserved
							UINT_16 M : 1; // More
							UINT_16 DNR : 1; // Do Not Retry
						};
					};
				};
				UINT_32 DWord3;
			};

			std::string toString() const;
		}COMPLETION_QUEUE_ENTRY, *PCOMPLETION_QUEUE_ENTRY;
		static_assert(sizeof(COMPLETION_QUEUE_ENTRY) == 16, "COMPLETION_QUEUE_ENTRY should be 16 byte(s) in size.");

	}
}
