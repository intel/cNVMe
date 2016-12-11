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

		}
	}
}
