/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
PCIe.h - A header file for the PCIe Registers
*/

#pragma once

#include "Types.h"

namespace cnvme
{
	namespace pci
	{
		namespace header
		{
			typedef struct PCI_IDENTIFIERS
			{
				UINT_16 VID; // Vendor Id
				UINT_16 DID; // Device Id

				std::string toString() const;
			} PCI_IDENTIFIERS, *PPCI_IDENTIFIERS;
			static_assert(sizeof(PCI_IDENTIFIERS) == 4, "ID should be 4 bytes in size.");

			typedef struct PCI_COMMAND
			{
				UINT_16 IOSE : 1; // I/O Space Enable
				UINT_16 MSE : 1; // Memory Space Enable
				UINT_16 BME : 1; // Bus Master Enable
				UINT_16 SCE : 1; // Special Cycle Enable
				UINT_16 MWIE : 1; // Memory Write and Invalidate Enable
				UINT_16 VGA : 1; // VGA Palette Snooping Enable
				UINT_16 PEE : 1; // Parity Error Response Enable
				UINT_16 RSVD0 : 1; // Hardwired to 0
				UINT_16 SEE : 1; //SERR# Enable
				UINT_16 FBE : 1; // Fast Back-to-Back Enable
				UINT_16 ID : 1; // Interrupt Disable
				UINT_16 RSVD1 : 5; // Reserved

				std::string toString() const;
			}PCI_COMMAND, *PPCI_COMMAND;
			static_assert(sizeof(PCI_COMMAND) == 2, "CMD should be 2 bytes in size.");

			typedef struct PCI_DEVICE_STATUS
			{
				UINT_16 RSVD0 : 3; // Reserved
				UINT_16 IS : 1; // Interrupt Status
				UINT_16 CL : 1; // Capabilities List
				UINT_16 C66 : 1; // 66 MHz Capable
				UINT_16 RSVD1 : 1; // Reserved
				UINT_16 FBC : 1; // Fast Back-to-Back Capable
				UINT_16 DPD : 1; // Master Data Parity Error Detected
				UINT_16 DEVT : 2; // DEVSEL# Timing
				UINT_16 STA : 1; // Signaled Target-Abort
				UINT_16 RTA : 1; // Received Target-Abort
				UINT_16 RMA : 1; // Received Master-Abort
				UINT_16 SSE : 1; // Signaled System Error
				UINT_16 DPE : 1; // Detected Parity Error

				std::string toString() const;
			}PCI_DEVICE_STATUS, *PPCI_DEVICE_STATUS;
			static_assert(sizeof(PCI_DEVICE_STATUS) == 2, "STS should be 2 bytes in size.");

			typedef struct PCI_REVISION_ID
			{
				UINT_8 RID; // Revision ID

				std::string toString() const;
			}PCI_REVISION_ID, *PPCI_REVISION_ID;
			static_assert(sizeof(PCI_REVISION_ID) == 1, "RID should be 1 byte in size.");

			typedef struct PCI_CLASS_CODE
			{
				UINT_8 PI; // Programming Interface
				UINT_8 SCC; // Sub Class Code
				UINT_8 BCC; // Base Class Code

				std::string toString() const;
			}PCI_CLASS_CODE, *PPCI_CLASS_CODE;
			static_assert(sizeof(PCI_CLASS_CODE) == 3, "CC should be 3 bytes in size.");

			typedef struct PCI_CACHE_LINE_SIZE
			{
				UINT_8 CLS; // Cache Line Size

				std::string toString() const;
			}PCI_CACHE_LINE_SIZE, *PPCI_CACHE_LINE_SIZE;
			static_assert(sizeof(PCI_CACHE_LINE_SIZE) == 1, "CLS should be 1 byte in size.");

			typedef struct PCI_MASTER_LATENCY_TIMER
			{
				UINT_8 MLT; // Master Latency Timer

				std::string toString() const;
			}PCI_MASTER_LATENCY_TIMER, *PPCI_MASTER_LATENCY_TIMER;
			static_assert(sizeof(PCI_MASTER_LATENCY_TIMER) == 1, "MLT should be 1 byte in size.");

			typedef struct PCI_HEADER_TYPE
			{
				UINT_8 HL : 1; // Header Layout
				UINT_8 MFD : 7; // Multi-Function Device

				std::string toString() const;
			}PCI_HEADER_TYPE, *PPCI_HEADER_TYPE;
			static_assert(sizeof(PCI_HEADER_TYPE) == 1, "HTYPE should be 1 byte in size.");

			typedef struct PCI_BUILT_IN_SELF_TEST
			{
				UINT_8 CC : 4; // Completion Code
				UINT_8 RSVD0 : 2; // Reserved
				UINT_8 SB : 1; // Start BIST
				UINT_8 BC : 1; // BIST Capable

				std::string toString() const;
			} PCI_BUILT_IN_SELF_TEST, *PPCI_BUILT_IN_SELF_TEST;
			static_assert(sizeof(PCI_BUILT_IN_SELF_TEST) == 1, "BIST should be 1 byte(s) in size.");

			typedef struct PCI_MEMORY_REGISTER_BASE_ADDRESS_LOWER_32
			{
				UINT_32 RTE : 1; // Resource Type Indicator
				UINT_32 TP : 2; // Type
				UINT_32 PF : 1; // Prefetchable
				UINT_32 RSVD0 : 10; // Reserved
				UINT_32 BA : 18; // Base Address

				std::string toString() const;
			} PCI_MEMORY_REGISTER_BASE_ADDRESS_LOWER_32, *PPCI_MEMORY_REGISTER_BASE_ADDRESS_LOWER_32;
			static_assert(sizeof(PCI_MEMORY_REGISTER_BASE_ADDRESS_LOWER_32) == 4, "MLBAR should be 4 byte(s) in size.");

			typedef struct PCI_MEMORY_REGISTER_BASE_ADDRESS_UPPER_32
			{
				UINT_32 BA; // Base Address

				std::string toString() const;
			} PCI_MEMORY_REGISTER_BASE_ADDRESS_UPPER_32, *PPCI_MEMORY_REGISTER_BASE_ADDRESS_UPPER_32;
			static_assert(sizeof(PCI_MEMORY_REGISTER_BASE_ADDRESS_UPPER_32) == 4, "MUBAR should be 4 byte(s) in size.");

