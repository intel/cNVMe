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
#include "Constants.h"
#include "Strings.h"

using namespace cnvme::constants::opcodes;
using namespace cnvme::constants::commands;

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

		UINT_64 NVME_COMMAND::getTransferSizeBytes(bool admin, UINT_32 sectorSizeInBytes) const
		{
			UINT_64 transferSize = 0;
			if (admin)
			{
				switch (this->DWord0Breakdown.OPC)
				{
				case admin::ABORT:
					break;
				case admin::ASYNCHRONOUS_EVENT_REQUEST:
					break;
				case admin::CREATE_IO_COMPLETION_QUEUE:
					transferSize = ONE_BASED_FROM_ZERO_BASED(this->DW10_CreateIoQueue.QSIZE) * sizeof(COMPLETION_QUEUE_ENTRY);
					break;
				case admin::CREATE_IO_SUBMISSION_QUEUE:
					transferSize = ONE_BASED_FROM_ZERO_BASED(this->DW10_CreateIoQueue.QSIZE) * sizeof(NVME_COMMAND);
					break;
				case admin::DELETE_IO_COMPLETION_QUEUE:
					break;
				case admin::DELETE_IO_SUBMISSION_QUEUE:
					break;
				case admin::DOORBELL_BUFFER_CONFIG:
					break;
				case admin::DEVICE_SELF_TEST:
					break;
				case admin::DIRECTIVE_RECEIVE:
					transferSize = ONE_BASED_FROM_ZERO_BASED(this->DWord10) * sizeof(UINT_32);
					break;
				case admin::DIRECTIVE_SEND:
					transferSize = ONE_BASED_FROM_ZERO_BASED(this->DWord10) * sizeof(UINT_32);
					break;
				case admin::FIRMWARE_COMMIT:
					break;
				case admin::FIRMWARE_IMAGE_DOWNLOAD:
					transferSize = ONE_BASED_FROM_ZERO_BASED(this->DWord10);
					break;
				case admin::GET_FEATURES:
					break; // technically this isn't always true with some optional pages
				case admin::GET_LOG_PAGE:
					transferSize = ((UINT_64)(this->DW11_GetLogPage.NUMDU) << 32) + this->DW10_GetLogPage.NUMDL;
					if (transferSize == ((UINT_64)-1) / sizeof(UINT_32))
					{
						LOG_INFO("Detected 64-bit overflow on NUMD!");
						transferSize = 0;
					}
					else
					{
						transferSize = ONE_BASED_FROM_ZERO_BASED(transferSize * sizeof(UINT_32));
					}
					break;
				case admin::IDENTIFY:
					transferSize = identify::sizes::IDENTIFY_SIZE;
					break;
				case admin::KEEP_ALIVE:
					break;
				case admin::NVME_MI_RECEIVE:
					break;
				case admin::NVME_MI_SEND:
					transferSize = ONE_BASED_FROM_ZERO_BASED(this->DWord11) * sizeof(UINT_32);
					break;
				case admin::NAMESPACE_ATTACHMENT:
					transferSize = ns_attachment::sizes::CONTROLLER_LIST_SIZE;
					break;
				case admin::NAMESPACE_MANAGEMENT:
					transferSize = identify::sizes::IDENTIFY_SIZE; // Uses an identify namespace structure;
					break;
				case admin::SET_FEATURES:
					break; // technically this isn't always true with some optional pages
				case admin::VIRTUALIZATION_MANAGEMENT:
					break;
				case admin::FORMAT_NVM:
					break;
				case admin::SANITIZE:
					break;
				case admin::SECURITY_SEND:
					transferSize = this->DWord11;
					break;
				case admin::SECURITY_RECEIVE:
					transferSize = this->DWord11;
					break;
				default:
					transferSize = (this->DWord10) * sizeof(UINT_32);
					LOG_INFO("Default case for Admin: transferSize = (DW10 + 1) * 4 = " + std::to_string(transferSize));
				}
			}
			else
			{
				switch (this->DWord0Breakdown.OPC)
				{
				case nvm::FLUSH:
					break;
				case nvm::WRITE:
					ASSERT_IF(sectorSizeInBytes < 512, "Invalid sector size to determine transfer size");
					transferSize = ONE_BASED_FROM_ZERO_BASED(this->DW12_IO.NLB) * sectorSizeInBytes;
					break; // 
				case nvm::READ:
					ASSERT_IF(sectorSizeInBytes < 512, "Invalid sector size to determine transfer size");
					transferSize = ONE_BASED_FROM_ZERO_BASED(this->DW12_IO.NLB) * sectorSizeInBytes;
					break; //
				case nvm::WRITE_UNCORRECTABLE:
					break;
				case nvm::COMPARE:
					ASSERT_IF(sectorSizeInBytes < 512, "Invalid sector size to determine transfer size");
					transferSize = ONE_BASED_FROM_ZERO_BASED(this->DW12_IO.NLB) * sectorSizeInBytes;
					break;
				case nvm::WRITE_ZEROES:
					break;
				case nvm::DATASET_MANAGEMENT:
					LOG_ERROR("Not supported cmd: DSM");
					break; //
				case nvm::RESERVATION_REGISTER:
					LOG_ERROR("Not supported cmd: RReg");
					break; //
				case nvm::RESERVATION_REPORT:
					LOG_ERROR("Not supported cmd: RRPT");
					break; //
				case nvm::RESERVATION_ACQUIRE:
					LOG_ERROR("Not supported cmd: RACQ");
					break; //
				case nvm::RESERVATION_RELEASE:
					LOG_ERROR("Not supported cmd: RRel");
					break; // 
				default:
					transferSize = ONE_BASED_FROM_ZERO_BASED(this->DWord10) * sizeof(UINT_32);
					LOG_INFO("Default case for NVM: transferSize = (DW10 + 1) * 4 = " + std::to_string(transferSize));
				}
			}
			return transferSize;
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

		bool COMPLETION_QUEUE_ENTRY::succeeded()
		{
			return ((this->SC | this->SCT) == 0);
		}
	}
}