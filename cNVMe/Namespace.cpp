/*
###########################################################################################
// cNVMe - An Open Source NVMe Device Simulation - MIT License
// Copyright 2018 - Intel Corporation

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
Namespace.cpp - An implementation file for a cNVMe Namespace
*/

#include "Constants.h"
#include "Namespace.h"
#include "PRP.h"
#include "Tests.h"

#define DEFAULT_NUMBER_OF_LBA_FORMAT 2; // 0-based!
#define IEEE_OUI 0xCCAACC
#define LBA_IN_BYTES_TO_LBADS(lbaSizeInBytes) ((UINT_8)(log2(lbaSizeInBytes)))
#define GET_RANDOM_BYTE() (UINT_8)(rand() % ((UINT_8)-1)) // lol not really random. Make sure to call srand above this somewhere.

namespace cnvme
{
	namespace ns
	{
		Namespace::Namespace()
		{
			memset(&this->IdentifyNamespace, 0, sizeof(this->IdentifyNamespace));
			this->getIdentifyNamespaceStructure(); // make sure we are setup.
		}

		Namespace::Namespace(size_t SizeInBytes) : Namespace()
		{
			Media = Payload(SizeInBytes);
		}

		Namespace::~Namespace()
		{
			// nothing to do yet.
		}

		identify::structures::IDENTIFY_NAMESPACE& Namespace::getIdentifyNamespaceStructure()
		{
			// Assume this is the first call if NLBAF is 0.
			// That way we generate the NGUID once.
			if (this->IdentifyNamespace.NLBAF == 0)
			{
				srand((unsigned int)time(NULL)); // 'random' seed

				for (size_t i = 0; i < sizeof(this->IdentifyNamespace.NGUID.VSEI); i++)
				{
					if (i < sizeof(this->IdentifyNamespace.NGUID.EI))
					{
						this->IdentifyNamespace.NGUID.EI[i] = GET_RANDOM_BYTE();
					}
					if (i < sizeof(this->IdentifyNamespace.NGUID.OUI))
					{
						this->IdentifyNamespace.NGUID.OUI[i] = (IEEE_OUI >> (i * 8)) & 0xFF;
					}
					this->IdentifyNamespace.NGUID.VSEI[i] = GET_RANDOM_BYTE();
				}
			}
			this->IdentifyNamespace.NamespaceGUIDAndEUI64AreNotRepeated = 1;     // Will try hard not to repeat NGUID

			this->IdentifyNamespace.NLBAF = DEFAULT_NUMBER_OF_LBA_FORMAT;        // support 512/4096/8192 byte sectors
			this->IdentifyNamespace.LBAF[0].LBADS = LBA_IN_BYTES_TO_LBADS(512);
			this->IdentifyNamespace.LBAF[1].LBADS = LBA_IN_BYTES_TO_LBADS(4096);
			this->IdentifyNamespace.LBAF[2].LBADS = LBA_IN_BYTES_TO_LBADS(8192);

			auto currentLbaFormat = this->IdentifyNamespace.LBAF[this->IdentifyNamespace.FLBAS.CurrentLBAFormat];
			ASSERT_IF(currentLbaFormat.LBADS < 9, "Minimum posssible selected LBADS should be 9. 2^9 = 512.");

			this->IdentifyNamespace.NSZE = this->getNamespaceSizeInSectors();
			this->IdentifyNamespace.NCAP = this->IdentifyNamespace.NSZE; // todo: dealloacted lbas should subtract from this one day.
			this->IdentifyNamespace.NUSE = this->IdentifyNamespace.NSZE;

			this->IdentifyNamespace.NMIC.NamespaceMayBeAttachedToMoreThanOneController = 1;

			this->IdentifyNamespace.NVMCAP.NVMCAP_64[0] = this->Media.getSize(); // When we need more terabytes... let me know.

			return this->IdentifyNamespace;
		}

		Payload Namespace::getIdentifyNamespaceDescriptorList()
		{
			Payload payload(sizeof(identify::structures::NAMESPACE_IDENTIFICATION_DESCRIPTOR_NGUID));

			ASSERT_IF(payload.getSize() > 4096, "Per NVMe spec this cannot ever be larger than 4096 bytes");

			auto pList = (identify::structures::NAMESPACE_IDENTIFICATION_DESCRIPTOR_NGUID*)payload.getBuffer();

			pList->NIDT = constants::commands::identify::ns_identifiers::NGUID;
			pList->NIDL = constants::commands::identify::sizes::NGUID_SIZE;
			pList->NGUID = this->getIdentifyNamespaceStructure().NGUID;

			return payload;
		}