			typedef struct PCI_INDEX_DATA_PAIR_REGISTER_BASE_ADDRESS
			{
				UINT_32 RTE : 1; // Resource Type Indicator
				UINT_32 RSVD0 : 2; // Reserved
				UINT_32 BA : 29; // Base Address

				std::string toString() const;
			} PCI_INDEX_DATA_PAIR_REGISTER_BASE_ADDRESS, *PPCI_INDEX_DATA_PAIR_REGISTER_BASE_ADDRESS;
			static_assert(sizeof(PCI_INDEX_DATA_PAIR_REGISTER_BASE_ADDRESS) == 4, "IDBAR should be 4 byte(s) in size.");

			typedef struct PCI_GENERIC_BAR
			{
				UINT_32 BAR; // Register Value

				std::string toString(int barNumber) const;
			} PCI_GENERIC_BAR, *PPCI_GENERIC_BAR;
			static_assert(sizeof(PCI_GENERIC_BAR) == 4, "BAR should be 4 byte(s) in size.");

			typedef struct PCI_CARDBUS_CIS_POINTER_BIT_TYPE_RESET_DESCRIPTION
			{
				UINT_32 RSVD0; // Not supported by NVM Express.

				std::string toString() const;
			} PCI_CARDBUS_CIS_POINTER_BIT_TYPE_RESET_DESCRIPTION, *PPCI_CARDBUS_CIS_POINTER_BIT_TYPE_RESET_DESCRIPTION;
			static_assert(sizeof(PCI_CARDBUS_CIS_POINTER_BIT_TYPE_RESET_DESCRIPTION) == 4, "CCPTR should be 4 byte(s) in size.");

			typedef struct PCI_SUB_SYSTEM_IDENTIFIERS
			{
				UINT_16 SSVID; // Subsystem Vendor ID
				UINT_16 SSID; // Subsystem ID

				std::string toString() const;
			} PCI_SUB_SYSTEM_IDENTIFIERS, *PPCI_SUB_SYSTEM_IDENTIFIERS;
			static_assert(sizeof(PCI_SUB_SYSTEM_IDENTIFIERS) == 4, "SS should be 4 byte(s) in size.");

			typedef struct PCI_EXPANSION_ROM
			{
				UINT_32 RBA; // ROM Base Address

				std::string toString() const;
			} PCI_EXPANSION_ROM, *PPCI_EXPANSION_ROM;
			static_assert(sizeof(PCI_EXPANSION_ROM) == 4, "EROM should be 4 byte(s) in size.");

			typedef struct PCI_CAPABILITIES_POINTER
			{
				UINT_8 CP; // Capability Pointer

				std::string toString() const;
			} PCI_CAPABILITIES_POINTER, *PPCI_CAPABILITIES_POINTER;
			static_assert(sizeof(PCI_CAPABILITIES_POINTER) == 1, "CAP should be 1 byte(s) in size.");

			typedef struct PCI_INTERRUPT_INFORMATION
			{
				UINT_8 ILINE; // Interrupt Line
				UINT_8 IPIN; // Interrupt Pin

				std::string toString() const;
			} PCI_INTERRUPT_INFORMATION, *PPCI_INTERRUPT_INFORMATION;
			static_assert(sizeof(PCI_INTERRUPT_INFORMATION) == 2, "INTR should be 2 byte(s) in size.");

			typedef struct PCI_MINIMUM_GRANT
			{
				UINT_8 GNT; // Grant

				std::string toString() const;
			} PCI_MINIMUM_GRANT, *PPCI_MINIMUM_GRANT;
			static_assert(sizeof(PCI_MINIMUM_GRANT) == 1, "MGNT should be 1 byte(s) in size.");

			typedef struct PCI_MAXIMUM_LATENCY
			{
				UINT_8 LAT; // Latency

				std::string toString() const;
			} PCI_MAXIMUM_LATENCY, *PPCI_MAXIMUM_LATENCY;
			static_assert(sizeof(PCI_MAXIMUM_LATENCY) == 1, "MLAT should be 1 byte(s) in size.");

			/*! This is the PCI Header structure (NVMe 1.2.1 section 2.1)
			*/
			typedef struct PCI_HEADER
			{
				PCI_IDENTIFIERS ID;
				PCI_COMMAND CMD;
				PCI_DEVICE_STATUS STS;
				PCI_REVISION_ID RID;
				PCI_CLASS_CODE CC;
				PCI_CACHE_LINE_SIZE CLS;
				PCI_MASTER_LATENCY_TIMER MLT;
				PCI_HEADER_TYPE HTYPE;
				PCI_BUILT_IN_SELF_TEST BIST;
				PCI_MEMORY_REGISTER_BASE_ADDRESS_LOWER_32 MLBAR;
				PCI_MEMORY_REGISTER_BASE_ADDRESS_UPPER_32 MUBAR;
				PCI_INDEX_DATA_PAIR_REGISTER_BASE_ADDRESS IDBAR;
				PCI_GENERIC_BAR BAR3;
				PCI_GENERIC_BAR BAR4;
				PCI_GENERIC_BAR BAR5;
				PCI_CARDBUS_CIS_POINTER_BIT_TYPE_RESET_DESCRIPTION CCPTR;
				PCI_SUB_SYSTEM_IDENTIFIERS SS;
				PCI_EXPANSION_ROM EROM;
				PCI_CAPABILITIES_POINTER CAP;
				UINT_8 RSVD0[7];
				PCI_INTERRUPT_INFORMATION INTR;
				PCI_MINIMUM_GRANT MGNT;
				PCI_MAXIMUM_LATENCY MLAT;

				std::string toString() const;
			} PCI_HEADER, *PPCI_HEADER;
			static_assert(sizeof(PCI_HEADER) == 64, "PCI_HEADER should be 64 byte(s) in size.");
		}

