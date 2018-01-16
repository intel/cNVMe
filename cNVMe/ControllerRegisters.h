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
ControllerRegisters.h - A header file for the Controller Registers
*/

#pragma once

#include "LoopingThread.h"
#include "Types.h"

namespace cnvme
{
	namespace controller
	{
		class Controller;

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

			typedef struct VERSION
			{
				UINT_8 TER; // Tertiary Version Number
				UINT_8 MNR; // Minor Version Number
				UINT_16 MJR; // Major Version Number

				std::string toString() const;
			}VERSION, *PVERSION;
			static_assert(sizeof(VERSION) == 4, "VS should be 4 byte(s) in size.");

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

			typedef struct QUEUE_DOORBELLS
			{
				SUBMISSION_QUEUE_Y_TAIL_DOORBELL SQTDBL;
				COMPLETION_QUEUE_Y_HEAD_DOORBELL CQHDBL;

				std::string toString() const;
			}QUEUE_DOORBELLS, *PQUEUE_DOORBELLS;
			static_assert(sizeof(QUEUE_DOORBELLS) == 8, "QD should be 8 byte(s) in size.");

			/// <summary>
			/// Controller Registers from section 3 of the NVMe 1.2.1 spec
			/// These are located in BAR0/BAR1 of memory
			/// </summary>
			typedef struct CONTROLLER_REGISTERS
			{
				CONTROLLER_CAPABILITIES CAP;
				VERSION VS;
				INTERRUPT_MASK_SET INTMS;
				INTERRUPT_MASK_CLEAR INTMC;
				CONTROLLER_CONFIGURATION CC;
				UINT_8 RSVD0[4];
				CONTROLLER_STATUS CSTS;
				NVM_SUBSYSTEM_RESET NSSR;
				ADMIN_QUEUE_ATTRIBUTES AQA;
				ADMIN_SUBMISSION_QUEUE_BASE_ADDRESS ASQ;
				ADMIN_COMPLETION_QUEUE_BASE_ADDRESS ACQ;
				CONTROLLER_MEMORY_BUFFER_LOCATION CMBLOC;
				CONTROLLER_MEMORY_BUFFER_SIZE CMBSZ;
				UINT_8 RSVD1[3776];
				UINT_8 CSS[256]; // Command Set Specific (RSVD)

				std::string toString() const;
			}CONTROLLER_REGISTERS, *PCONTROLLER_REGISTERS;
			static_assert(sizeof(CONTROLLER_REGISTERS) == 4096, "CR should be 4096 byte(s) in size.");

			class ControllerRegisters
			{
			public:
				/// <summary>
				/// Base constructor
				/// </summary>
				ControllerRegisters();

				/// <summary>
				/// Place these registers at the memoryLocation 
				/// </summary>
				/// <param name="memoryLocation">Location of BAR0</param>
				ControllerRegisters(UINT_64 memoryLocation);

				/// <summary>
				/// Place the registers at the memory location, though also give a pointer to the controller to make callbacks for resets
				/// </summary>
				/// <param name="memoryLocation">Location of BAR0</param>
				/// <param name="controller">Poi8nter to the controller</param>
				ControllerRegisters(UINT_64 memoryLocation, cnvme::controller::Controller* controller);

				/// <summary>
				/// Destructor
				/// </summary>
				~ControllerRegisters();

				/// <summary>
				/// Returns the actual controller registers
				/// </summary>
				/// <returns>A pointer to the controller registers</returns>
				CONTROLLER_REGISTERS* getControllerRegisters();

				/// <summary>
				/// Returns a pointer to the queue doorbells
				/// </summary>
				/// <returns>Queue doorbells pointer</returns>
				QUEUE_DOORBELLS* getQueueDoorbells();

				/// <summary>
				/// Wait for an iteration of the interrupt loop
				/// </summary>
				void waitForChangeLoop();

				/// <summary>
				/// Gets the memory page size via CC.MPS
				/// </summary>
				/// <returns>Memory Page Size</returns>
				UINT_32 getMemoryPageSize();

			private:

				/// <summary>
				/// Pointer to the controller registers (and BAR0 in PCIe NVMe)
				/// </summary>
				CONTROLLER_REGISTERS* ControllerRegistersPointer;

				/// <summary>
				/// Looping thread to watch the registers.
				/// </summary>
				LoopingThread RegisterWatcher;

				/// <summary>
				/// The controller that owns this.
				/// </summary>
				cnvme::controller::Controller* Controller;

				/// <summary>
				/// Used to keep track if a reset was sent via CC.EN == 0
				/// </summary>
				bool controllerResetInitiated;

				/// <summary>
				/// Function to be called in loop looking for changes
				/// </summary>
				void checkForChanges();

				/// <summary>
				/// Does a controller reset;
				/// </summary>
				void controllerReset();
			};
		}

	}
}