		command::COMPLETION_QUEUE_ENTRY Namespace::formatNVM(command::NVME_COMMAND nvmeCommand)
		{
			command::COMPLETION_QUEUE_ENTRY completionQueueEntry = { 0 };

			// we don't support metadata. We also should block invalid lba formats
			if (nvmeCommand.DW10_Format.MSET || nvmeCommand.DW10_Format.PI || nvmeCommand.DW10_Format.PIL || nvmeCommand.DW10_Format.LBAF > this->IdentifyNamespace.NLBAF)
			{
				completionQueueEntry.DNR = true;
				completionQueueEntry.SCT = constants::status::types::COMMAND_SPECIFIC;
				completionQueueEntry.SC = constants::status::codes::specific::INVALID_FORMAT;
				return completionQueueEntry;
			}

			// update or current lba format
			this->IdentifyNamespace.FLBAS.CurrentLBAFormat = nvmeCommand.DW10_Format.LBAF;

			// delete the 'key'... in our case delete the Media and recreate it.
			if (nvmeCommand.DW10_Format.SES == constants::commands::format::ses::CRYPTOGRAPHIC_ERASE)
			{
				LOG_INFO("Performing a crypto erase");
				this->Media = Payload(Media.getSize());
			}
			else if (nvmeCommand.DW10_Format.SES == constants::commands::format::ses::USER_DATA_ERASE)
			{
				LOG_INFO("Performing a user data erase");
				// Per NVMe spec the controller can do whatever here as long as the data is gone... so we shall do some randomization.
				for (size_t i = 0; i < 8; i++) // 8 is a number i picked without much reason.
				{
					tests::helpers::randomizePayload(this->Media);
				}
			}
			else
			{
				LOG_INFO("Performing a non-secure erase");
				memset(this->Media.getBuffer(), 0x00, this->Media.getSize());
			}

			return completionQueueEntry;
		}

		command::COMPLETION_QUEUE_ENTRY Namespace::read(command::NVME_COMMAND nvmeCommand, Payload &outputPayload)
		{
			command::COMPLETION_QUEUE_ENTRY completionQueueEntry = { 0 };

			// Assume metadata is not supported.

			UINT_64 namespaceSizeInSectors = this->getNamespaceSizeInSectors();

			// Make sure the LBA is in range
			if (nvmeCommand.SLBA > namespaceSizeInSectors || nvmeCommand.SLBA + ONE_BASED_FROM_ZERO_BASED(nvmeCommand.DW12_IO.NLB) > namespaceSizeInSectors)
			{
				completionQueueEntry.DNR = true;
				completionQueueEntry.SCT = constants::status::types::GENERIC_COMMAND;
				completionQueueEntry.SC = constants::status::codes::generic::LBA_OUT_OF_RANGE;
				return completionQueueEntry;
			}

			UINT_64 transferSize = this->getSectorSize() * ONE_BASED_FROM_ZERO_BASED(nvmeCommand.DW12_IO.NLB);
			UINT_64 byteOffset = this->getSectorSize() * nvmeCommand.SLBA;

			// Give data back
			outputPayload = Payload(this->Media.getBuffer() + byteOffset, (size_t)transferSize);

			return completionQueueEntry;
		}

		command::COMPLETION_QUEUE_ENTRY Namespace::write(command::NVME_COMMAND nvmeCommand, UINT_32 memoryPageSize)
		{
			command::COMPLETION_QUEUE_ENTRY completionQueueEntry = { 0 };

			// Assume metadata is not supported.

			UINT_64 namespaceSizeInSectors = this->getNamespaceSizeInSectors();

			// Make sure the LBA is in range
			if (nvmeCommand.SLBA > namespaceSizeInSectors || nvmeCommand.SLBA + ONE_BASED_FROM_ZERO_BASED(nvmeCommand.DW12_IO.NLB) > namespaceSizeInSectors)
			{
				completionQueueEntry.DNR = true;
				completionQueueEntry.SCT = constants::status::types::GENERIC_COMMAND;
				completionQueueEntry.SC = constants::status::codes::generic::LBA_OUT_OF_RANGE;
				return completionQueueEntry;
			}

			UINT_64 transferSize = this->getSectorSize() * ONE_BASED_FROM_ZERO_BASED(nvmeCommand.DW12_IO.NLB);
			UINT_64 byteOffset = this->getSectorSize() * nvmeCommand.SLBA;

			// Get data from PRPs
			PRP prps(nvmeCommand.DPTR.DPTR1, nvmeCommand.DPTR.DPTR2, (size_t)transferSize, memoryPageSize);
			auto inputPayload = prps.getPayloadCopy();

			// Give data back
			memcpy_s(this->Media.getBuffer() + byteOffset, (size_t)(this->Media.getSize() - byteOffset), inputPayload.getBuffer(), (size_t)transferSize);

			return completionQueueEntry;
		}

		identify::structures::IDENTIFY_NAMESPACE Namespace::getGenericIdentifyNamespaceStructure()
		{
			Namespace tmp;
			auto refIdentifyNamespace = tmp.getIdentifyNamespaceStructure();
			identify::structures::IDENTIFY_NAMESPACE retIdentifyNamespace = { 0 };

			// Copy relevant things from the reference structure to the return one
			memcpy_s(&retIdentifyNamespace.LBAF, sizeof(retIdentifyNamespace.LBAF), &refIdentifyNamespace.LBAF, sizeof(refIdentifyNamespace.LBAF));
			retIdentifyNamespace.NLBAF = refIdentifyNamespace.NLBAF;

			return retIdentifyNamespace;
		}

		UINT_64 Namespace::getNamespaceSizeInSectors()
		{
			UINT_32 sectorSize = this->getSectorSize();
			ASSERT_IF(this->Media.getSize() % sectorSize != 0, "The media's size needs to be divisible by the namespace sector size");

			UINT_64 namespaceSizeInSectors = this->Media.getSize() / sectorSize;
			return namespaceSizeInSectors;
		}

		UINT_32 Namespace::getSectorSize()
		{
			UINT_32 sectorSize = (UINT_32)std::pow(2, this->IdentifyNamespace.LBAF[this->IdentifyNamespace.FLBAS.CurrentLBAFormat].LBADS);
			ASSERT_IF(sectorSize < 512, "Sector size shouldn't be less than 512!");
			return sectorSize;
		}
	}
}