		namespace capabilities
		{
			typedef struct PCI_CAPABILITY_ID
			{
				UINT_8 CID; // Cap ID
				UINT_8 NEXT; // Next Capability

				std::string toString() const;
			} PCI_CAPABILITY_ID, *PPCI_CAPABILITY_ID;
			static_assert(sizeof(PCI_CAPABILITY_ID) == 2, "CAPID should be 2 byte(s) in size.");

			typedef struct PCI_POWER_MANAGEMENT
			{
				UINT_16 VS : 3; // Version
				UINT_16 PMEC : 1; // PME Clock
				UINT_16 RSVD0 : 1; // Reserved
				UINT_16 DSI : 1; // Device Specific Initialization
				UINT_16 AUXC : 3; // Aux Current
				UINT_16 D1S : 1; // D1 Support
				UINT_16 D2S : 1; // D2 Support
				UINT_16 PSUP : 5; // PME Support

				std::string toString() const;
			} PCI_POWER_MANAGEMENT, *PPCI_POWER_MANAGEMENT;
			static_assert(sizeof(PCI_POWER_MANAGEMENT) == 2, "PC should be 2 byte(s) in size.");

			typedef struct PCI_POWER_MANAGEMENT_CONTROL_AND_STATUS
			{
				UINT_8 PS : 2; // Power State
				UINT_8 RSVD1 : 1; // Reserved
				UINT_8 NSFRST : 1; // No Soft Reset
				UINT_8 RSVD0 : 4; // Reserved
				UINT_8 PMEE : 1; // PME Enable
				UINT_8 DSE : 4; // Data Select
				UINT_8 DSC : 2; // Data Scale
				UINT_8 PMES : 1; // PME Status

				std::string toString() const;
			} PCI_POWER_MANAGEMENT_CONTROL_AND_STATUS, *PPCI_POWER_MANAGEMENT_CONTROL_AND_STATUS;
			static_assert(sizeof(PCI_POWER_MANAGEMENT_CONTROL_AND_STATUS) == 2, "PMCS should be 2 byte(s) in size.");

			/*! This is the PCI Power Manangement Capabilities structure (NVMe 1.2.1 section 2.2)
			*/
			typedef struct PCI_POWER_MANAGEMENT_CAPABILITIES
			{
				PCI_CAPABILITY_ID PID;
				PCI_POWER_MANAGEMENT PC;
				PCI_POWER_MANAGEMENT_CONTROL_AND_STATUS PMCS;

				std::string toString() const;
			}PCI_POWER_MANAGEMENT_CAPABILITIES, *PPCI_POWER_MANAGEMENT_CAPABILITIES;
			static_assert(sizeof(PCI_POWER_MANAGEMENT_CAPABILITIES) == 6, "PMCAP should be 6 byte(s) in size.");

			typedef struct PCI_MESSAGE_SIGNALED_INTERRUPT_MESSAGE_CONTROL
			{
				UINT_8 MSIE : 1; // MSI Enable
				UINT_8 MMC : 3; // Multiple Message Capable
				UINT_8 MME : 3; // Multiple Message Enable
				UINT_8 C64 : 1; // 64 Bit Address Capable
				UINT_8 PVM : 1; // Per-Vector Masking Capable
				UINT_8 RSVD0 : 7; // Reserved

				std::string toString() const;
			} PCI_MESSAGE_SIGNALED_INTERRUPT_MESSAGE_CONTROL, *PPCI_MESSAGE_SIGNALED_INTERRUPT_MESSAGE_CONTROL;
			static_assert(sizeof(PCI_MESSAGE_SIGNALED_INTERRUPT_MESSAGE_CONTROL) == 2, "MC should be 2 byte(s) in size.");

			typedef struct PCI_MESSAGE_SIGNALED_INTERRUPT_MESSAGE_ADDRESS
			{
				UINT_32 RSVD0 : 2; // Reserved
				UINT_32 ADDR : 30; // Address

				std::string toString() const;
			} PCI_MESSAGE_SIGNALED_INTERRUPT_MESSAGE_ADDRESS, *PPCI_MESSAGE_SIGNALED_INTERRUPT_MESSAGE_ADDRESS;
			static_assert(sizeof(PCI_MESSAGE_SIGNALED_INTERRUPT_MESSAGE_ADDRESS) == 4, "MA should be 4 byte(s) in size.");

			typedef struct PCI_MESSAGE_SIGNALED_INTERRUPT_UPPER_ADDRESS
			{
				UINT_32 UADDR; // Upper Address

				std::string toString() const;
			} PCI_MESSAGE_SIGNALED_INTERRUPT_UPPER_ADDRESS, *PPCI_MESSAGE_SIGNALED_INTERRUPT_UPPER_ADDRESS;
			static_assert(sizeof(PCI_MESSAGE_SIGNALED_INTERRUPT_UPPER_ADDRESS) == 4, "MUA should be 4 byte(s) in size.");

			typedef struct PCI_MESSAGE_SIGNALED_INTERRUPT_MESSAGE_DATA
			{
				UINT_16 DATA; // Data

				std::string toString() const;
			} PCI_MESSAGE_SIGNALED_INTERRUPT_MESSAGE_DATA, *PPCI_MESSAGE_SIGNALED_INTERRUPT_MESSAGE_DATA;
			static_assert(sizeof(PCI_MESSAGE_SIGNALED_INTERRUPT_MESSAGE_DATA) == 2, "MD should be 2 byte(s) in size.");

			typedef struct PCI_MESSAGE_SIGNALED_INTERRUPT_MASK_BITS
			{
				UINT_32 MASK; // Mask Bits

				std::string toString() const;
			} PCI_MESSAGE_SIGNALED_INTERRUPT_MASK_BITS, *PPCI_MESSAGE_SIGNALED_INTERRUPT_MASK_BITS;
			static_assert(sizeof(PCI_MESSAGE_SIGNALED_INTERRUPT_MASK_BITS) == 4, "MMASK should be 4 byte(s) in size.");

