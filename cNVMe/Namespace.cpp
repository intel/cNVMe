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

#define DEFAULT_NUMBER_OF_LBA_FORMAT 2; // 0-based!

#include <ctgmath>

#include "Constants.h"
#include "Namespace.h"

namespace cnvme
{
	namespace ns
	{
		Namespace::Namespace()
		{
			memset(&this->IdentifyNamespace, 0, sizeof(this->IdentifyNamespace));
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
			this->IdentifyNamespace.NLBAF = DEFAULT_NUMBER_OF_LBA_FORMAT; // support 512/4096/8192 byte sectors
			this->IdentifyNamespace.LBAF[0].LBADS = 9; // 2^9 = 512
			this->IdentifyNamespace.LBAF[1].LBADS = 12; // 2^12 = 4096
			this->IdentifyNamespace.LBAF[2].LBADS = 13; // 2^13 = 8192

			auto currentLbaFormat = this->IdentifyNamespace.LBAF[this->IdentifyNamespace.FLBAS.CurrentLBAFormat];
			ASSERT_IF(currentLbaFormat.LBADS < 9, "Minimum posssible selected LBADS should be 9. 2^9 = 512.");

			this->IdentifyNamespace.NSZE = this->getNamespaceSizeInSectors();
			this->IdentifyNamespace.NCAP = this->IdentifyNamespace.NSZE; // todo: dealloacted lbas should subtract from this one day.
			this->IdentifyNamespace.NUSE = this->IdentifyNamespace.NSZE;

			this->IdentifyNamespace.NMIC.NamespaceMayBeAttachedToMoreThanOneController = 1;

			this->IdentifyNamespace.NVMCAP.NVMCAP_64[0] = this->Media.getSize(); // When we need more terabytes... let me know.

			return this->IdentifyNamespace;
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
				// Per NVMe spec the controller can do whatever here as long as the data is gone... so we shall 0x0C fill.
				memset(this->Media.getBuffer(), 0x0C, this->Media.getSize());
			}
			else
			{
				LOG_INFO("Performing a non-secure erase");
				memset(this->Media.getBuffer(), 0x00, this->Media.getSize());
			}

			return completionQueueEntry;
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
			UINT_32 sectorSize = (UINT_32)std::powl(2, this->IdentifyNamespace.LBAF[this->IdentifyNamespace.FLBAS.CurrentLBAFormat].LBADS);
			ASSERT_IF(sectorSize < 512, "Sector size shouldn't be less than 512!");
			return sectorSize;
		}
	}
}