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
				retStr += strings::toString(ToStringParams(RSVD0, "Initiate Function Level Reset - A write of `1' initiates Function Level Reset to the Function. The value read by software from this bit shall always `0'"));
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


		}

	}
}