			typedef struct PCI_MESSAGE_SIGNALED_INTERRUPT_PENDING_BITS
			{
				UINT_32 PEND; // Pending Bits

				std::string toString() const;
			} PCI_MESSAGE_SIGNALED_INTERRUPT_PENDING_BITS, *PPCI_MESSAGE_SIGNALED_INTERRUPT_PENDING_BITS;
			static_assert(sizeof(PCI_MESSAGE_SIGNALED_INTERRUPT_PENDING_BITS) == 4, "MPEND should be 4 byte(s) in size.");

			/*! This is the PCI Message Signaled Interrupt Capability structure (NVMe 1.2.1 section 2.3)
			*/
			typedef struct PCI_MESSAGE_SIGNALED_INTERRUPT_CAPABILITY
			{
				PCI_CAPABILITY_ID MID;
				PCI_MESSAGE_SIGNALED_INTERRUPT_MESSAGE_CONTROL MC;
				PCI_MESSAGE_SIGNALED_INTERRUPT_MESSAGE_ADDRESS MA;
				PCI_MESSAGE_SIGNALED_INTERRUPT_UPPER_ADDRESS MUA;
				PCI_MESSAGE_SIGNALED_INTERRUPT_MESSAGE_DATA MD;
				UINT_16 RSVD0;
				PCI_MESSAGE_SIGNALED_INTERRUPT_MASK_BITS MMASK;
				PCI_MESSAGE_SIGNALED_INTERRUPT_PENDING_BITS MPEND;

				std::string toString() const;
			}PCI_MESSAGE_SIGNALED_INTERRUPT_CAPABILITY, *PPCI_MESSAGE_SIGNALED_INTERRUPT_CAPABILITY;
			static_assert(sizeof(PCI_MESSAGE_SIGNALED_INTERRUPT_CAPABILITY) == 24, "MSICAP should be 24 byte(s) in size.");

			typedef struct PCI_MSI_X_MESSAGE_CONTROL
			{
				UINT_16 TS : 11; // Table Size
				UINT_16 RSVD0 : 3; // Reserved
				UINT_16 FM : 1; // Function Mask
				UINT_16 MXE : 1; // MSI-X Enable

				std::string toString() const;
			} PCI_MSI_X_MESSAGE_CONTROL, *PPCI_MSI_X_MESSAGE_CONTROL;
			static_assert(sizeof(PCI_MSI_X_MESSAGE_CONTROL) == 2, "MXC should be 2 byte(s) in size.");

			typedef struct PCI_MSI_X_TABLE_OFFSET_TABLE_BIR
			{
				UINT_32 TBIR : 3; // Table BIR
				UINT_32 TO : 29; // Table Offset

				std::string toString() const;
			} PCI_MSI_X_TABLE_OFFSET_TABLE_BIR, *PPCI_MSI_X_TABLE_OFFSET_TABLE_BIR;
			static_assert(sizeof(PCI_MSI_X_TABLE_OFFSET_TABLE_BIR) == 4, "MTAB should be 4 byte(s) in size.");

			typedef struct PCI_MSI_X_PBA_OFFSET_PBA_BIR
			{
				UINT_32 PBIR : 3; // PBA BIR
				UINT_32 PBAO : 29; // PBA Offset

				std::string toString() const;
			} PCI_MSI_X_PBA_OFFSET_PBA_BIR, *PPCI_MSI_X_PBA_OFFSET_PBA_BIR;
			static_assert(sizeof(PCI_MSI_X_PBA_OFFSET_PBA_BIR) == 4, "MPBA should be 4 byte(s) in size.");

			/*! This is the PCI Message Signaled Interrupt X Capability structure (NVMe 1.2.1 section 2.4)
			*/
			typedef struct PCI_MESSAGE_SIGNALED_INTERRUPT_X_CAPABILITY
			{
				PCI_CAPABILITY_ID MXID;
				PCI_MSI_X_MESSAGE_CONTROL MXC;
				PCI_MSI_X_TABLE_OFFSET_TABLE_BIR MTAB;
				PCI_MSI_X_PBA_OFFSET_PBA_BIR MPBA;

				std::string toString() const;
			}PCI_MESSAGE_SIGNALED_INTERRUPT_X_CAPABILITY, *PPCI_MESSAGE_SIGNALED_INTERRUPT_X_CAPABILITY;
			static_assert(sizeof(PCI_MESSAGE_SIGNALED_INTERRUPT_X_CAPABILITY) == 12, "MSIXCAP should be 12 byte(s) in size.");

			typedef struct PCI_EXPRESS_CAPABILITIES
			{
				UINT_8 VER : 4; // Capability Version
				UINT_8 DPT : 4; // Device/Port Type
				UINT_8 SI : 1; // Slot Implemented
				UINT_8 IMN : 5; // Interrupt Message Number
				UINT_8 RSVD0 : 2; // Reserved

				std::string toString() const;
			} PCI_EXPRESS_CAPABILITIES, *PPCI_EXPRESS_CAPABILITIES;
			static_assert(sizeof(PCI_EXPRESS_CAPABILITIES) == 2, "PXCAP should be 2 byte(s) in size.");

			typedef struct PCI_EXPRESS_DEVICE_CAPABILITIES
			{
				UINT_32 MPS : 3; // Max Payload Size Supported
				UINT_32 PFS : 2; // Phantom Functions Supported
				UINT_32 ETFS : 1; // Extended Tag Field Supported
				UINT_32 L0SL : 3; // Endpoint L0s Acceptable Latency
				UINT_32 L1L : 3; // Endpoint L1 Acceptable Latency
				UINT_32 RSVD2 : 3; // Reserved
				UINT_32 RER : 1; // Role-based Error Reporting
				UINT_32 RSVD1 : 2; // Reserved
				UINT_32 CSPLV : 8; // Captured Slot Power Limit Value
				UINT_32 CSPLS : 2; // Captured Slot Power Limit Scale
				UINT_32 FLRC : 1; // Function Level Reset Capability
				UINT_32 RSVD0 : 3; // Reserved

				std::string toString() const;
			} PCI_EXPRESS_DEVICE_CAPABILITIES, *PPCI_EXPRESS_DEVICE_CAPABILITIES;
			static_assert(sizeof(PCI_EXPRESS_DEVICE_CAPABILITIES) == 4, "PXDCAP should be 4 byte(s) in size.");

