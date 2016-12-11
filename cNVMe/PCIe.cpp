/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
PCIe.cpp - A implementation file for the PCIe Registers
*/

#include "PCIe.h"
#include "Strings.h"

/// <summary>
/// Used to get the params for strings::toString
/// </summary>
#define ToStringParams(abbreviation, description) \
	abbreviation, #abbreviation, description

#define BAR_SIZE 4096          // 4K Bytes
#define CAPABILITIES_SIZE 4096 // Much larger than probably needed

// Cap Ids
#define CID_PMCAP     0x01
#define CID_MSICAP    0x05
#define CID_MSIXCAP   0x11
#define CID_PXCAP     0x10

// Extended Cap Ids
#define CID_AERCAP    0x0001

using namespace cnvme::pci::capabilities;
using namespace cnvme::pci::header;

namespace cnvme
{
	namespace pci
	{
		namespace header
		{
			std::string PCI_IDENTIFIERS::toString() const
			{
				std::string retStr;
				retStr += "PCI Identifiers (ID / Offset 0x00):\n";
				retStr += strings::toString(ToStringParams(VID, "Vendor ID"));
				retStr += strings::toString(ToStringParams(DID, "Device ID"));
				return retStr;
			}

			std::string PCI_COMMAND::toString() const
			{
				std::string retStr;
				retStr += "PCI Command (CMD / Offset 0x04):\n";
				retStr += strings::toString(ToStringParams(IOSE, "I/O Space Enable"));
				retStr += strings::toString(ToStringParams(MSE, "Memory Space Enable"));
				retStr += strings::toString(ToStringParams(BME, "Bus Master Enable"));
				retStr += strings::toString(ToStringParams(SCE, "Special Cycle Enable"));
				retStr += strings::toString(ToStringParams(MWIE, "Memory Write and Invalidate Enable"));
				retStr += strings::toString(ToStringParams(VGA, "VGA Palette Snooping Enable"));
				retStr += strings::toString(ToStringParams(PEE, "Parity Error Response Enable"));
				retStr += strings::toString(ToStringParams(RSVD0, "Hardwired to 0"));
				retStr += strings::toString(ToStringParams(SEE, "SERR# Enable"));
				retStr += strings::toString(ToStringParams(FBE, "Fast Back-to-Back Enable"));
				retStr += strings::toString(ToStringParams(ID, "Interrupt Disable"));
				retStr += strings::toString(ToStringParams(RSVD1, "Reserved"));
				return retStr;
			}

			std::string PCI_DEVICE_STATUS::toString() const
			{
				std::string retStr;
				retStr += "PCI Device Status (STS / Offset 0x06):\n";
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				retStr += strings::toString(ToStringParams(IS, "Interrupt Status"));
				retStr += strings::toString(ToStringParams(CL, "Capabilities List"));
				retStr += strings::toString(ToStringParams(C66, "66 MHz Capable"));
				retStr += strings::toString(ToStringParams(RSVD1, "Reserved"));
				retStr += strings::toString(ToStringParams(FBC, "Fast Back-to-Back Capable"));
				retStr += strings::toString(ToStringParams(DPD, "Master Data Parity Error Detected"));
				retStr += strings::toString(ToStringParams(DEVT, "DEVSEL# Timing"));
				retStr += strings::toString(ToStringParams(STA, "Signaled Target-Abort"));
				retStr += strings::toString(ToStringParams(RTA, "Received Target-Abort"));
				retStr += strings::toString(ToStringParams(RMA, "Received Master-Abort"));
				retStr += strings::toString(ToStringParams(SSE, "Signaled System Error"));
				retStr += strings::toString(ToStringParams(DPE, "Detected Parity Error"));
				return retStr;
			}

			std::string PCI_REVISION_ID::toString() const
			{
				std::string retStr;
				retStr += "PCI Revision Id (RID / Offset 0x08):\n";
				retStr += strings::toString(ToStringParams(RID, "Revision ID"));
				return retStr;
			}

			std::string PCI_CLASS_CODE::toString() const
			{
				std::string retStr;
				retStr += "PCI Class Code (CC / Offset 0x09):\n";
				retStr += strings::toString(ToStringParams(PI, "Programming Interface"));
				retStr += strings::toString(ToStringParams(SCC, "Sub Class Code"));
				retStr += strings::toString(ToStringParams(BCC, "Base Class Code"));
				return retStr;
			}

			std::string PCI_CACHE_LINE_SIZE::toString() const
			{
				std::string retStr;
				retStr += "PCI Cache Line Size (CLS / Offset 0x0C):\n";
				retStr += strings::toString(ToStringParams(CLS, "Cache Line Size"));
				return retStr;
			}

			std::string PCI_MASTER_LATENCY_TIMER::toString() const
			{
				std::string retStr;
				retStr += "PCI Master Latency Timer (MLT / Offset 0x0D):\n";
				retStr += strings::toString(ToStringParams(MLT, "Master Latency Timer"));
				return retStr;
			}

			std::string PCI_HEADER_TYPE::toString() const
			{
				std::string retStr;
				retStr += "PCI Header Type (HTYPE / Offset 0x0E):\n";
				retStr += strings::toString(ToStringParams(HL, "Header Layout"));
				retStr += strings::toString(ToStringParams(MFD, "Multi-Function Device"));
				return retStr;
			}

