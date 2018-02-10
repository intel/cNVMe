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
Constants.h - A header file for the NVMe Constants
*/

#include "Types.h"

#pragma once

namespace cnvme
{
	namespace constants
	{
		namespace opcodes
		{
			namespace admin
			{
				const UINT_8 DELETE_IO_SUBMISSION_QUEUE = 0x00;
				const UINT_8 CREATE_IO_SUBMISSION_QUEUE = 0x01;
				const UINT_8 GET_LOG_PAGE = 0x02;
				const UINT_8 DELETE_IO_COMPLETION_QUEUE = 0x04;
				const UINT_8 CREATE_IO_COMPLETION_QUEUE = 0x05;
				const UINT_8 IDENTIFY = 0x06;
				const UINT_8 ABORT = 0x08;
				const UINT_8 SET_FEATURES = 0x09;
				const UINT_8 GET_FEATURES = 0x0A;
				const UINT_8 ASYNCHRONOUS_EVENT_REQUEST = 0x0C;
				const UINT_8 NAMESPACE_MANAGEMENT = 0x0D;
				const UINT_8 FIRMWARE_COMMIT = 0x10;
				const UINT_8 FIRMWARE_IMAGE_DOWNLOAD = 0x11;
				const UINT_8 DEVICE_SELF_TEST = 0x14;
				const UINT_8 NAMESPACE_ATTACHMENT = 0x15;
				const UINT_8 KEEP_ALIVE = 0x18;
				const UINT_8 DIRECTIVE_SEND = 0x19;
				const UINT_8 DIRECTIVE_RECEIVE = 0x1A;
				const UINT_8 VIRTUALIZATION_MANAGEMENT = 0x1C;
				const UINT_8 NVME_MI_SEND = 0x1D;
				const UINT_8 NVME_MI_RECEIVE = 0x1E;
				const UINT_8 FORMAT_NVM = 0x80;
				const UINT_8 SECURITY_SEND = 0x81;
				const UINT_8 SECURITY_RECEIVE = 0x82;
				const UINT_8 SANITIZE = 0x84;
				const UINT_8 DOORBELL_BUFFER_CONFIG = 0x7C;
			}

			namespace nvm
			{
				const UINT_8 FLUSH = 0x00;
				const UINT_8 WRITE = 0x01;
				const UINT_8 READ = 0x02;
				const UINT_8 WRITE_UNCORRECTABLE = 0x04;
				const UINT_8 COMPARE = 0x05;
				const UINT_8 WRITE_ZEROES = 0x08;
				const UINT_8 DATASET_MANAGEMENT = 0x09;
				const UINT_8 RESERVATION_REGISTER = 0x0D;
				const UINT_8 RESERVATION_REPORT = 0x0E;
				const UINT_8 RESERVATION_ACQUIRE = 0x11;
				const UINT_8 RESERVATION_RELEASE = 0x15;
			}
		}

		namespace status
		{
			namespace types
			{
				const UINT_8 GENERIC_COMMAND = 0x00;
				const UINT_8 COMMAND_SPECIFIC = 0x01;
				const UINT_8 MEDIA_AND_DATA_INTEGRITY = 0x02;
			}