			typedef struct PCI_EXPRESS_DEVICE_CONTROL
			{
				UINT_8 CERE : 1; // Correctable Error Reporting Enable
				UINT_8 NFERE : 1; // Non-Fatal Error Reporting Enable
				UINT_8 FERE : 1; // Fatal Error Reporting Enable
				UINT_8 URRE : 1; // Unsupported Request Reporting Enable
				UINT_8 ERO : 1; // Enable Relaxed Ordering
				UINT_8 MPS : 3; // Max Payload Size
				UINT_8 ETE : 1; // Extended Tag Enable
				UINT_8 PFE : 1; // Phantom Functions Enable
				UINT_8 APPME : 1; // AUX Power PM Enable
				UINT_8 ENS : 1; // Enable No Snoop
				UINT_8 MRRS : 3; // Max Read Request Size
				UINT_8 RSVD0 : 1; // Initiate Function Level Reset - A write of `1' initiates Function Level Reset to the Function. The value read by software from this bit shall always `0'

				std::string toString() const;
			} PCI_EXPRESS_DEVICE_CONTROL, *PPCI_EXPRESS_DEVICE_CONTROL;
			static_assert(sizeof(PCI_EXPRESS_DEVICE_CONTROL) == 2, "PXDC should be 2 byte(s) in size.");

			typedef struct PCI_EXPRESS_DEVICE_STATUS
			{
				UINT_16 CED : 1; // Correctable Error Detected
				UINT_16 NFED : 1; // Non-Fatal Error Detected
				UINT_16 FED : 1; // Fatal Error Detected
				UINT_16 URD : 1; // Unsupported Request Detected
				UINT_16 APD : 1; // AUX Power Detected
				UINT_16 TP : 1; // Transactions Pending
				UINT_16 RSVD0 : 10; // Reserved

				std::string toString() const;
			} PCI_EXPRESS_DEVICE_STATUS, *PPCI_EXPRESS_DEVICE_STATUS;
			static_assert(sizeof(PCI_EXPRESS_DEVICE_STATUS) == 2, "PXDS should be 2 byte(s) in size.");

			typedef struct PCI_EXPRESS_LINK_CAPABILITIES
			{
				UINT_32 SLS : 4; // Supported Link Speeds
				UINT_32 MLW : 6; // Maximum Link Width
				UINT_32 ASPMS : 2; // Active State Power Management Support
				UINT_32 L0SEL : 3; // L0s Exit Latency
				UINT_32 L1EL : 3; // L1 Exit Latency
				UINT_32 CPM : 1; // Clock Power Management
				UINT_32 SDERC : 1; // Surprise Down Error Reporting Capable
				UINT_32 DLLLA : 1; // Data Link Layer Link Active Reporting Capable
				UINT_32 LBNC : 1; // Link Bandwidth Notification Capability
				UINT_32 AOC : 1; // ASPM Optionality Compliance
				UINT_32 RSVD0 : 1; // Reserved
				UINT_32 PN : 8; // Port Number

				std::string toString() const;
			} PCI_EXPRESS_LINK_CAPABILITIES, *PPCI_EXPRESS_LINK_CAPABILITIES;
			static_assert(sizeof(PCI_EXPRESS_LINK_CAPABILITIES) == 4, "PXLCAP should be 4 byte(s) in size.");

			typedef struct PCI_EXPRESS_LINK_CONTROL
			{
				UINT_8 ASPMC : 2; // Active State Power Management Control
				UINT_8 RSVD2 : 1; // Reserved
				UINT_8 RCB : 1; // Read Completion Boundary
				UINT_8 RSVD1 : 2; // Reserved: These bits are reserved on Endpoints
				UINT_8 CCC : 1; // Common Clock Configuration
				UINT_8 ES : 1; // Extended Synch
				UINT_8 ECPM : 1; // Enable Clock Power Management
				UINT_8 HAWD : 1; // Hardware Autonomous Width Disable
				UINT_8 RSVD0 : 6; // Reserved

				std::string toString() const;
			} PCI_EXPRESS_LINK_CONTROL, *PPCI_EXPRESS_LINK_CONTROL;
			static_assert(sizeof(PCI_EXPRESS_LINK_CONTROL) == 2, "PXLC should be 2 byte(s) in size.");

			typedef struct PCI_EXPRESS_LINK_STATUS
			{
				UINT_16 CLS : 4; // Current Link Speed
				UINT_16 NLW : 6; // Negotiated Link Width
				UINT_16 RSVD1 : 2; // Reserved
				UINT_16 SCC : 1; // Slot Clock Configuration
				UINT_16 RSVD0 : 3; // Reserved

				std::string toString() const;
			} PCI_EXPRESS_LINK_STATUS, *PPCI_EXPRESS_LINK_STATUS;
			static_assert(sizeof(PCI_EXPRESS_LINK_STATUS) == 2, "PXLS should be 2 byte(s) in size.");

			typedef struct PCI_EXPRESS_DEVICE_CAPABILITIES_2
			{
				UINT_32 CTRS : 4; // Completion Timeout Ranges Supported
				UINT_32 CTDS : 1; // Completion Timeout Disable Supported
				UINT_32 ARIFS : 1; // ARI Forwarding Supported
				UINT_32 AORS : 1; // AtomicOp Routing Supported
				UINT_32 AOCS32 : 1; // 32-bit AtomicOp Completer Supported
				UINT_32 AOCS64 : 1; // 64-bit AtomicOp Completer Supported
				UINT_32 CCS128 : 1; // 128-bit CAS Completer Supported
				UINT_32 NPRPR : 1; // No RO-enabled PR-PR Passing
				UINT_32 LTRS : 1; // Latency Tolerance Reporting Supported
				UINT_32 TPHCS : 2; // TPH Completer Supported
				UINT_32 RSVD1 : 4; // Reserved
				UINT_32 OBFFS : 2; // OBFF Supported
				UINT_32 EFFS : 1; // Extended Fmt Field Supported
				UINT_32 EETPS : 1; // End-End TLP Prefix Supported
				UINT_32 MEETP : 2; // Max End-End TLP Prefixes
				UINT_32 RSVD0 : 8; // Reserved

				std::string toString() const;
			} PCI_EXPRESS_DEVICE_CAPABILITIES_2, *PPCI_EXPRESS_DEVICE_CAPABILITIES_2;
			static_assert(sizeof(PCI_EXPRESS_DEVICE_CAPABILITIES_2) == 4, "PXDCAP2 should be 4 byte(s) in size.");

