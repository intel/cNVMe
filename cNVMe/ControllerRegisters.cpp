/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
ControllerRegisters.cpp - An implementation file for the ControllerRegisters
*/

#include "ControllerRegisters.h"
#include "Strings.h"

namespace cnvme
{
	namespace controller
	{
		namespace registers
		{

			std::string CONTROLLER_CAPABILITIES::toString() const
			{
				std::string retStr;
				retStr += "Controller Capabilities (CAP / Offset 0x00):\n";
				retStr += strings::toString(ToStringParams(MQES, "Maximum Queue Entries Supported"));
				retStr += strings::toString(ToStringParams(CQR, "Contiguous Queues Required"));
				retStr += strings::toString(ToStringParams(AMS, "Arbitration Mechanism Supported"));
				retStr += strings::toString(ToStringParams(RSVD2, "Reserved"));
				retStr += strings::toString(ToStringParams(TO, "Timeout"));
				retStr += strings::toString(ToStringParams(DSTRD, "Doorbell Stride"));
				retStr += strings::toString(ToStringParams(NSSRS, "NVM Subsystem Reset Supported"));
				retStr += strings::toString(ToStringParams(CSS, "Command Sets Supported"));
				retStr += strings::toString(ToStringParams(RSVD1, "Reserved"));
				retStr += strings::toString(ToStringParams(MPSMIN, "Memory Page Size Minimum"));
				retStr += strings::toString(ToStringParams(MPSMAX, "Memory Page Size Maximum"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string Version::toString() const
			{
				std::string retStr;
				retStr += "Version (VS / Offset 0x08):\n";
				retStr += strings::toString(ToStringParams(TER, "Tertiary Version Number"));
				retStr += strings::toString(ToStringParams(MNR, "Minor Version Number"));
				retStr += strings::toString(ToStringParams(MJR, "Major Version Number"));
				return retStr;
			}

			std::string INTERRUPT_MASK_SET::toString() const
			{
				std::string retStr;
				retStr += "Interrupt Mask Set (INTMS / Offset 0x0C):\n";
				retStr += strings::toString(ToStringParams(IVMS, "Interrupt Vector Mask Set"));
				return retStr;
			}

			std::string INTERRUPT_MASK_CLEAR::toString() const
			{
				std::string retStr;
				retStr += "Interrupt Mask Clear (INTMC / Offset 0x10):\n";
				retStr += strings::toString(ToStringParams(IVMC, "Interrupt Vector Mask Clear"));
				return retStr;
			}

			std::string CONTROLLER_CONFIGURATION::toString() const
			{
				std::string retStr;
				retStr += "Controller Configuration (CC / Offset 0x14):\n";
				retStr += strings::toString(ToStringParams(EN, "Enable"));
				retStr += strings::toString(ToStringParams(RSVD1, "Reserved"));
				retStr += strings::toString(ToStringParams(CSS, "I/O Command Set Selected"));
				retStr += strings::toString(ToStringParams(MPS, "Memory Page Size"));
				retStr += strings::toString(ToStringParams(AMS, "Arbitration Mechanism Selected"));
				retStr += strings::toString(ToStringParams(SHN, "Shutdown Notification"));
				retStr += strings::toString(ToStringParams(IOSQES, "I/O Submission Queue Entry Size"));
				retStr += strings::toString(ToStringParams(IOCQES, "I/O Completion Queue Entry Size"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string CONTROLLER_STATUS::toString() const
			{
				std::string retStr;
				retStr += "Controller Status (CSTS / Offset 0x1C):\n";
				retStr += strings::toString(ToStringParams(RDY, "Ready"));
				retStr += strings::toString(ToStringParams(CFS, "Controller Fatal Status"));
				retStr += strings::toString(ToStringParams(SHST, "Shutdown Status"));
				retStr += strings::toString(ToStringParams(NSSRO, "NVM Subsystem Reset Occurred"));
				retStr += strings::toString(ToStringParams(PP, "Processing Paused"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string NVM_SUBSYSTEM_RESET::toString() const
			{
				std::string retStr;
				retStr += "NVM Subsystem Reset (NSSR / Offset 0x20):\n";
				retStr += strings::toString(ToStringParams(NSSRC, "NVM Subsystem Reset Control"));
				return retStr;
			}

			std::string ADMIN_QUEUE_ATTRIBUTES::toString() const
			{
				std::string retStr;
				retStr += "Admin Queue Attributes (AQA / Offset 0x24):\n";
				retStr += strings::toString(ToStringParams(ASQS, "Admin Submission Queue Size"));
				retStr += strings::toString(ToStringParams(RSVD1, "Reserved"));
				retStr += strings::toString(ToStringParams(ACQS, "Admin Completion Queue Size"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string ADMIN_SUBMISSION_QUEUE_BASE_ADDRESS::toString() const
			{
				std::string retStr;
				retStr += "Admin Submission Queue Base Address (ASQ / Offset 0x28):\n";
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				retStr += strings::toString(ToStringParams(ASQB, "Admin Submission Queue Base"));
				return retStr;
			}

			std::string ADMIN_COMPLETION_QUEUE_BASE_ADDRESS::toString() const
			{
				std::string retStr;
				retStr += "Admin Completion Queue Base Address (ACQ / Offset 0x30):\n";
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				retStr += strings::toString(ToStringParams(ACQB, "Admin Completion Queue Base"));
				return retStr;
			}

			std::string CONTROLLER_MEMORY_BUFFER_LOCATION::toString() const
			{
				std::string retStr;
				retStr += "Controller Memory Buffer Location (CMBLOC / Offset 0x38):\n";
				retStr += strings::toString(ToStringParams(BIR, "Base Indicator Register"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				retStr += strings::toString(ToStringParams(OFST, "Offset"));
				return retStr;
			}

			std::string CONTROLLER_MEMORY_BUFFER_SIZE::toString() const
			{
				std::string retStr;
				retStr += "Controller Memory Buffer Size (CMBSZ / Offset 0x3C):\n";
				retStr += strings::toString(ToStringParams(SQS, "Submission Queue Support"));
				retStr += strings::toString(ToStringParams(CQS, "Completion Queue Support"));
				retStr += strings::toString(ToStringParams(LISTS, "PRP SGL List Support"));
				retStr += strings::toString(ToStringParams(RDS, "Read Data Support"));
				retStr += strings::toString(ToStringParams(WDS, "Write Data Support"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				retStr += strings::toString(ToStringParams(SZU, "Size Units"));
				retStr += strings::toString(ToStringParams(SZ, "Size"));
				return retStr;
			}

			std::string SUBMISSION_QUEUE_Y_TAIL_DOORBELL::toString() const
			{
				std::string retStr;
				retStr += "Submission Queue Y Tail Doorbell (SQyTDBL / Offset (1000 + 0x((2y) * (4 << CAP.DSTRD)))):\n";
				retStr += strings::toString(ToStringParams(SQT, "Submission Queue Tail"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string COMPLETION_QUEUE_Y_HEAD_DOORBELL::toString() const
			{
				std::string retStr;
				retStr += "Completion Queue Y Head Doorbell (CQyHDBL / Offset (1000 + 0x((2y):\n";
				retStr += strings::toString(ToStringParams(CQH, "Completion Queue Head"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

		}
	}
}