			namespace codes
			{
				namespace generic
				{
					const UINT_8 SUCCESSFUL_COMPLETION = 0x00;
					const UINT_8 INVALID_COMMAND_OPCODE = 0x01;
					const UINT_8 INVALID_FIELD_IN_COMMAND = 0x02;
					const UINT_8 COMMAND_ID_CONFLICT = 0x03;
					const UINT_8 DATA_TRANSFER_ERROR = 0x04;
					const UINT_8 COMMAND_ABORTED_DUE_TO_POWER_LOSS_NOTIFICATION = 0x05;
					const UINT_8 INTERNAL_ERROR = 0x06;
					const UINT_8 COMMAND_ABORT_REQUESTED = 0x07;
					const UINT_8 COMMAND_ABORTED_DUE_TO_SQ_DELETION = 0x08;
					const UINT_8 COMMAND_ABORTED_DUE_TO_FAILED_FUSED_COMMAND = 0x09;
					const UINT_8 COMMAND_ABORTED_DUE_TO_MISSING_FUSED_COMMAND = 0x0A;
					const UINT_8 INVALID_NAMESPACE_OR_FORMAT = 0x0B;
					const UINT_8 COMMAND_SEQUENCE_ERROR = 0x0C;
					const UINT_8 INVALID_SGL_SEGMENT_DESCRIPTOR = 0x0D;
					const UINT_8 INVALID_NUMBER_OF_SGL_DESCRIPTORS = 0x0E;
					const UINT_8 DATA_SGL_LENGTH_INVALID = 0x0F;
					const UINT_8 METADATA_SGL_LENGTH_INVALID = 0x10;
					const UINT_8 SGL_DESCRIPTOR_TYPE_INVALID = 0x11;
					const UINT_8 INVALID_USE_CONTROLLER_MEMORY_BUFFER = 0x12;
					const UINT_8 PRP_OFFSET_INVALID = 0x13;
					const UINT_8 ATOMIC_WRITE_UNIT_EXCEEDED = 0x14;
					const UINT_8 SGL_OFFSET_INVALID = 0x16;
					const UINT_8 SGL_SUB_TYPE_INVALID = 0x17;
					const UINT_8 HOST_IDENTIFIER_INCONSISTENT_FORMAT = 0x18;
					const UINT_8 KEEP_ALIVE_TIMEOUT_EXPIRED = 0x19;
					const UINT_8 KEEP_ALIVE_TIMEOUT_INVALID = 0x1A;

					// NVM Specific
					const UINT_8 LBA_OUT_OF_RANGE = 0x80;
					const UINT_8 CAPACITY_EXCEEDED = 0x81;
					const UINT_8 NAMESPACE_NOT_READY = 0x82;
					const UINT_8 RESERVATION_CONFLICT = 0x83;
					const UINT_8 FORMAT_IN_PROGRESS = 0x84;
				}

				namespace specific
				{
					const UINT_8 COMPLETION_QUEUE_INVALID = 0x00;
					const UINT_8 INVALID_QUEUE_IDENTIFIER = 0x01;
					const UINT_8 INVALID_QUEUE_SIZE = 0x02;
					const UINT_8 ABORT_COMMAND_LIMIT_EXCEEDED = 0x03;
					const UINT_8 ASYNCHRONOUS_EVENT_REQUEST_LIMIT_EXCEEDED = 0x05;
					const UINT_8 INVALID_FIRMWARE_SLOT = 0x06;
					const UINT_8 INVALID_FIRMWARE_IMAGE = 0x07;
					const UINT_8 INVALID_INTERRUPT_VECTOR = 0x08;
					const UINT_8 INVALID_LOG_PAGE = 0x09;
					const UINT_8 INVALID_FORMAT = 0x0A;
					const UINT_8 FIRMWARE_ACTIVATION_REQUIRES_CONVENTIONAL_RESET = 0x0B;
					const UINT_8 INVALID_QUEUE_DELETION = 0x0C;
					const UINT_8 FEATURE_IDENTIFIER_NOT_SAVEABLE = 0x0D;
					const UINT_8 FEATURE_NOT_CHANGEABLE = 0x0E;
					const UINT_8 FEATURE_NOT_NAMESPACE_SPECIFIC = 0x0F;
					const UINT_8 FIRMWARE_ACTIVATION_REQUIRES_NVM_SUBSYSTEM_RESET = 0x10;
					const UINT_8 FIRMWARE_ACTIVATION_REQUIRES_RESET = 0x11;
					const UINT_8 FIRMWARE_ACTIVATION_REQUIRES_MAXIMUM_TIME_VIOLATION = 0x12;
					const UINT_8 FIRMWARE_ACTIVATION_PROHIBITED = 0x13;
					const UINT_8 OVERLAPPING_RANGE = 0x14;
					const UINT_8 NAMESPACE_INSUFFICIENT_CAPACITY = 0x15;
					const UINT_8 NAMESPACE_IDENTIFIER_UNAVAILABLE = 0x16;
					const UINT_8 NAMESPACE_ALREADY_ATTACHED = 0x18;
					const UINT_8 NAMESPACE_IS_PRIVATE = 0x19;
					const UINT_8 NAMESPACE_NOT_ATTACHED = 0x1A;
					const UINT_8 THIN_PROVISIONING_NOT_SUPPORTED = 0x1B;
					const UINT_8 CONTROLLER_LIST_INVALID = 0x1C;