			typedef struct PCI_EXPRESS_DEVICE_CONTROL_2
			{
				UINT_32 RSVD3 : 4; // Completion Timeout Value:
				UINT_32 CTD : 1; // Completion Timeout Disable
				UINT_32 RSVD2 : 5; // Reserved
				UINT_32 LTRME : 1; // Latency Tolerance Reporting Mechanism Enable
				UINT_32 RSVD1 : 2; // Reserved
				UINT_32 OBFFE : 2; // OBFF Enable
				UINT_32 RSVD0 : 17; // Reserved

				std::string toString() const;
			} PCI_EXPRESS_DEVICE_CONTROL_2, *PPCI_EXPRESS_DEVICE_CONTROL_2;
			static_assert(sizeof(PCI_EXPRESS_DEVICE_CONTROL_2) == 4, "PXDC2 should be 4 byte(s) in size.");

			/*! This is the PCI Express Capability structure (NVMe 1.2.1 section 2.5)
			*/
			typedef struct PCI_EXPRESS_CAPABILITY
			{
				PCI_CAPABILITY_ID PXID;
				PCI_EXPRESS_CAPABILITIES PXCAP;
				PCI_EXPRESS_DEVICE_CAPABILITIES PXDCAP;
				PCI_EXPRESS_DEVICE_CONTROL PXDC;
				PCI_EXPRESS_DEVICE_STATUS PXDS;
				PCI_EXPRESS_LINK_CAPABILITIES PXLCAP;
				PCI_EXPRESS_LINK_CONTROL PXLC;
				PCI_EXPRESS_LINK_STATUS PXLS;
				UINT_8 RSVD0[16];
				PCI_EXPRESS_DEVICE_CAPABILITIES_2 PXDCAP2;
				PCI_EXPRESS_DEVICE_CONTROL_2 PXDC2;

				std::string toString() const;
			}PCI_EXPRESS_CAPABILITY, *PPCI_EXPRESS_CAPABILITY;
			static_assert(sizeof(PCI_EXPRESS_CAPABILITY) == 44, "PXCAP should be 44 byte(s) in size.");

			// Extended Capabilities start at 0x100, after the regular ones

			typedef struct PCI_AER_CAPABILITY_ID
			{
				UINT_16 CID; // Capability ID
				UINT_16 CVER : 4; // Capability Version
				UINT_16 NEXT : 12; // Next Pointer

				std::string toString() const;
			} PCI_AER_CAPABILITY_ID, *PPCI_AER_CAPABILITY_ID;
			static_assert(sizeof(PCI_AER_CAPABILITY_ID) == 4, "AERID should be 4 byte(s) in size.");

			typedef struct PCI_AER_UNCORRECTABLE_ERROR_STATUS_REGISTER
			{
				UINT_32 RSVD2 : 4; // Reserved
				UINT_32 DLPES : 1; // Data Link Protocol Error Status
				UINT_32 RSVD1 : 7; // Reserved
				UINT_32 PTS : 1; // Poisoned TLP Status
				UINT_32 FCPES : 1; // Flow Control Protocol Error Status
				UINT_32 CTS : 1; // Completion Timeout Status
				UINT_32 CAS : 1; // Completer Abort Status
				UINT_32 UCS : 1; // Unexpected Completion Status
				UINT_32 ROS : 1; // Receiver Overflow Status
				UINT_32 MTS : 1; // Malformed TLP Status
				UINT_32 ECRCES : 1; // ECRC Error Status
				UINT_32 URES : 1; // Unsupported Request Error Status
				UINT_32 ACSVS : 1; // ACS Violation Status
				UINT_32 UIES : 1; // Uncorrectable Internal Error Status
				UINT_32 MCBTS : 1; // MC Blocked TLP Status
				UINT_32 AOEBS : 1; // AtomicOp Egress Blocked Status
				UINT_32 TPBES : 1; // TLP Prefix Blocked Error Status
				UINT_32 RSVD0 : 6; // Reserved

				std::string toString() const;
			} PCI_AER_UNCORRECTABLE_ERROR_STATUS_REGISTER, *PPCI_AER_UNCORRECTABLE_ERROR_STATUS_REGISTER;
			static_assert(sizeof(PCI_AER_UNCORRECTABLE_ERROR_STATUS_REGISTER) == 4, "AERUCES should be 4 byte(s) in size.");

			typedef struct PCI_AER_UNCORRECTABLE_ERROR_MASK_REGISTER
			{
				UINT_32 RSVD2 : 4; // Reserved
				UINT_32 DLPEM : 1; // Data Link Protocol Error Mask
				UINT_32 RSVD1 : 7; // Reserved
				UINT_32 PTM : 1; // Poisoned TLP Mask
				UINT_32 FCPEM : 1; // Flow Control Protocol Error Mask
				UINT_32 CTM : 1; // Completion Timeout Mask
				UINT_32 CAM : 1; // Completer Abort Mask
				UINT_32 UCM : 1; // Unexpected Completion Mask
				UINT_32 ROM : 1; // Receiver Overflow Mask
				UINT_32 MTM : 1; // Malformed TLP Mask
				UINT_32 ECRCEM : 1; // ECRC Error Mask
				UINT_32 UREM : 1; // Unsupported Request Error Mask
				UINT_32 ACSVM : 1; // ACS Violation Mask
				UINT_32 UIEM : 1; // Uncorrectable Internal Error Mask
				UINT_32 MCBTM : 1; // MC Blocked TLP Mask
				UINT_32 AOEBM : 1; // AtomicOp Egress Blocked Mask
				UINT_32 TPBEM : 1; // TLP Prefix Blocked Error Mask
				UINT_32 RSVD0 : 6; // Reserved

				std::string toString() const;
			} PCI_AER_UNCORRECTABLE_ERROR_MASK_REGISTER, *PPCI_AER_UNCORRECTABLE_ERROR_MASK_REGISTER;
			static_assert(sizeof(PCI_AER_UNCORRECTABLE_ERROR_MASK_REGISTER) == 4, "AERUCEM should be 4 byte(s) in size.");

