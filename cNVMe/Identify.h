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
Identify.h - A header file for the NVMe Identify Commands
*/

#include "Types.h"

// Defaults
#define DEFAULT_MODEL "CNVMe Model Number"
#define DEFAULT_SERIAL "CNVMe Serial Number"
#define DEFAULT_FIRMWARE "00000001"
#define DEFAULT_SUBMISSION_QUEUE_ENTRY_SIZE 6 // 2^6 = 64
#define DEFAULT_COMPLETION_QUEUE_ENTRY_SIZE 4 // 2^4 = 16
#define DEFAULT_MAX_NAMESPACES 1

#pragma once

namespace cnvme
{
	namespace identify
	{
		namespace structures
		{
			typedef struct IDENTIFY_POWER_STATE_DESCRIPTOR {
				UINT_16 MP;
				UINT_8 RSVD_16_23;
				UINT_8 MXPS : 1;
				UINT_8 NOPS : 1;
				UINT_8 RSVD_26_31 : 6;
				UINT_32 ENLAT;
				UINT_32 EXLAT;
				UINT_8 RRT : 5;
				UINT_8 RSVD_101_103 : 3;
				UINT_8 RRL : 5;
				UINT_8 RSVD_109_111 : 3;
				UINT_8 RWT : 5;
				UINT_8 RSVD_117_119 : 3;
				UINT_8 RWL : 5;
				UINT_8 RSVD_125_127 : 3;
				UINT_16 IDLP;
				UINT_8 RSVD_144_149 : 6;
				UINT_8 IPS : 2;
				UINT_8 RSVD_152_159;
				UINT_16 ACTP;
				UINT_8 APW : 3;
				UINT_8 RSVD_179_181 : 3;
				UINT_8 APS : 2;
				UINT_8 RSVD_184_255[9];
			} IDENTIFY_POWER_STATE_DESCRIPTOR, *PIDENTIFY_POWER_STATE_DESCRIPTOR;
			static_assert(sizeof(IDENTIFY_POWER_STATE_DESCRIPTOR) == 32, "The identify power state descriptor is 32 bytes.");