					// NVM Specific
					const UINT_8 CONFLICTING_ATTRIBUTES = 0x80;
					const UINT_8 INVALID_PROTECTION_INFORMATION = 0x81;
					const UINT_8 ATTEMPTED_WRITE_TO_READ_ONLY_RANGE = 0x82;
				}

				namespace integrity
				{
					// NVM Specific
					const UINT_8 WRITE_FAULT = 0x80;
					const UINT_8 UNRECOVERED_READ_ERROR = 0x81;
					const UINT_8 END_TO_END_GUARD_CHECK_ERROR = 0x82;
					const UINT_8 END_TO_END_APPLICATION_TAG_CHECK_ERROR = 0x83;
					const UINT_8 END_TO_END_REFERENCE_TAG_CHECK_ERROR = 0x84;
					const UINT_8 COMPARE_FAILURE = 0x85;
					const UINT_8 ACCESS_DENIED = 0x86;
					const UINT_8 DEALLOCATED_OR_UNWRITTEN_LOGICAL_BLOCK = 0x87;
				}
			}
		}

		namespace commands
		{
			namespace identify
			{
				namespace cns
				{
					const UINT_8 NAMESPACE_ACTIVE = 0x00;
					const UINT_8 CONTROLLER = 0x01;
					const UINT_8 NAMESPACES_ACTIVE = 0x02;
					const UINT_8 NAMESPACE_DESCRIPTOR = 0x03;
					const UINT_8 NAMESPACES_ALL = 0x10;
					const UINT_8 NAMESPACES_ALLOCATED = 0x11;
					const UINT_8 CONTROLLERS_ATTACHED_TO_NAMESPACE = 0x12;
					const UINT_8 CONTROLLERS_ALL = 0x13;
					const UINT_8 PRIMARY_CONTROLLER_CAPABILITIES = 0x14;
					const UINT_8 SECONDARY_CONTROLLER_LIST = 0x15;
				}

				namespace sizes
				{
					const UINT_32 IDENTIFY_SIZE = 4096;
					const UINT_32 MAX_NSID_IN_NAMESPACE_LIST = 1024;
					const UINT_32 NGUID_SIZE = 16;
					const UINT_8 MAX_FW_SLOTS = 7;
				}

				namespace ns_identifiers
				{
					const UINT_32 IEEE_EXTENDED = 0x01;
					const UINT_32 NGUID = 0x02;
					const UINT_32 NAMESPACE_UUID = 0x03;
				}

				const std::string EMPTY_NQN = "nqn.2014-08.org.nvmexpress:uuid:        -    -    -    -            ";
			}

			namespace fw_commit
			{
				namespace commit_action
				{
					const UINT_8 REPLACE_IN_SLOT_NO_ACTIVATE = 0b000;
					const UINT_8 REPLACE_IN_SLOT_AND_ACTIVATE_ON_RESET = 0b001;
					const UINT_8 ACTIVATE_GIVEN_SLOT_ON_RESET = 0b010;
					const UINT_8 ACTIVATE_GIVEN_SLOT_NOW = 0b011;
					const UINT_8 REPLACE_BOOT_PARTITION = 0b110;
					const UINT_8 ACTIVATE_BOOT_PARTITION = 0b111;
				}
			}

			namespace fw_download
			{
				namespace fwug
				{
					const UINT_8 NO_INFO = 0x00;
					const UINT_8 NO_RESTRICTION = 0xFF;
					const UINT_32 FW_UPDATE_GRANULARITY_UNIT = 4096; // bytes
				}
			}

			namespace format
			{
				namespace ses
				{
					const UINT_32 NO_SECURE_ERASE = 0b000;
					const UINT_32 USER_DATA_ERASE = 0b001;
					const UINT_32 CRYPTOGRAPHIC_ERASE = 0b010;
				}
			}

			namespace ns_attachment
			{
				namespace sizes
				{
					const UINT_32 CONTROLLER_LIST_SIZE = 1024;
				}
			}
		}
	
		namespace crapi
		{
			const UINT_8 CRAPI_HANDLED = 0;
			const UINT_8 CNVME_HANDLED = 1;
			const UINT_8 ASSERT_FROM_CRAPI = 9;
		}
	}
}