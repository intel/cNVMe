/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2017
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

	}
}