			std::string PCI_BUILT_IN_SELF_TEST::toString() const
			{
				std::string retStr;
				retStr += "PCI Built In Self Test (BIST / Offset 0x0F):\n";
				retStr += strings::toString(ToStringParams(CC, "Completion Code"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				retStr += strings::toString(ToStringParams(SB, "Start BIST"));
				retStr += strings::toString(ToStringParams(BC, "BIST Capable"));
				return retStr;
			}

			std::string PCI_MEMORY_REGISTER_BASE_ADDRESS_LOWER_32::toString() const
			{
				std::string retStr;
				retStr += "PCI Memory Register Base Address Lower 32 (MLBAR / Offset 0x10):\n";
				retStr += strings::toString(ToStringParams(RTE, "Resource Type Indicator"));
				retStr += strings::toString(ToStringParams(TP, "Type"));
				retStr += strings::toString(ToStringParams(PF, "Prefetchable"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				retStr += strings::toString(ToStringParams(BA, "Base Address"));
				return retStr;
			}

			std::string PCI_MEMORY_REGISTER_BASE_ADDRESS_UPPER_32::toString() const
			{
				std::string retStr;
				retStr += "PCI Memory Register Base Address Upper 32 (MUBAR / Offset 0x14):\n";
				retStr += strings::toString(ToStringParams(BA, "Base Address"));
				return retStr;
			}

			std::string PCI_INDEX_DATA_PAIR_REGISTER_BASE_ADDRESS::toString() const
			{
				std::string retStr;
				retStr += "PCI Index Data Pair Register Base Address (IDBAR / Offset 0x18):\n";
				retStr += strings::toString(ToStringParams(RTE, "Resource Type Indicator"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				retStr += strings::toString(ToStringParams(BA, "Base Address"));
				return retStr;
			}

			std::string PCI_GENERIC_BAR::toString(int barNumber) const
			{
				std::string retStr;

				std::string hexOffset;
				std::string description;
				if (barNumber == 3)
				{
					hexOffset = "1C";
					description = "Reserved";
				}
				else if (barNumber == 4)
				{
					hexOffset = "20";
					description = "Vendor Specific";
				}
				else if (barNumber == 5)
				{
					hexOffset = "24";
					description = "Vendor Specific";
				}
				else
				{
					throw std::invalid_argument("Invalid barNumber given: " + std::to_string(barNumber));
				}

				retStr += "PCI " + description + " Base Address Register (BAR" + std::to_string(barNumber) + " / Offset 0x" + hexOffset + "):\n";
				retStr += strings::toString(ToStringParams(BAR, "Base Address Register"));
				return retStr;
			}

			std::string PCI_CARDBUS_CIS_POINTER_BIT_TYPE_RESET_DESCRIPTION::toString() const
			{
				std::string retStr;
				retStr += "PCI Cardbus Cis Pointer Bit Type Reset Description (CCPTR / Offset 0x28):\n";
				retStr += strings::toString(ToStringParams(RSVD0, "Not supported by NVM Express"));
				return retStr;
			}

			std::string PCI_SUB_SYSTEM_IDENTIFIERS::toString() const
			{
				std::string retStr;
				retStr += "PCI Sub System Identifiers (SS / Offset 0x2C):\n";
				retStr += strings::toString(ToStringParams(SSVID, "Subsystem Vendor ID"));
				retStr += strings::toString(ToStringParams(SSID, "Subsystem ID"));
				return retStr;
			}

			std::string PCI_EXPANSION_ROM::toString() const
			{
				std::string retStr;
				retStr += "PCI Expansion Rom (EROM / Offset 0x30):\n";
				retStr += strings::toString(ToStringParams(RBA, "ROM Base Address"));
				return retStr;
			}

			std::string PCI_CAPABILITIES_POINTER::toString() const
			{
				std::string retStr;
				retStr += "PCI Capabilities Pointer Bit Type Reset Description (CAP / Offset 0x34):\n";
				retStr += strings::toString(ToStringParams(CP, "Capability Pointer"));
				return retStr;
			}

			std::string PCI_INTERRUPT_INFORMATION::toString() const
			{
				std::string retStr;
				retStr += "PCI Interrupt Information (INTR / Offset 0x3C):\n";
				retStr += strings::toString(ToStringParams(ILINE, "Interrupt Line"));
				retStr += strings::toString(ToStringParams(IPIN, "Interrupt Pin"));
				return retStr;
			}

			std::string PCI_MINIMUM_GRANT::toString() const
			{
				std::string retStr;
				retStr += "PCI Minimum Grant (MGNT / Offset 0x3E):\n";
				retStr += strings::toString(ToStringParams(GNT, "Grant"));
				return retStr;
			}

			std::string PCI_MAXIMUM_LATENCY::toString() const
			{
				std::string retStr;
				retStr += "PCI Maximum Latency (MLAT / Offset 0x3F):\n";
				retStr += strings::toString(ToStringParams(LAT, "Latency"));
				return retStr;
			}

			std::string PCI_HEADER::toString() const
			{
				std::string retStr;
				retStr += "PCI Header:\n";
				retStr += strings::indentLines(ID.toString());
				retStr += strings::indentLines(CMD.toString());
				retStr += strings::indentLines(STS.toString());
				retStr += strings::indentLines(RID.toString());
				retStr += strings::indentLines(CC.toString());
				retStr += strings::indentLines(CLS.toString());
				retStr += strings::indentLines(MLT.toString());
				retStr += strings::indentLines(HTYPE.toString());
				retStr += strings::indentLines(BIST.toString());
				retStr += strings::indentLines(MLBAR.toString());
				retStr += strings::indentLines(MUBAR.toString());
				retStr += strings::indentLines(IDBAR.toString());
				retStr += strings::indentLines(BAR3.toString(3));
				retStr += strings::indentLines(BAR4.toString(4));
				retStr += strings::indentLines(BAR5.toString(5));
				retStr += strings::indentLines(CCPTR.toString());
				retStr += strings::indentLines(SS.toString());
				retStr += strings::indentLines(EROM.toString());
				retStr += strings::indentLines(CAP.toString());
				retStr += strings::indentLines(INTR.toString());
				retStr += strings::indentLines(MGNT.toString());
				retStr += strings::indentLines(MLAT.toString());
				return retStr;
			}
		}

		namespace capabilities
		{
			std::string PCI_CAPABILITY_ID::toString() const
			{
				std::string retStr;
				retStr += "PCI Power Management Capability Id (PID / Offset _CAP):\n";
				retStr += strings::toString(ToStringParams(CID, "Cap ID"));
				retStr += strings::toString(ToStringParams(NEXT, "Next Capability"));
				return retStr;
			}

			std::string PCI_POWER_MANAGEMENT::toString() const
			{
				std::string retStr;
				retStr += "PCI Power Management Capabilities (PC / Offset PMCAP + 2):\n";
				retStr += strings::toString(ToStringParams(VS, "Version"));
				retStr += strings::toString(ToStringParams(PMEC, "PME Clock"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				retStr += strings::toString(ToStringParams(DSI, "Device Specific Initialization"));
				retStr += strings::toString(ToStringParams(AUXC, "Aux Current"));
				retStr += strings::toString(ToStringParams(D1S, "D1 Support"));
				retStr += strings::toString(ToStringParams(D2S, "D2 Support"));
				retStr += strings::toString(ToStringParams(PSUP, "PME Support"));
				return retStr;
			}

			std::string PCI_POWER_MANAGEMENT_CONTROL_AND_STATUS::toString() const
			{
				std::string retStr;
				retStr += "PCI Power Management Control And Status (PMCS / Offset PMCAP + 4):\n";
				retStr += strings::toString(ToStringParams(PS, "Power State"));
				retStr += strings::toString(ToStringParams(RSVD1, "Reserved"));
				retStr += strings::toString(ToStringParams(NSFRST, "No Soft Reset"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				retStr += strings::toString(ToStringParams(PMEE, "PME Enable"));
				retStr += strings::toString(ToStringParams(DSE, "Data Select"));
				retStr += strings::toString(ToStringParams(DSC, "Data Scale"));
				retStr += strings::toString(ToStringParams(PMES, "PME Status"));
				return retStr;
			}

			std::string PCI_POWER_MANAGEMENT_CAPABILITIES::toString() const
			{
				std::string retStr;
				retStr += "PCI Power Management Capabilities (PMCAP)";
				retStr += strings::indentLines(PID.toString());
				retStr += strings::indentLines(PC.toString());
				retStr += strings::indentLines(PMCS.toString());
				return retStr;
			}

			std::string PCI_MESSAGE_SIGNALED_INTERRUPT_MESSAGE_CONTROL::toString() const
			{
				std::string retStr;
				retStr += "PCI Message Signaled Interrupt Message Control (MC / Offset MSICAP  + 0x2):\n";
				retStr += strings::toString(ToStringParams(MSIE, "MSI Enable"));
				retStr += strings::toString(ToStringParams(MMC, "Multiple Message Capable"));
				retStr += strings::toString(ToStringParams(MME, "Multiple Message Enable"));
				retStr += strings::toString(ToStringParams(C64, "64 Bit Address Capable"));
				retStr += strings::toString(ToStringParams(PVM, "Per-Vector Masking Capable"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string PCI_MESSAGE_SIGNALED_INTERRUPT_MESSAGE_ADDRESS::toString() const
			{
				std::string retStr;
				retStr += "PCI Message Signaled Interrupt Message Address (MA / Offset MSICAP  + 0x4):\n";
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				retStr += strings::toString(ToStringParams(ADDR, "Address"));
				return retStr;
			}

			std::string PCI_MESSAGE_SIGNALED_INTERRUPT_UPPER_ADDRESS::toString() const
			{
				std::string retStr;
				retStr += "PCI Message Signaled Interrupt Upper Address (MUA / Offset MSICAP  + 0x8):\n";
				retStr += strings::toString(ToStringParams(UADDR, "Upper Address"));
				return retStr;
			}

			std::string PCI_MESSAGE_SIGNALED_INTERRUPT_MESSAGE_DATA::toString() const
			{
				std::string retStr;
				retStr += "PCI Message Signaled Interrupt Message Data (MD / Offset MSICAP  + 0x0C):\n";
				retStr += strings::toString(ToStringParams(DATA, "Data"));
				return retStr;
			}

			std::string PCI_MESSAGE_SIGNALED_INTERRUPT_MASK_BITS::toString() const
			{
				std::string retStr;
				retStr += "PCI Message Signaled Interrupt Mask Bits (MMASK / Offset MSICAP  + 0x10):\n";
				retStr += strings::toString(ToStringParams(MASK, "Mask Bits"));
				return retStr;
			}

			std::string PCI_MESSAGE_SIGNALED_INTERRUPT_PENDING_BITS::toString() const
			{
				std::string retStr;
				retStr += "PCI Message Signaled Interrupt Pending Bits (MPEND / Offset MSICAP  + 0x14):\n";
				retStr += strings::toString(ToStringParams(PEND, "Pending Bits"));
				return retStr;
			}

			std::string PCI_MESSAGE_SIGNALED_INTERRUPT_CAPABILITY::toString() const
			{
				std::string retStr;
				retStr += "PCI Message Signaled Interrupt Capability (MSICAP):\n";
				retStr += strings::indentLines(MID.toString());
				retStr += strings::indentLines(MC.toString());
				retStr += strings::indentLines(MA.toString());
				retStr += strings::indentLines(MUA.toString());
				retStr += strings::indentLines(MD.toString());
				retStr += strings::indentLines(MMASK.toString());
				retStr += strings::indentLines(MPEND.toString());
				return retStr;
			}

			std::string PCI_MSI_X_MESSAGE_CONTROL::toString() const
			{
				std::string retStr;
				retStr += "PCI Msi X Message Control (MXC / Offset MSIXCAP + 0x2):\n";
				retStr += strings::toString(ToStringParams(TS, "Table Size"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				retStr += strings::toString(ToStringParams(FM, "Function Mask"));
				retStr += strings::toString(ToStringParams(MXE, "MSI-X Enable"));
				return retStr;
			}

			std::string PCI_MSI_X_TABLE_OFFSET_TABLE_BIR::toString() const
			{
				std::string retStr;
				retStr += "PCI Msi X Table Offset Table Bir (MTAB / Offset MSIXCAP + 0x4):\n";
				retStr += strings::toString(ToStringParams(TBIR, "Table BIR"));
				retStr += strings::toString(ToStringParams(TO, "Table Offset"));
				return retStr;
			}

			std::string PCI_MSI_X_PBA_OFFSET_PBA_BIR::toString() const
			{
				std::string retStr;
				retStr += "PCI Msi X Pba Offset Pba Bir (MPBA / Offset MSIXCAP + 0x8):\n";
				retStr += strings::toString(ToStringParams(PBIR, "PBA BIR"));
				retStr += strings::toString(ToStringParams(PBAO, "PBA Offset"));
				return retStr;
			}

			std::string PCI_MESSAGE_SIGNALED_INTERRUPT_X_CAPABILITY::toString() const
			{
				std::string retStr;
				retStr += "PCI Message Signaled Interrupt X Capability (MSIXCAP):\n";
				retStr += strings::indentLines(MXID.toString());
				retStr += strings::indentLines(MXC.toString());
				retStr += strings::indentLines(MTAB.toString());
				retStr += strings::indentLines(MPBA.toString());
				return retStr;
			}

			std::string PCI_EXPRESS_CAPABILITIES::toString() const
			{
				std::string retStr;
				retStr += "PCI Express Capabilities (PXCAP / Offset PXCAP + 0x2):\n";
				retStr += strings::toString(ToStringParams(VER, "Capability Version"));
				retStr += strings::toString(ToStringParams(DPT, "Device/Port Type"));
				retStr += strings::toString(ToStringParams(SI, "Slot Implemented"));
				retStr += strings::toString(ToStringParams(IMN, "Interrupt Message Number"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string PCI_EXPRESS_DEVICE_CAPABILITIES::toString() const
			{
				std::string retStr;
				retStr += "PCI Express Device Capabilities (PXDCAP / Offset PXCAP + 0x4):\n";
				retStr += strings::toString(ToStringParams(MPS, "Max Payload Size Supported"));
				retStr += strings::toString(ToStringParams(PFS, "Phantom Functions Supported"));
				retStr += strings::toString(ToStringParams(ETFS, "Extended Tag Field Supported"));
				retStr += strings::toString(ToStringParams(L0SL, "Endpoint L0s Acceptable Latency"));
				retStr += strings::toString(ToStringParams(L1L, "Endpoint L1 Acceptable Latency"));
				retStr += strings::toString(ToStringParams(RSVD2, "Reserved"));
				retStr += strings::toString(ToStringParams(RER, "Role-based Error Reporting"));
				retStr += strings::toString(ToStringParams(RSVD1, "Reserved"));
				retStr += strings::toString(ToStringParams(CSPLV, "Captured Slot Power Limit Value"));
				retStr += strings::toString(ToStringParams(CSPLS, "Captured Slot Power Limit Scale"));
				retStr += strings::toString(ToStringParams(FLRC, "Function Level Reset Capability"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string PCI_EXPRESS_DEVICE_CONTROL::toString() const
			{
				std::string retStr;
				retStr += "PCI Express Device Control (PXDC / Offset PXCAP + 0x8):\n";
				retStr += strings::toString(ToStringParams(CERE, "Correctable Error Reporting Enable"));
				retStr += strings::toString(ToStringParams(NFERE, "Non-Fatal Error Reporting Enable"));
				retStr += strings::toString(ToStringParams(FERE, "Fatal Error Reporting Enable"));
				retStr += strings::toString(ToStringParams(URRE, "Unsupported Request Reporting Enable"));
				retStr += strings::toString(ToStringParams(ERO, "Enable Relaxed Ordering"));
				retStr += strings::toString(ToStringParams(MPS, "Max Payload Size"));
				retStr += strings::toString(ToStringParams(ETE, "Extended Tag Enable"));
				retStr += strings::toString(ToStringParams(PFE, "Phantom Functions Enable"));
				retStr += strings::toString(ToStringParams(APPME, "AUX Power PM Enable"));
				retStr += strings::toString(ToStringParams(ENS, "Enable No Snoop"));
				retStr += strings::toString(ToStringParams(MRRS, "Max Read Request Size"));
				retStr += strings::toString(ToStringParams(IFLR, "Initiate Function Level Reset"));
				return retStr;
			}

			std::string PCI_EXPRESS_DEVICE_STATUS::toString() const
			{
				std::string retStr;
				retStr += "PCI Express Device Status (PXDS / Offset PXCAP + 0xA):\n";
				retStr += strings::toString(ToStringParams(CED, "Correctable Error Detected"));
				retStr += strings::toString(ToStringParams(NFED, "Non-Fatal Error Detected"));
				retStr += strings::toString(ToStringParams(FED, "Fatal Error Detected"));
				retStr += strings::toString(ToStringParams(URD, "Unsupported Request Detected"));
				retStr += strings::toString(ToStringParams(APD, "AUX Power Detected"));
				retStr += strings::toString(ToStringParams(TP, "Transactions Pending"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string PCI_EXPRESS_LINK_CAPABILITIES::toString() const
			{
				std::string retStr;
				retStr += "PCI Express Link Capabilities (PXLCAP / Offset PXCAP + 0xC):\n";
				retStr += strings::toString(ToStringParams(SLS, "Supported Link Speeds"));
				retStr += strings::toString(ToStringParams(MLW, "Maximum Link Width"));
				retStr += strings::toString(ToStringParams(ASPMS, "Active State Power Management Support"));
				retStr += strings::toString(ToStringParams(L0SEL, "L0s Exit Latency"));
				retStr += strings::toString(ToStringParams(L1EL, "L1 Exit Latency"));
				retStr += strings::toString(ToStringParams(CPM, "Clock Power Management"));
				retStr += strings::toString(ToStringParams(SDERC, "Surprise Down Error Reporting Capable"));
				retStr += strings::toString(ToStringParams(DLLLA, "Data Link Layer Link Active Reporting Capable"));
				retStr += strings::toString(ToStringParams(LBNC, "Link Bandwidth Notification Capability"));
				retStr += strings::toString(ToStringParams(AOC, "ASPM Optionality Compliance"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				retStr += strings::toString(ToStringParams(PN, "Port Number"));
				return retStr;
			}

			std::string PCI_EXPRESS_LINK_CONTROL::toString() const
			{
				std::string retStr;
				retStr += "PCI Express Link Control (PXLC / Offset PXCAP + 0x10):\n";
				retStr += strings::toString(ToStringParams(ASPMC, "Active State Power Management Control"));
				retStr += strings::toString(ToStringParams(RSVD2, "Reserved"));
				retStr += strings::toString(ToStringParams(RCB, "Read Completion Boundary"));
				retStr += strings::toString(ToStringParams(RSVD1, "Reserved: These bits are reserved on Endpoints"));
				retStr += strings::toString(ToStringParams(CCC, "Common Clock Configuration"));
				retStr += strings::toString(ToStringParams(ES, "Extended Synch"));
				retStr += strings::toString(ToStringParams(ECPM, "Enable Clock Power Management"));
				retStr += strings::toString(ToStringParams(HAWD, "Hardware Autonomous Width Disable"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string PCI_EXPRESS_LINK_STATUS::toString() const
			{
				std::string retStr;
				retStr += "PCI Express Link Status (PXLS / Offset PXCAP + 0x12):\n";
				retStr += strings::toString(ToStringParams(CLS, "Current Link Speed"));
				retStr += strings::toString(ToStringParams(NLW, "Negotiated Link Width"));
				retStr += strings::toString(ToStringParams(RSVD1, "Reserved"));
				retStr += strings::toString(ToStringParams(SCC, "Slot Clock Configuration"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string PCI_EXPRESS_DEVICE_CAPABILITIES_2::toString() const
			{
				std::string retStr;
				retStr += "PCI Express Device Capabilities 2 (PXDCAP2 / Offset PXCAP + 0x24):\n";
				retStr += strings::toString(ToStringParams(CTRS, "Completion Timeout Ranges Supported"));
				retStr += strings::toString(ToStringParams(CTDS, "Completion Timeout Disable Supported"));
				retStr += strings::toString(ToStringParams(ARIFS, "ARI Forwarding Supported"));
				retStr += strings::toString(ToStringParams(AORS, "AtomicOp Routing Supported"));
				retStr += strings::toString(ToStringParams(AOCS32, "32-bit AtomicOp Completer Supported"));
				retStr += strings::toString(ToStringParams(AOCS64, "64-bit AtomicOp Completer Supported"));
				retStr += strings::toString(ToStringParams(CCS128, "128-bit CAS Completer Supported"));
				retStr += strings::toString(ToStringParams(NPRPR, "No RO-enabled PR-PR Passing"));
				retStr += strings::toString(ToStringParams(LTRS, "Latency Tolerance Reporting Supported"));
				retStr += strings::toString(ToStringParams(TPHCS, "TPH Completer Supported"));
				retStr += strings::toString(ToStringParams(RSVD1, "Reserved"));
				retStr += strings::toString(ToStringParams(OBFFS, "OBFF Supported"));
				retStr += strings::toString(ToStringParams(EFFS, "Extended Fmt Field Supported"));
				retStr += strings::toString(ToStringParams(EETPS, "End-End TLP Prefix Supported"));
				retStr += strings::toString(ToStringParams(MEETP, "Max End-End TLP Prefixes"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string PCI_EXPRESS_DEVICE_CONTROL_2::toString() const
			{
				std::string retStr;
				retStr += "PCI Express Device Control 2 (PXDC2 / Offset PXCAP + 0x28):\n";
				retStr += strings::toString(ToStringParams(RSVD3, "Completion Timeout Value:"));
				retStr += strings::toString(ToStringParams(CTD, "Completion Timeout Disable"));
				retStr += strings::toString(ToStringParams(RSVD2, "Reserved"));
				retStr += strings::toString(ToStringParams(LTRME, "Latency Tolerance Reporting Mechanism Enable"));
				retStr += strings::toString(ToStringParams(RSVD1, "Reserved"));
				retStr += strings::toString(ToStringParams(OBFFE, "OBFF Enable"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string PCI_EXPRESS_CAPABILITY::toString() const
			{
				std::string retStr;
				retStr += "PCI Express Capability (PXCAP):\n";
				retStr += strings::indentLines(PXID.toString());
				retStr += strings::indentLines(PXCAP.toString());
				retStr += strings::indentLines(PXDCAP.toString());
				retStr += strings::indentLines(PXDC.toString());
				retStr += strings::indentLines(PXDS.toString());
				retStr += strings::indentLines(PXLCAP.toString());
				retStr += strings::indentLines(PXLC.toString());
				retStr += strings::indentLines(PXLS.toString());
				retStr += strings::indentLines(PXDCAP2.toString());
				retStr += strings::indentLines(PXDC2.toString());
				return retStr;
			}

			std::string PCI_AER_CAPABILITY_ID::toString() const
			{
				std::string retStr;
				retStr += "PCI Aer Capability Id (AERID / Offset AERCAP):\n";
				retStr += strings::toString(ToStringParams(CID, "Capability ID"));
				retStr += strings::toString(ToStringParams(CVER, "Capability Version"));
				retStr += strings::toString(ToStringParams(NEXT, "Next Pointer"));
				return retStr;
			}

			std::string PCI_AER_UNCORRECTABLE_ERROR_STATUS_REGISTER::toString() const
			{
				std::string retStr;
				retStr += "PCI Aer Uncorrectable Error Status Register (AERUCES / Offset AERCAP + 0x4):\n";
				retStr += strings::toString(ToStringParams(RSVD2, "Reserved"));
				retStr += strings::toString(ToStringParams(DLPES, "Data Link Protocol Error Status"));
				retStr += strings::toString(ToStringParams(RSVD1, "Reserved"));
				retStr += strings::toString(ToStringParams(PTS, "Poisoned TLP Status"));
				retStr += strings::toString(ToStringParams(FCPES, "Flow Control Protocol Error Status"));
				retStr += strings::toString(ToStringParams(CTS, "Completion Timeout Status"));
				retStr += strings::toString(ToStringParams(CAS, "Completer Abort Status"));
				retStr += strings::toString(ToStringParams(UCS, "Unexpected Completion Status"));
				retStr += strings::toString(ToStringParams(ROS, "Receiver Overflow Status"));
				retStr += strings::toString(ToStringParams(MTS, "Malformed TLP Status"));
				retStr += strings::toString(ToStringParams(ECRCES, "ECRC Error Status"));
				retStr += strings::toString(ToStringParams(URES, "Unsupported Request Error Status"));
				retStr += strings::toString(ToStringParams(ACSVS, "ACS Violation Status"));
				retStr += strings::toString(ToStringParams(UIES, "Uncorrectable Internal Error Status"));
				retStr += strings::toString(ToStringParams(MCBTS, "MC Blocked TLP Status"));
				retStr += strings::toString(ToStringParams(AOEBS, "AtomicOp Egress Blocked Status"));
				retStr += strings::toString(ToStringParams(TPBES, "TLP Prefix Blocked Error Status"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string PCI_AER_UNCORRECTABLE_ERROR_MASK_REGISTER::toString() const
			{
				std::string retStr;
				retStr += "PCI Aer Uncorrectable Error Mask Register (AERUCEM / Offset AERCAP + 0x8):\n";
				retStr += strings::toString(ToStringParams(RSVD2, "Reserved"));
				retStr += strings::toString(ToStringParams(DLPEM, "Data Link Protocol Error Mask"));
				retStr += strings::toString(ToStringParams(RSVD1, "Reserved"));
				retStr += strings::toString(ToStringParams(PTM, "Poisoned TLP Mask"));
				retStr += strings::toString(ToStringParams(FCPEM, "Flow Control Protocol Error Mask"));
				retStr += strings::toString(ToStringParams(CTM, "Completion Timeout Mask"));
				retStr += strings::toString(ToStringParams(CAM, "Completer Abort Mask"));
				retStr += strings::toString(ToStringParams(UCM, "Unexpected Completion Mask"));
				retStr += strings::toString(ToStringParams(ROM, "Receiver Overflow Mask"));
				retStr += strings::toString(ToStringParams(MTM, "Malformed TLP Mask"));
				retStr += strings::toString(ToStringParams(ECRCEM, "ECRC Error Mask"));
				retStr += strings::toString(ToStringParams(UREM, "Unsupported Request Error Mask"));
				retStr += strings::toString(ToStringParams(ACSVM, "ACS Violation Mask"));
				retStr += strings::toString(ToStringParams(UIEM, "Uncorrectable Internal Error Mask"));
				retStr += strings::toString(ToStringParams(MCBTM, "MC Blocked TLP Mask"));
				retStr += strings::toString(ToStringParams(AOEBM, "AtomicOp Egress Blocked Mask"));
				retStr += strings::toString(ToStringParams(TPBEM, "TLP Prefix Blocked Error Mask"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string PCI_AER_UNCORRECTABLE_ERROR_SEVERITY_REGISTER::toString() const
			{
				std::string retStr;
				retStr += "PCI Aer Uncorrectable Error Severity Register (AERUCESEV / Offset AERCAP + 0xC):\n";
				retStr += strings::toString(ToStringParams(RSVD2, "Reserved"));
				retStr += strings::toString(ToStringParams(DLPESEV, "Data Link Protocol Error Severity"));
				retStr += strings::toString(ToStringParams(RSVD1, "Reserved"));
				retStr += strings::toString(ToStringParams(PTSEV, "Poisoned TLP Severity"));
				retStr += strings::toString(ToStringParams(FCPESEV, "Flow Control Protocol Error Severity"));
				retStr += strings::toString(ToStringParams(CTSEV, "Completion Timeout Severity"));
				retStr += strings::toString(ToStringParams(CASEV, "Completer Abort Severity"));
				retStr += strings::toString(ToStringParams(UCSEV, "Unexpected Completion Severity"));
				retStr += strings::toString(ToStringParams(ROSEV, "Receiver Overflow Severity"));
				retStr += strings::toString(ToStringParams(MTSEV, "Malformed TLP Severity"));
				retStr += strings::toString(ToStringParams(ECRCESEV, "ECRC Error Severity"));
				retStr += strings::toString(ToStringParams(URESEV, "Unsupported Request Error Severity"));
				retStr += strings::toString(ToStringParams(ACSVSEV, "ACS Violation Severity"));
				retStr += strings::toString(ToStringParams(UIESEV, "Uncorrectable Internal Error Severity"));
				retStr += strings::toString(ToStringParams(MCBTSEV, "MC Blocked TLP Severity"));
				retStr += strings::toString(ToStringParams(AOEBSEV, "AtomicOp Egress Blocked Severity"));
				retStr += strings::toString(ToStringParams(TPBESEV, "TLP Prefix Blocked Error Severity"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string PCI_AER_CORRECTABLE_ERROR_STATUS_REGISTER::toString() const
			{
				std::string retStr;
				retStr += "PCI Aer Correctable Error Status Register (AERCS / Offset AERCAP + 0x10):\n";
				retStr += strings::toString(ToStringParams(RES, "Receiver Error Status"));
				retStr += strings::toString(ToStringParams(RSVD2, "Reserved"));
				retStr += strings::toString(ToStringParams(BTS, "Bad TLP Status"));
				retStr += strings::toString(ToStringParams(BDS, "Bad DLLP Status"));
				retStr += strings::toString(ToStringParams(RRS, "REPLAY NUM Rollover Status"));
				retStr += strings::toString(ToStringParams(RSVD1, "Reserved"));
				retStr += strings::toString(ToStringParams(RTS, "Replay Timer Timeout Status"));
				retStr += strings::toString(ToStringParams(ANFES, "Advisory Non-Fatal Error Status"));
				retStr += strings::toString(ToStringParams(CIES, "Corrected Internal Error Status"));
				retStr += strings::toString(ToStringParams(HLOS, "Header Log Overflow Status"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string PCI_AER_CORRECTABLE_ERROR_MASK_REGISTER::toString() const
			{
				std::string retStr;
				retStr += "PCI Aer Correctable Error Mask Register (AERCEM / Offset AERCAP + 0x14):\n";
				retStr += strings::toString(ToStringParams(REM, "Receiver Error Mask"));
				retStr += strings::toString(ToStringParams(RSVD3, "Reserved"));
				retStr += strings::toString(ToStringParams(BTM, "Bad TLP Mask"));
				retStr += strings::toString(ToStringParams(BDM, "Bad DLLP Mask"));
				retStr += strings::toString(ToStringParams(RRM, "REPLAY NUM Rollover Mask"));
				retStr += strings::toString(ToStringParams(RSVD2, "Reserved"));
				retStr += strings::toString(ToStringParams(RTM, "Replay Timer Timeout Mask"));
				retStr += strings::toString(ToStringParams(RSVD1, "Advisory Non-Fatal Error Mask ANFEM)"));
				retStr += strings::toString(ToStringParams(CIEM, "Corrected Internal Error Mask"));
				retStr += strings::toString(ToStringParams(HLOM, "Header Log Overflow Mask"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string PCI_AER_CAPABILITIES_AND_CONTROL_REGISTER::toString() const
			{
				std::string retStr;
				retStr += "PCI Aer Capabilities And Control Register (AERCC / Offset AERCAP + 0x18):\n";
				retStr += strings::toString(ToStringParams(FEP, "First Error Pointer"));
				retStr += strings::toString(ToStringParams(EGC, "ECRC Generation Capable"));
				retStr += strings::toString(ToStringParams(EGE, "ECRC Generation Enable"));
				retStr += strings::toString(ToStringParams(ECC, "ECRC Check Capable"));
				retStr += strings::toString(ToStringParams(ECE, "ECRC Check Enable"));
				retStr += strings::toString(ToStringParams(MHRC, "Multiple Header Recording Capable"));
				retStr += strings::toString(ToStringParams(MHRE, "Multiple Header Recording Enable"));
				retStr += strings::toString(ToStringParams(TPLP, "TLP Prefix Log Present"));
				retStr += strings::toString(ToStringParams(RSVD0, "Reserved"));
				return retStr;
			}

			std::string PCI_AER_HEADER_LOG_REGISTER::toString() const
			{
				std::string retStr;
				retStr += "PCI Aer Header Log Register (AERHL / Offset AERCAP + 0x1C):\n";
				retStr += strings::toString(ToStringParams(HB12, "Header Byte 12"));
				retStr += strings::toString(ToStringParams(HB13, "Header Byte 13"));
				retStr += strings::toString(ToStringParams(HB14, "Header Byte 14"));
				retStr += strings::toString(ToStringParams(HB15, "Header Byte 15"));
				retStr += strings::toString(ToStringParams(HB8, "Header Byte 8"));
				retStr += strings::toString(ToStringParams(HB9, "Header Byte 9"));
				retStr += strings::toString(ToStringParams(HB10, "Header Byte 10"));
				retStr += strings::toString(ToStringParams(HB11, "Header Byte 11"));
				retStr += strings::toString(ToStringParams(HB4, "Header Byte 4"));
				retStr += strings::toString(ToStringParams(HB5, "Header Byte 5"));
				retStr += strings::toString(ToStringParams(HB6, "Header Byte 6"));
				retStr += strings::toString(ToStringParams(HB7, "Header Byte 7"));
				retStr += strings::toString(ToStringParams(HB0, "Header Byte 0"));
				retStr += strings::toString(ToStringParams(HB1, "Header Byte 1"));
				retStr += strings::toString(ToStringParams(HB2, "Header Byte 2"));
				retStr += strings::toString(ToStringParams(HB3, "Header Byte 3"));
				return retStr;
			}

			std::string PCI_AER_TLP_PREFIX_LOG_REGISTER::toString() const
			{
				std::string retStr;
				retStr += "PCI Aer Tlp Prefix Log Register (AERTLP / Offset AERCAP + 0x38):\n";
				retStr += strings::toString(ToStringParams(TPL4B0, "Fourth TLP Prefix Log Byte 0"));
				retStr += strings::toString(ToStringParams(TPL4B1, "Fourth TLP Prefix Log Byte 1"));
				retStr += strings::toString(ToStringParams(TPL4B2, "Fourth TLP Prefix Log Byte 2"));
				retStr += strings::toString(ToStringParams(TPL4B3, "Fourth TLP Prefix Log Byte 3"));
				retStr += strings::toString(ToStringParams(TPL3B0, "Third TLP Prefix Log Byte 0"));
				retStr += strings::toString(ToStringParams(TPL3B1, "Third TLP Prefix Log Byte 1"));
				retStr += strings::toString(ToStringParams(TPL3B2, "Third TLP Prefix Log Byte 2"));
				retStr += strings::toString(ToStringParams(TPL3B3, "Third TLP Prefix Log Byte 3"));
				retStr += strings::toString(ToStringParams(TPL2B0, "Second TLP Prefix Log Byte 0"));
				retStr += strings::toString(ToStringParams(TPL2B1, "Second TLP Prefix Log Byte 1"));
				retStr += strings::toString(ToStringParams(TPL2B2, "Second TLP Prefix Log Byte 2"));
				retStr += strings::toString(ToStringParams(TPL2B3, "Second TLP Prefix Log Byte 3"));
				retStr += strings::toString(ToStringParams(TPL1B0, "First TLP Prefix Log Byte 0"));
				retStr += strings::toString(ToStringParams(TPL1B1, "First TLP Prefix Log Byte 1"));
				retStr += strings::toString(ToStringParams(TPL1B2, "First TLP Prefix Log Byte 2"));
				retStr += strings::toString(ToStringParams(TPL1B3, "First TLP Prefix Log Byte 3"));
				return retStr;
			}

			std::string PCI_ADVANCED_ERROR_REPORTING_CAPABILITY::toString() const
			{
				std::string retStr;
				retStr += "PCI Advanced Error Reporting Extended Capability (PXCAP):\n";
				retStr += strings::indentLines(AERID.toString());
				retStr += strings::indentLines(AERUCES.toString());
				retStr += strings::indentLines(AERUCEM.toString());
				retStr += strings::indentLines(AERUCESEV.toString());
				retStr += strings::indentLines(AERCS.toString());
				retStr += strings::indentLines(AERCEM.toString());
				retStr += strings::indentLines(AERCC.toString());
				retStr += strings::indentLines(AERHL.toString());
				retStr += strings::indentLines(AERTLP.toString());
				return retStr;
			}
		}

		std::string PCI_EXPRESS_REGISTERS::toString() const
		{
			std::string retStr;
			retStr += "PCI Express Registers:\n";
			if (PciHeader)
			{
				retStr += strings::indentLines(PciHeader->toString());
			}

			if (PMCAP)
			{
				retStr += strings::indentLines(PMCAP->toString());
			}

			if (MSICAP)
			{
				retStr += strings::indentLines(MSICAP->toString());
			}

			if (MSIXCAP)
			{
				retStr += strings::indentLines(MSIXCAP->toString());
			}

			if (PXCAP)
			{
				retStr += strings::indentLines(PXCAP->toString());
			}

			if (AERCAP)
			{
				retStr += strings::indentLines(AERCAP->toString());
			}

			return retStr;
		}

		PCIExpressRegisters::PCIExpressRegisters()
		{
			resetPciHeader();
#ifdef SINGLE_THREADED
			listeningThreadBool = false;
#else
			listeningThreadBool = true;
			listeningThread = std::thread(&PCIExpressRegisters::listenForChanges, this);
#endif
		}

		void PCIExpressRegisters::resetPciHeader()
		{
			PciHeaderAndCapabilities = cnvme::Payload(sizeof(PCI_HEADER) + CAPABILITIES_SIZE);
			PCI_HEADER* PciHeader = getPciHeader();

			PciHeader->ID.DID = 0xCCAA;
			PciHeader->ID.VID = 0xAACC;

			PciHeader->CMD.MSE = 1;  // Allow controller register memory space access
			PciHeader->CMD.IOSE = 1; // Allow access to controller's target I/O space

			PciHeader->STS.CL = 1;   // Device has a capabilities list

			PciHeader->RID.RID = 1;  // This is stepping 1 of this 'hardware'

			PciHeader->CC.BCC = 1; // Mass storage controller
			PciHeader->CC.SCC = 8; // Non-Volatile Memory controller
			PciHeader->CC.PI = 2;  // NVM Express

			// Allocate BARs
			allocateBars();

			PciHeader->SS.SSID = 0xFFEE; // Subsystem ID
			PciHeader->SS.SSVID = 0xEEFF; // Subsystem Vendor ID

			// Allocate Capabilities
			allocateCapabilities();

			// Allocate Extended Capabilities
			// TODO
		}

		void PCIExpressRegisters::allocateBars()
		{
			Bars = cnvme::Payload(BAR_SIZE * 6); // 6 bars
			PCI_HEADER* PciHeader = getPciHeader();

			// BAR 0
			UINT_64 baseAddress = (UINT_64)&(*Bars.getBuffer());
			PciHeader->MLBAR.BA = baseAddress & 0x3ffff; // 18 lower bits
			PciHeader->MLBAR.TP = 2; // Support 64 bit addresses

			// Each next BAR must be within 32 bit worth of bytes ahead

			// BAR 1
			PciHeader->MUBAR.BA = (UINT_32)(baseAddress >> 18);

			// Reconstruction code
			// UINT_64 addr = PciHeader->MLBAR.BA + (PciHeader->MUBAR.BA << 18);
			// BAR0 = ((UINT_8*)addr);

			// BAR 2 technically only would need 8 bytes of space...
			PciHeader->IDBAR.BA = BAR_SIZE * 2;
			PciHeader->IDBAR.RTE = 1; // I/O Space

			// Bar 3
			PciHeader->BAR3.BAR = BAR_SIZE * 3;

			// Bar 4
			PciHeader->BAR4.BAR = BAR_SIZE * 4;

			// Bar 5
			PciHeader->BAR5.BAR = BAR_SIZE * 5;
		}

		void PCIExpressRegisters::allocateCapabilities()
		{
			UINT_8* Capabilities = (PciHeaderAndCapabilities.getBuffer() + sizeof(PCI_HEADER));
			PCI_HEADER* PciHeader = getPciHeader();

			PciHeader->CAP.CP = sizeof(PCI_HEADER); // Put the first capability right after the header

			// Capability 1 - PMCAP
			PCI_POWER_MANAGEMENT_CAPABILITIES* PMCAP = (PCI_POWER_MANAGEMENT_CAPABILITIES*)(((UINT_8*)(PciHeader)) + PciHeader->CAP.CP);

			PMCAP->PID.CID = 1; // PCI Power Management Capability
			PMCAP->PID.NEXT = PciHeader->CAP.CP + sizeof(PCI_POWER_MANAGEMENT_CAPABILITIES); // Next will be right after this

			PMCAP->PC.VS = 3; // Version 1.2 (I think)

			PMCAP->PMCS.NSFRST = 1; // No internal reset on D3hot to D0

			// Capability 2 - MSICAP
			PCI_MESSAGE_SIGNALED_INTERRUPT_CAPABILITY* MSICAP = (PCI_MESSAGE_SIGNALED_INTERRUPT_CAPABILITY*)(((UINT_8*)(PciHeader)) + PMCAP->PID.NEXT);

			MSICAP->MID.CID = 5; // PCI MSI Capability
			MSICAP->MID.NEXT = PMCAP->PID.NEXT + sizeof(PCI_MESSAGE_SIGNALED_INTERRUPT_CAPABILITY);

			MSICAP->MC.C64 = 1; // Supports 64-bit addresses

			// Currently don't support MSI. Use traditional interrupts.

			// Capability 3 - MSIXCAP 
			PCI_MESSAGE_SIGNALED_INTERRUPT_X_CAPABILITY* MSIXCAP = (PCI_MESSAGE_SIGNALED_INTERRUPT_X_CAPABILITY*)(((UINT_8*)(PciHeader)) + MSICAP->MID.NEXT);

			MSIXCAP->MXID.CID = 0x11; // PCI MSIX Capability
			MSIXCAP->MXID.NEXT = MSICAP->MID.NEXT + sizeof(PCI_MESSAGE_SIGNALED_INTERRUPT_X_CAPABILITY);

			// Capability 4 - PXCAP
			PCI_EXPRESS_CAPABILITY* PXCAP = (PCI_EXPRESS_CAPABILITY*)(((UINT_8*)(PciHeader)) + MSIXCAP->MXID.NEXT);

			PXCAP->PXID.CID = 0x10; // PCI Express Capability
			PXCAP->PXID.NEXT = 0; // End of capability list

			PXCAP->PXCAP.VER = 2; // Version 2 of this structure

			PXCAP->PXDCAP.FLRC = 1; // Support function level reset
			PXCAP->PXDCAP.RER = 1; // Supports role based error reporting

			PXCAP->PXLS.SCC = 1; // Slot Clock Configuration to show same physical clock used for all

			PXCAP->PXLS.NLW = 8; // x8 width
			PXCAP->PXLS.CLS = 8; // x8 speed
		}

		cnvme::Payload PCIExpressRegisters::readHeaderAndCapabilities()
		{
			return cnvme::Payload(PciHeaderAndCapabilities);
		}

		void PCIExpressRegisters::writeHeaderAndCapabilities(const cnvme::Payload& payload)
		{
			// Update registers
			PciHeaderAndCapabilities = payload;
		}

		PCI_EXPRESS_REGISTERS PCIExpressRegisters::getPciExpressRegisters()
		{
			PCI_EXPRESS_REGISTERS Registers = { 0 };
			Registers.PciHeader = getPciHeader();

			UINT_32 capOffset = Registers.PciHeader->CAP.CP;
			UINT_32 lastOffset = 0;

			// Go through capability linked list
			while (capOffset != 0 && lastOffset != capOffset)
			{
				UINT_8* capPointer = (PciHeaderAndCapabilities.getBuffer() + capOffset);
				PCI_CAPABILITY_ID* capId = ((PCI_CAPABILITY_ID*)capPointer);

				if (capId->CID == CID_PMCAP) // PCI Power Management
				{
					Registers.PMCAP = (PCI_POWER_MANAGEMENT_CAPABILITIES*)capPointer;
				}
				else if (capId->CID == CID_MSICAP) // PCI MSI 
				{
					Registers.MSICAP = (PCI_MESSAGE_SIGNALED_INTERRUPT_CAPABILITY*)capPointer;
				}
				else if (capId->CID == CID_MSIXCAP) // PCI MSIX 
				{
					Registers.MSIXCAP = (PCI_MESSAGE_SIGNALED_INTERRUPT_X_CAPABILITY*)capPointer;
				}
				else if (capId->CID == CID_PXCAP) // PCI Express 
				{
					Registers.PXCAP = (PCI_EXPRESS_CAPABILITY*)capPointer;
				}
				else
				{
					LOG_ERROR("Unknown CID: 0x" + strings::toHexString(capId->CID) + ". Breaking loop.");
				}

				lastOffset = capOffset;
				capOffset = capId->NEXT;
			}

			// Find extended capabilities
			capOffset = 0x1000;

			while (capOffset != 0 && lastOffset != capOffset)
			{
				UINT_8* capPointer = (PciHeaderAndCapabilities.getBuffer() + capOffset);
				PCI_AER_CAPABILITY_ID* capId = ((PCI_AER_CAPABILITY_ID*)capPointer);

				// No Extended Capabilities.
				if (capId->CID == 0)
				{
					break;
				}

				if (capId->CID == CID_AERCAP) // AER
				{
					Registers.AERCAP = (PCI_ADVANCED_ERROR_REPORTING_CAPABILITY*)capPointer;
				}
				else
				{
					LOG_ERROR("Unknown Extended CID: 0x" + strings::toHexString(capId->CID) + ". Breaking loop.");
				}

				lastOffset = capOffset;
				capOffset = capId->NEXT;
			}

			return Registers;
		}

		void PCIExpressRegisters::checkForChanges()
		{
			auto &regs = getPciExpressRegisters();

			// If we can't find PXCAP or IFLR was set to 1 do the function level reset
			if ((!regs.PXCAP) || regs.PXCAP->PXDC.IFLR == 1)
			{
				resetPciHeader(); // Function level reset
				LOG_INFO("Initiated a function level reset");
			}
		}

		PCI_HEADER* PCIExpressRegisters::getPciHeader()
		{
			return (PCI_HEADER*)PciHeaderAndCapabilities.getBuffer();
		}

		void PCIExpressRegisters::listenForChanges()
		{
			while (listeningThreadBool)
			{
				// Check for changes twice a second
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				checkForChanges();
			}
		}
	}
}
