/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
ControllerRegisters.h - A header file for the Controller Registers
*/

#pragma once

#include "Types.h"

namespace cnvme
{
	namespace controller
	{
		namespace registers
		{
			typedef struct CONTROLLER_CAPABILITIES
			{
				UINT_64 MQES : 16; // Maximum Queue Entries Supported
				UINT_64 CQR : 1; // Contiguous Queues Required
				UINT_64 AMS : 2; // Arbitration Mechanism Supported
				UINT_64 RSVD2 : 5; // Reserved
				UINT_64 TO : 8; // Timeout
				UINT_64 DSTRD : 4; // Doorbell Stride
				UINT_64 NSSRS : 1; // NVM Subsystem Reset Supported
				UINT_64 CSS : 8; // Command Sets Supported
				UINT_64 RSVD1 : 3; // Reserved
				UINT_64 MPSMIN : 4; // Memory Page Size Minimum
				UINT_64 MPSMAX : 4; // Memory Page Size Maximum
				UINT_64 RSVD0 : 8; // Reserved

				std::string toString() const;
			} CONTROLLER_CAPABILITIES, *PCONTROLLER_CAPABILITIES;
			static_assert(sizeof(CONTROLLER_CAPABILITIES) == 8, "CAP should be 8 byte(s) in size.");

			typedef struct Version
			{
				UINT_8 TER; // Tertiary Version Number
				UINT_8 MNR; // Minor Version Number
				UINT_16 MJR; // Major Version Number

				std::string toString() const;
			}Version, *PVersion;
			static_assert(sizeof(Version) == 4, "VS should be 4 byte(s) in size.");

			typedef struct INTERRUPT_MASK_SET
			{
				UINT_32 IVMS; // Interrupt Vector Mask Set

				std::string toString() const;
			} INTERRUPT_MASK_SET, *PINTERRUPT_MASK_SET;
			static_assert(sizeof(INTERRUPT_MASK_SET) == 4, "INTMS should be 4 byte(s) in size.");

			typedef struct INTERRUPT_MASK_CLEAR
			{
				UINT_32 IVMC; // Interrupt Vector Mask Clear

				std::string toString() const;
			} INTERRUPT_MASK_CLEAR, *PINTERRUPT_MASK_CLEAR;
			static_assert(sizeof(INTERRUPT_MASK_CLEAR) == 4, "INTMC should be 4 byte(s) in size.");

			typedef struct CONTROLLER_CONFIGURATION
			{
				UINT_32 EN : 1; // Enable
				UINT_32 RSVD1 : 3; // Reserved
				UINT_32 CSS : 3; // I/O Command Set Selected
				UINT_32 MPS : 4; // Memory Page Size
				UINT_32 AMS : 3; // Arbitration Mechanism Selected
				UINT_32 SHN : 2; // Shutdown Notification
				UINT_32 IOSQES : 4; // I/O Submission Queue Entry Size
				UINT_32 IOCQES : 4; // I/O Completion Queue Entry Size
				UINT_32 RSVD0 : 8; // Reserved

				std::string toString() const;
			} CONTROLLER_CONFIGURATION, *PCONTROLLER_CONFIGURATION;
			static_assert(sizeof(CONTROLLER_CONFIGURATION) == 4, "CC should be 4 byte(s) in size.");

			typedef struct CONTROLLER_STATUS
			{
				UINT_32 RDY : 1; // Ready
				UINT_32 CFS : 1; // Controller Fatal Status
				UINT_32 SHST : 2; // Shutdown Status
				UINT_32 NSSRO : 1; // NVM Subsystem Reset Occurred
				UINT_32 PP : 1; // Processing Paused
				UINT_32 RSVD0 : 26; // Reserved

				std::string toString() const;
			} CONTROLLER_STATUS, *PCONTROLLER_STATUS;
			static_assert(sizeof(CONTROLLER_STATUS) == 4, "CSTS should be 4 byte(s) in size.");

			typedef struct NVM_SUBSYSTEM_RESET
			{
				UINT_32 NSSRC; // NVM Subsystem Reset Control

				std::string toString() const;
			} NVM_SUBSYSTEM_RESET, *PNVM_SUBSYSTEM_RESET;
			static_assert(sizeof(NVM_SUBSYSTEM_RESET) == 4, "NSSR should be 4 byte(s) in size.");