			typedef struct PCI_AER_UNCORRECTABLE_ERROR_SEVERITY_REGISTER
			{
				UINT_32 RSVD2 : 4; // Reserved
				UINT_32 DLPESEV : 1; // Data Link Protocol Error Severity
				UINT_32 RSVD1 : 7; // Reserved
				UINT_32 PTSEV : 1; // Poisoned TLP Severity
				UINT_32 FCPESEV : 1; // Flow Control Protocol Error Severity
				UINT_32 CTSEV : 1; // Completion Timeout Severity
				UINT_32 CASEV : 1; // Completer Abort Severity
				UINT_32 UCSEV : 1; // Unexpected Completion Severity
				UINT_32 ROSEV : 1; // Receiver Overflow Severity
				UINT_32 MTSEV : 1; // Malformed TLP Severity
				UINT_32 ECRCESEV : 1; // ECRC Error Severity
				UINT_32 URESEV : 1; // Unsupported Request Error Severity
				UINT_32 ACSVSEV : 1; // ACS Violation Severity
				UINT_32 UIESEV : 1; // Uncorrectable Internal Error Severity
				UINT_32 MCBTSEV : 1; // MC Blocked TLP Severity
				UINT_32 AOEBSEV : 1; // AtomicOp Egress Blocked Severity
				UINT_32 TPBESEV : 1; // TLP Prefix Blocked Error Severity
				UINT_32 RSVD0 : 6; // Reserved

				std::string toString() const;
			} PCI_AER_UNCORRECTABLE_ERROR_SEVERITY_REGISTER, *PPCI_AER_UNCORRECTABLE_ERROR_SEVERITY_REGISTER;
			static_assert(sizeof(PCI_AER_UNCORRECTABLE_ERROR_SEVERITY_REGISTER) == 4, "AERUCESEV should be 4 byte(s) in size.");

			typedef struct PCI_AER_CORRECTABLE_ERROR_STATUS_REGISTER
			{
				UINT_8 RES : 1; // Receiver Error Status
				UINT_8 RSVD2 : 5; // Reserved
				UINT_8 BTS : 1; // Bad TLP Status
				UINT_8 BDS : 1; // Bad DLLP Status
				UINT_8 RRS : 1; // REPLAY NUM Rollover Status
				UINT_8 RSVD1 : 3; // Reserved
				UINT_8 RTS : 1; // Replay Timer Timeout Status
				UINT_8 ANFES : 1; // Advisory Non-Fatal Error Status
				UINT_8 CIES : 1; // Corrected Internal Error Status
				UINT_8 HLOS : 1; // Header Log Overflow Status
				UINT_16 RSVD0; // Reserved

				std::string toString() const;
			} PCI_AER_CORRECTABLE_ERROR_STATUS_REGISTER, *PPCI_AER_CORRECTABLE_ERROR_STATUS_REGISTER;
			static_assert(sizeof(PCI_AER_CORRECTABLE_ERROR_STATUS_REGISTER) == 4, "AERCS should be 4 byte(s) in size.");

			typedef struct PCI_AER_CORRECTABLE_ERROR_MASK_REGISTER
			{
				UINT_8 REM : 1; // Receiver Error Mask
				UINT_8 RSVD3 : 5; // Reserved
				UINT_8 BTM : 1; // Bad TLP Mask
				UINT_8 BDM : 1; // Bad DLLP Mask
				UINT_8 RRM : 1; // REPLAY NUM Rollover Mask
				UINT_8 RSVD2 : 3; // Reserved
				UINT_8 RTM : 1; // Replay Timer Timeout Mask
				UINT_8 RSVD1 : 1; // Advisory Non-Fatal Error Mask ANFEM)
				UINT_8 CIEM : 1; // Corrected Internal Error Mask
				UINT_8 HLOM : 1; // Header Log Overflow Mask
				UINT_16 RSVD0; // Reserved

				std::string toString() const;
			} PCI_AER_CORRECTABLE_ERROR_MASK_REGISTER, *PPCI_AER_CORRECTABLE_ERROR_MASK_REGISTER;
			static_assert(sizeof(PCI_AER_CORRECTABLE_ERROR_MASK_REGISTER) == 4, "AERCEM should be 4 byte(s) in size.");

			typedef struct PCI_AER_CAPABILITIES_AND_CONTROL_REGISTER
			{
				UINT_32 FEP : 5; // First Error Pointer
				UINT_32 EGC : 1; // ECRC Generation Capable
				UINT_32 EGE : 1; // ECRC Generation Enable
				UINT_32 ECC : 1; // ECRC Check Capable
				UINT_32 ECE : 1; // ECRC Check Enable
				UINT_32 MHRC : 1; // Multiple Header Recording Capable
				UINT_32 MHRE : 1; // Multiple Header Recording Enable
				UINT_32 TPLP : 1; // TLP Prefix Log Present
				UINT_32 RSVD0 : 20; // Reserved

				std::string toString() const;
			} PCI_AER_CAPABILITIES_AND_CONTROL_REGISTER, *PPCI_AER_CAPABILITIES_AND_CONTROL_REGISTER;
			static_assert(sizeof(PCI_AER_CAPABILITIES_AND_CONTROL_REGISTER) == 4, "AERCC should be 4 byte(s) in size.");

