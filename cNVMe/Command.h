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

			union
			{
				struct
				{
					union
					{
						struct
						{
							UINT_8 CNS; // Controller or Namespace Structure
							UINT_8 IDENTIFY_DW10_RSVD;
							UINT_16 CNTID; // Controller Identifier
						} DW10_Identify;

						struct
						{
							UINT_32 LBAF : 4; // LBA Format
							UINT_32 MSET : 1; // Metadata Settings
							UINT_32 PI : 2; // Protection Information
							UINT_32 PIL : 1; // Protection Information Location
							UINT_32 SES : 3; // Secure Erase Settings
							UINT_32 FORMAT_DW10_RSVD : 20;
						} DW10_Format;

						struct
						{
							UINT_16 QID; // Queue ID
							UINT_16 QSIZE; // Queue Size (in entries)
						} DW10_CreateIoQueue; // Both submission/completion

						struct
						{
							UINT_16 QID; // Queue ID
							UINT_16 DELETE_QUEUE_RSVD;
						} DW10_DeleteIoQueue; // Both submission/completion

						UINT_32 DWord10; // Command Specific DW10
					};

					union
					{
						struct
						{
							UINT_32 PC : 1; // Physically Contiguous
							UINT_32 IEN : 1; // Interrupts Enabled
							UINT_32 CREATE_IO_COMPLETION_QUEUE_DW11_RSVD : 14;
							UINT_32 IV : 16; // Interrupt Vector
						} DW11_CreateIoCompletionQueue;

						struct
						{
							UINT_32 PC : 1; // Physically Contiguous
							UINT_32 QPRIO : 2; // Queue Priority
							UINT_32 CREATE_IO_SUBMISSION_QUEUE_DW11_RSVD : 13;
							UINT_32 CQID : 16; // Completion Queue Identifier
						} DW11_CreateIoSubmissionQueue;

						UINT_32 DWord11; // Command Specific DW11
					};
				};
				UINT_64 SLBA; // Starting LBA
			};
			union
			{
				UINT_32 DWord12; // Command Specific DW12
				struct
				{
					UINT_32 NLB : 16; // Number of Logical Blocks
					UINT_32 READ_DW12_RSVD : 10;
					UINT_32 PRINFO : 4; // Protection Info
					UINT_32 FUA : 1; // Force Unit Access
					UINT_32 LR : 1; // Limited Retry
				} DW12_IO;
			};
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