			typedef struct ADMIN_QUEUE_ATTRIBUTES
			{
				UINT_16 ASQS : 12; // Admin Submission Queue Size
				UINT_16 RSVD1 : 4; // Reserved
				UINT_16 ACQS : 12; // Admin Completion Queue Size
				UINT_16 RSVD0 : 4; // Reserved

				std::string toString() const;
			} ADMIN_QUEUE_ATTRIBUTES, *PADMIN_QUEUE_ATTRIBUTES;
			static_assert(sizeof(ADMIN_QUEUE_ATTRIBUTES) == 4, "AQA should be 4 byte(s) in size.");

			typedef struct ADMIN_SUBMISSION_QUEUE_BASE_ADDRESS
			{
				UINT_64 RSVD0 : 12; // Reserved
				UINT_64 ASQB : 52; // Admin Submission Queue Base

				std::string toString() const;
			} ADMIN_SUBMISSION_QUEUE_BASE_ADDRESS, *PADMIN_SUBMISSION_QUEUE_BASE_ADDRESS;
			static_assert(sizeof(ADMIN_SUBMISSION_QUEUE_BASE_ADDRESS) == 8, "ASQ should be 8 byte(s) in size.");

			typedef struct ADMIN_COMPLETION_QUEUE_BASE_ADDRESS
			{
				UINT_64 RSVD0 : 12; // Reserved
				UINT_64 ACQB : 52; // Admin Completion Queue Base

				std::string toString() const;
			} ADMIN_COMPLETION_QUEUE_BASE_ADDRESS, *PADMIN_COMPLETION_QUEUE_BASE_ADDRESS;
			static_assert(sizeof(ADMIN_COMPLETION_QUEUE_BASE_ADDRESS) == 8, "ACQ should be 8 byte(s) in size.");

			typedef struct CONTROLLER_MEMORY_BUFFER_LOCATION
			{
				UINT_32 BIR : 3; // Base Indicator Register
				UINT_32 RSVD0 : 9; // Reserved
				UINT_32 OFST : 20; // Offset

				std::string toString() const;
			} CONTROLLER_MEMORY_BUFFER_LOCATION, *PCONTROLLER_MEMORY_BUFFER_LOCATION;
			static_assert(sizeof(CONTROLLER_MEMORY_BUFFER_LOCATION) == 4, "CMBLOC should be 4 byte(s) in size.");

			typedef struct CONTROLLER_MEMORY_BUFFER_SIZE
			{
				UINT_32 SQS : 1; // Submission Queue Support
				UINT_32 CQS : 1; // Completion Queue Support
				UINT_32 LISTS : 1; // PRP SGL List Support
				UINT_32 RDS : 1; // Read Data Support
				UINT_32 WDS : 1; // Write Data Support
				UINT_32 RSVD0 : 3; // Reserved
				UINT_32 SZU : 4; // Size Units
				UINT_32 SZ : 20; // Size

				std::string toString() const;
			} CONTROLLER_MEMORY_BUFFER_SIZE, *PCONTROLLER_MEMORY_BUFFER_SIZE;
			static_assert(sizeof(CONTROLLER_MEMORY_BUFFER_SIZE) == 4, "CMBSZ should be 4 byte(s) in size.");

			typedef struct SUBMISSION_QUEUE_Y_TAIL_DOORBELL
			{
				UINT_16 SQT; // Submission Queue Tail
				UINT_16 RSVD0; // Reserved

				std::string toString() const;
			} SUBMISSION_QUEUE_Y_TAIL_DOORBELL, *PSUBMISSION_QUEUE_Y_TAIL_DOORBELL;
			static_assert(sizeof(SUBMISSION_QUEUE_Y_TAIL_DOORBELL) == 4, "SQyTDBL should be 4 byte(s) in size.");

			typedef struct COMPLETION_QUEUE_Y_HEAD_DOORBELL
			{
				UINT_16 CQH; // Completion Queue Head
				UINT_16 RSVD0; // Reserved

				std::string toString() const;
			} COMPLETION_QUEUE_Y_HEAD_DOORBELL, *PCOMPLETION_QUEUE_Y_HEAD_DOORBELL;
			static_assert(sizeof(COMPLETION_QUEUE_Y_HEAD_DOORBELL) == 4, "CQyHDBL should be 4 byte(s) in size.");

		}
	}
}