			typedef struct PCI_AER_HEADER_LOG_REGISTER
			{
				UINT_8 HB12; // Header Byte 12
				UINT_8 HB13; // Header Byte 13
				UINT_8 HB14; // Header Byte 14
				UINT_8 HB15; // Header Byte 15
				UINT_8 HB8; // Header Byte 8
				UINT_8 HB9; // Header Byte 9
				UINT_8 HB10; // Header Byte 10
				UINT_8 HB11; // Header Byte 11
				UINT_8 HB4; // Header Byte 4
				UINT_8 HB5; // Header Byte 5
				UINT_8 HB6; // Header Byte 6
				UINT_8 HB7; // Header Byte 7
				UINT_8 HB0; // Header Byte 0
				UINT_8 HB1; // Header Byte 1
				UINT_8 HB2; // Header Byte 2
				UINT_8 HB3; // Header Byte 3

				std::string toString() const;
			} PCI_AER_HEADER_LOG_REGISTER, *PPCI_AER_HEADER_LOG_REGISTER;
			static_assert(sizeof(PCI_AER_HEADER_LOG_REGISTER) == 16, "AERHL should be 16 byte(s) in size.");

			typedef struct PCI_AER_TLP_PREFIX_LOG_REGISTER
			{
				UINT_8 TPL4B0; // Fourth TLP Prefix Log Byte 0
				UINT_8 TPL4B1; // Fourth TLP Prefix Log Byte 1
				UINT_8 TPL4B2; // Fourth TLP Prefix Log Byte 2
				UINT_8 TPL4B3; // Fourth TLP Prefix Log Byte 3
				UINT_8 TPL3B0; // Third TLP Prefix Log Byte 0
				UINT_8 TPL3B1; // Third TLP Prefix Log Byte 1
				UINT_8 TPL3B2; // Third TLP Prefix Log Byte 2
				UINT_8 TPL3B3; // Third TLP Prefix Log Byte 3
				UINT_8 TPL2B0; // Second TLP Prefix Log Byte 0
				UINT_8 TPL2B1; // Second TLP Prefix Log Byte 1
				UINT_8 TPL2B2; // Second TLP Prefix Log Byte 2
				UINT_8 TPL2B3; // Second TLP Prefix Log Byte 3
				UINT_8 TPL1B0; // First TLP Prefix Log Byte 0
				UINT_8 TPL1B1; // First TLP Prefix Log Byte 1
				UINT_8 TPL1B2; // First TLP Prefix Log Byte 2
				UINT_8 TPL1B3; // First TLP Prefix Log Byte 3

				std::string toString() const;
			} PCI_AER_TLP_PREFIX_LOG_REGISTER, *PPCI_AER_TLP_PREFIX_LOG_REGISTER;
			static_assert(sizeof(PCI_AER_TLP_PREFIX_LOG_REGISTER) == 16, "AERTLP should be 16 byte(s) in size.");


			/*! This is the PCI AER Extended Capability structure (NVMe 1.2.1 section 2.6)
			* @info This has to be at offset 0x100 here as it is the only extended capability.
			*/
			typedef struct PCI_ADVANCED_ERROR_REPORTING_CAPABILITY
			{
				PCI_AER_CAPABILITY_ID AERID;
				PCI_AER_UNCORRECTABLE_ERROR_STATUS_REGISTER AERUCES;
				PCI_AER_UNCORRECTABLE_ERROR_MASK_REGISTER AERUCEM;
				PCI_AER_UNCORRECTABLE_ERROR_SEVERITY_REGISTER AERUCESEV;
				PCI_AER_CORRECTABLE_ERROR_STATUS_REGISTER AERCS;
				PCI_AER_CORRECTABLE_ERROR_MASK_REGISTER	AERCEM;
				PCI_AER_CAPABILITIES_AND_CONTROL_REGISTER AERCC;
				UINT_8 RSVD0[12];
				PCI_AER_HEADER_LOG_REGISTER AERHL;
				PCI_AER_TLP_PREFIX_LOG_REGISTER AERTLP;

				std::string toString() const;
			}PCI_ADVANCED_ERROR_REPORTING_CAPABILITY, *PPCI_ADVANCED_ERROR_REPORTING_CAPABILITY;
			static_assert(sizeof(PCI_ADVANCED_ERROR_REPORTING_CAPABILITY) == 72, "AERCAP should be 72 byte(s) in size.");
		}

		/// <summary>
		/// This is a class for Section 2 of the NVMe 1.2.1 Spec. (System Bus (PCI Express) Registers)
		/// </summary>
		class PCIExpressRegisters
		{
		public:
			/// <summary>
			/// Plain constructor
			/// </summary>
			PCIExpressRegisters();

			/// <summary>
			/// Destructor
			/// </summary>
			~PCIExpressRegisters() = default;

			/// <summary>
			/// Sets the PciHeader back to defaults
			/// </summary>
			void resetPciHeader();

			/// <summary>
			/// Allocates all 6 BARs
			/// </summary>
			void allocateBars();

			/// <summary>
			/// Allocates all PCI capability registers
			/// </summary>
			void allocateCapabilities();

			/// <summary>
			/// Gets a copy of the header and capabilities payload
			/// </summary>
			/// <returns>A copy of PciHeaderAndCapabilities</returns>
			cnvme::Payload readHeaderAndCapabilities();

			/// <summary>
			/// Updates the header and capabilities payload with the one given
			/// </summary>
			/// <param name="payload">Payload of header and capability data</param>
			void writeHeaderAndCapabilities(const cnvme::Payload &payload);

		private:
			/// <summary>
			/// The private implementation of the BAR memory.
			/// The user can only see it via the addresses in the Header structure
			/// </summary>
			cnvme::Payload Bars;

			/// <summary>
			/// The private implementation of the Header and Capabilities memory
			/// The user can only see it via the addresses in the Header structure
			/// </summary>
			cnvme::Payload PciHeaderAndCapabilities;

			/// <summary>
			/// Gets a pointer to the Pci Header
			/// This is private since it is raw access.
			///     If something was modified, we couldn't 'fire an interrupt'
			/// </summary>
			/// <returns>PCI Header Pointer</returns>
			cnvme::pci::header::PCI_HEADER* getPciHeader();

		};
	}
}
