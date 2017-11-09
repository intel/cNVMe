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
Command.cpp - An implementation file for the NVMe Command
*/

#include "Command.h"
#include "Strings.h"

namespace cnvme
{
	namespace command
	{
		std::string DWORD0::toString() const
		{
			std::string retStr;
			retStr += "DWord0:\n";
			retStr += strings::toString(ToStringParams(OPC, "Opcode"));
			retStr += strings::toString(ToStringParams(FUSE, "Fused Operation"));
			retStr += strings::toString(ToStringParams(Reserved0, "Reserved"));
			retStr += strings::toString(ToStringParams(PSDT, "PRP or SGL for Data Transfer"));
			retStr += strings::toString(ToStringParams(CID, "Command Identifier"));
			return retStr;
		}

		std::string MPTR::toString() const
		{
			std::string retStr;
			retStr += "MPTR (Metadata Pointer):\n";
			retStr += strings::toString(ToStringParams(MPTR1, "MPTR1 (DWord 4)"));
			retStr += strings::toString(ToStringParams(MPTR2, "MPTR2 (DWord 5)"));
			return retStr;
		}

		std::string DPTR::toString() const
		{
			std::string retStr;
			retStr += "DPTR (Data Pointer):\n";
			retStr += strings::toString(ToStringParams(DWord6, "DWord6 / PRP1"));
			retStr += strings::toString(ToStringParams(DWord7, "DWord7 / PRP1"));
			retStr += strings::toString(ToStringParams(DPTR1, "DPTR1 (DWords 6/7)"));
			retStr += strings::toString(ToStringParams(DWord8, "DWord8 / PRP2"));
			retStr += strings::toString(ToStringParams(DWord9, "DWord9 / PRP2"));
			retStr += strings::toString(ToStringParams(DPTR2, "DPTR2 (DWords 8/9)"));
			return retStr;
		}

		std::string NVME_COMMAND::toString() const
		{
			std::string retStr;
			retStr += "NVMe Command:\n";
			retStr += strings::indentLines(DWord0Breakdown.toString());
			retStr += strings::toString(ToStringParams(DWord1, "Command DWord 1 / NSID"));
			retStr += strings::toString(ToStringParams(DWord2, "Command DWord 2 / Reserved"));
			retStr += strings::toString(ToStringParams(DWord3, "Command DWord 3 / Reserved"));
			retStr += strings::indentLines(MPTR.toString());
			retStr += strings::indentLines(strings::toString(CompleteMPTR, "MPTR", "Command DWord 4 / 5 / Metadata Pointer"));
			retStr += strings::indentLines(DPTR.toString());
			retStr += strings::toString(ToStringParams(DWord10, "Command DWord 10 / Command Specific"));
			retStr += strings::toString(ToStringParams(DWord11, "Command DWord 11 / Command Specific"));
			retStr += strings::toString(ToStringParams(DWord12, "Command DWord 12 / Command Specific"));
			retStr += strings::toString(ToStringParams(DWord13, "Command DWord 13 / Command Specific"));
			retStr += strings::toString(ToStringParams(DWord14, "Command DWord 14 / Command Specific"));
			retStr += strings::toString(ToStringParams(DWord15, "Command DWord 15 / Command Specific"));
			return retStr;
		}

		std::string COMPLETION_QUEUE_ENTRY::toString() const
		{
			std::string retStr;
			retStr += "Completion Queue Entry\n";
			retStr += strings::toString(ToStringParams(DWord0, "Command DWord 0 / NSID"));
			retStr += strings::toString(ToStringParams(DWord1, "Command DWord 1 / Reserved"));
			retStr += strings::toString(ToStringParams(DWord2, "Command DWord 2 / SQHD / SQID"));
			retStr += strings::indentLines(strings::toString(ToStringParams(SQHD, "Submission Queue Head Pointer")));
			retStr += strings::indentLines(strings::toString(ToStringParams(SQID, "Submission Queue Identifier")));
			retStr += strings::toString(ToStringParams(DWord3, "Command DWord 3 / CID / P / SF"));
			retStr += strings::indentLines(strings::toString(ToStringParams(CID, "Command Identifier")));
			retStr += strings::indentLines(strings::toString(ToStringParams(P, "Phase Tag")));
			retStr += strings::indentLines(strings::toString(ToStringParams(SC, "Status Code")));
			retStr += strings::indentLines(strings::toString(ToStringParams(SCT, "Status Code Type")));
			retStr += strings::indentLines(strings::toString(ToStringParams(M, "More")));
			retStr += strings::indentLines(strings::toString(ToStringParams(DNR, "DNR")));
			return retStr;
		}

	}
}