			typedef struct IDENTIFY_CONTROLLER {
				UINT_16 VID;
				UINT_16 SSID;
				char SN[20];
				char MN[40];
				char FR[8];
				UINT_8 RAB;
				UINT_32 IEEE : 24;
				UINT_32 CMIC : 8;
				UINT_8 MDTS;
				UINT_16 CNTLID;
				union {
					struct {
						UINT_32 MJR : 16;
						UINT_32 MNR : 8;
						UINT_32 TER : 8;
					};
					UINT_32 VER;
				};
				UINT_32 RTD3R;
				UINT_32 RTD3E;
				UINT_32 OAES;
				UINT_32 CTRATT;
				UINT_8 RSVD_100_111[12];
				UINT_8 FGUID[16];
				UINT_8 RSVD_128_239[112];
				UINT_8 RSVD_240_255_MI[16];
				union {
					struct {
						UINT_16 SecuritySupported : 1;
						UINT_16 FormatNVMSupported : 1;
						UINT_16 FirmwareDownloadAndCommitSupported : 1;
						UINT_16 NamespaceCommandsSupported : 1;
						UINT_16 DeviceSelfTestSupported : 1;
						UINT_16 DirectivesSupported : 1;
						UINT_16 NVMeMiSendAndReceiveSupported : 1;
						UINT_16 VirtualizationManagementSupported : 1;
						UINT_16 DoorbellBufferConfigSupported : 1;
						UINT_16 RSVD_OACS : 7;
					};
					UINT_16 OACS;
				};
				UINT_8 ACL;
				UINT_8 AERL;
				union {
					struct {
						UINT_8 FirstFirmwareSlotIsReadOnly : 1;
						UINT_8 NumberOfFirmwareSlots : 3;
						UINT_8 FirmwareActivationWithoutResetSupported : 1;
						UINT_8 RSVD_FRMW : 3;
					};
					UINT_8 FRMW;
				};
				union {
					struct {
						UINT_8 SMARTHealthLogOnNamespaceBasisSupported : 1;
						UINT_8 CommandsEffectsLogSupported : 1;
						UINT_8 ExtendedDataForGetLogPageSupported : 1;
						UINT_8 TelemetryLogsSupported : 1;
						UINT_8 RSVD_LPA : 4;
					};
					UINT_8 LPA;
				};
				UINT_8 ELPE;
				UINT_8 NPSS;
				UINT_8 AVSCC;
				UINT_8 APSTA;
				UINT_16 WCTEMP;
				UINT_16 CCTEMP;
				UINT_16 MTFA;
				UINT_32 HMPRE;
				UINT_32 HMMIN;
				UINT_8 TNVMCAP[16];
				UINT_8 UNVMCAP[16];
				UINT_32 RPMBS;
				UINT_16 EDSTT;
				UINT_8 DSTO;
				UINT_8 FWUG;
				UINT_16 KAS;
				UINT_16 HCTMA;
				UINT_16 MNTMT;
				UINT_16 MXTMT;
				union {
					struct {
						UINT_32 CryptoEraseSanitizeSupported : 1;
						UINT_32 BlockEraseSanitizeSupported : 1;
						UINT_32 OverwriteSanitizeSupported : 1;
						UINT_32 RSVD_SANICAP : 29;
					};
					UINT_32 SANICAP;
				};
				UINT_8 RSVD_332_511[180];
				union {
					struct {
						UINT_8 MaxSubmissionQueueEntrySize : 4;
						UINT_8 RequiredSubmissionQueueEntrySize : 4;
					};
					UINT_8 SQES;
				};
				union {
					struct {
						UINT_8 MaxCompletionQueueEntrySize : 4;
						UINT_8 RequiredCompletionQueueEntrySize : 4;
					};
					UINT_8 CQES;
				};
				UINT_16 MAXCMD;
				UINT_32 NN;
				union {
					struct {
						UINT_16 CompareSupported : 1;
						UINT_16 WriteUncorrectableSupported : 1;
						UINT_16 DatasetManagementSupported : 1;
						UINT_16 WriteZeroesSupported : 1;
						UINT_16 SaveFieldOfSetFeaturesSupported : 1;
						UINT_16 ReservationsSupported : 1;
						UINT_16 TimestampFeatureSupported : 1;
						UINT_16 RSVD_ONCS : 9;
					};
					UINT_16 ONCS;
				};
				UINT_16 FUSES;
				union {
					struct {
						UINT_8 FormatAppliesToAllNamespaces : 1;
						UINT_8 AllNamespacesErasedOnSecureErase : 1;
						UINT_8 CryptoEraseSupportedAsPartOfSecureErase : 1;
						UINT_8 RSVD_FNA : 5;
					};
					UINT_8 FNA;
				};
				UINT_8 VWC;
				UINT_16 AWUN;
				UINT_16 AWUPF;
				UINT_8 NVSCC;
				UINT_8 RSVD_531;
				UINT_16 ACWU;
				UINT_16 RSVD_534_535;
				UINT_32 SGLS;
				UINT_8 RSVD_540_767[228];
				char SUBNQN[256];
				UINT_8 RSVD_1024_1791[768];
				UINT_8 RSVD_NVME_OVER_FABRICS[256];
				IDENTIFY_POWER_STATE_DESCRIPTOR PSD[32];
				UINT_8 VendorSpecific[1024];
			} IDENTIFY_CONTROLLER, *PIDENTIFY_CONTROLLER;
			static_assert(sizeof(IDENTIFY_CONTROLLER) == 4096, "Identify controller should be 4096 bytes in size");

			typedef struct LBA_FORMAT {
				UINT_16 MS;
				UINT_8 LBADS;
				UINT_8 RP : 2;
				UINT_8 RSVD : 6;
			} LBA_FORMAT, *PLBA_FORMAT;
			static_assert(sizeof(LBA_FORMAT) == 4, "LBA Formats are 4 bytes in size");

			typedef struct IDENTIFY_NAMESPACE {
				UINT_64 NSZE;
				UINT_64 NCAP;
				UINT_64 NUSE;
				UINT_8 NSFEAT;
				UINT_8 NLBAF;
				UINT_8 FLBAS;
				UINT_8 MC;
				UINT_8 DPC;
				UINT_8 DPS;
				struct {
					UINT_8 NamespaceMayBeAttachedToMoreThanOneController : 1;
					UINT_8 RSVD_NMIC : 7;
				} NMIC;
				UINT_8 RESCAP;
				UINT_8 FPI;
				UINT_8 DLFEAT;
				UINT_16 NAWUN;
				UINT_16 NAWUPF;
				UINT_16 NACWU;
				UINT_16 NABSN;
				UINT_16 NABO;
				UINT_16 NABSPF;
				UINT_16 NOIOB;
				struct {
					UINT_64 NVMCAP_64[2]; // 128 bits
				} NVMCAP;
				UINT_8 RSVD_103_64[40];
				char NGUID[16];
				UINT_64 EUI64; // big endian
				LBA_FORMAT LBAF[16];
				UINT_8 RSVD_192_383[192];
				UINT_8 VendorSpecific[3712];
			} IDENTIFY_NAMESPACE, *PIDENTIFY_NAMESPACE;
			static_assert(sizeof(IDENTIFY_NAMESPACE) == 4096, "Identify Namespace should be 4096 bytes in size");
		}
	}
}
