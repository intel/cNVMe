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
PRP.cpp - An implementation file for the PRPs
*/

#include "PRP.h"

namespace cnvme
{
	PRP::PRP()
	{
		NumberOfBytes = 0;
		PRP1 = 0;
		PRP2 = 0;
		FreeOnScopeLoss = false;
		MemoryPageSize = 0;
	}

	PRP::PRP(UINT_64 prp1, UINT_64 prp2, size_t numBytes, UINT_32 memoryPageSize) : PRP()
	{
		FreeOnScopeLoss = false;
		PRP1 = prp1;
		PRP2 = prp2;
		NumberOfBytes = numBytes;
		MemoryPageSize = (size_t)memoryPageSize;
	}

	PRP::PRP(const Payload &payload, size_t memoryPageSize) : PRP()
	{
		this->constructFromPayloadAndMemoryPageSize(payload, memoryPageSize);
	}

	PRP::PRP(PRP & other) : PRP::PRP()
	{
		*this = other;
	}

	PRP& PRP::operator=(PRP& other)
	{
		// check for self-assignment
		if (&other == this)
		{
			return *this;
		}
		
		this->constructFromPayloadAndMemoryPageSize(other.getPayloadCopy(), other.MemoryPageSize);
		return *this;
	}

	PRP::~PRP()
	{
		if (FreeOnScopeLoss)
		{
			if (PRP1)
			{
				delete[] MEMORY_ADDRESS_TO_8POINTER(PRP1);
				PRP1 = 0;
			}

			if (usesPRPList())
			{
				// Go through all items in the PRP2 list and free memory
				std::vector<std::pair<BYTE*, size_t>> prpList = getPRPListPointers();
				for (std::pair<BYTE*, size_t> &prp : prpList)
				{
					delete[]prp.first;
				}
			}

			if (PRP2)
			{
				delete[] MEMORY_ADDRESS_TO_8POINTER(PRP2);
				PRP2 = 0;
			}
		}
	}

	Payload PRP::getPayloadCopy()
	{
		Payload payload;
		if (NumberOfBytes > 0)
		{
			size_t bytesRemaining = NumberOfBytes;
			// no matter what, prp 1 is used
			BYTE* prp1Pointer = MEMORY_ADDRESS_TO_8POINTER(PRP1);
			payload.append(Payload(prp1Pointer, std::min(bytesRemaining, MemoryPageSize)));
			bytesRemaining -= std::min(bytesRemaining, MemoryPageSize);
			if (bytesRemaining > 0)
			{
				if (usesPRPList())
				{
					std::vector<std::pair<BYTE*, size_t>> prpList = getPRPListPointers();
					for (std::pair<BYTE*, size_t> &prp : prpList)
					{
						payload.append(Payload(prp.first, prp.second));
					}
				}
				else
				{
					BYTE* prp2Pointer = MEMORY_ADDRESS_TO_8POINTER(PRP2);
					payload.append(Payload(prp2Pointer, std::min(bytesRemaining, MemoryPageSize)));
					bytesRemaining -= std::min(bytesRemaining, MemoryPageSize);
				}
			}
		}
		return payload;
	}

	size_t PRP::getNumBytes()
	{
		return NumberOfBytes;
	}

	bool PRP::willFreeUponScopeLoss()
	{
		return FreeOnScopeLoss;
	}

	size_t PRP::getMemoryPageSize()
	{
		return MemoryPageSize;
	}

	UINT_64 PRP::getPRP1()
	{
		return PRP1;
	}

	UINT_64 PRP::getPRP2()
	{
		return PRP2;
	}

	bool PRP::placePayloadInExistingPRPs(Payload &payload)
	{
		if (payload.getSize() > getNumBytes())
		{
			ASSERT("Given payload is larger than the allocated PRPs");
			return false;
		}

		size_t bytesRemaining = payload.getSize();

		BYTE* prp1Pointer = MEMORY_ADDRESS_TO_8POINTER(PRP1);

		// Copy in first page of data
		size_t bytesIntoPrp1 = std::min(MemoryPageSize, bytesRemaining);
		BYTE* payloadBuf = payload.getBuffer();
		memcpy_s(prp1Pointer, MemoryPageSize, payloadBuf, bytesIntoPrp1);
		payloadBuf += bytesIntoPrp1;
		bytesRemaining -= bytesIntoPrp1;

		if (bytesRemaining)
		{
			BYTE* prp2Pointer = MEMORY_ADDRESS_TO_8POINTER(PRP2);
			if (usesPRPList())
			{
				std::vector<std::pair<BYTE*, size_t>> prpList = getPRPListPointers();
				for (std::pair<BYTE*, size_t> &prp : prpList)
				{
					memcpy_s(prp.first, prp.second, payloadBuf, prp.second);
					payloadBuf += prp.second;
				}
			}
			else
			{
				// Copy the rest of the data into PRP2
				size_t bytesIntoPrp2 = std::min(MemoryPageSize, bytesRemaining);
				memcpy_s(prp2Pointer, MemoryPageSize, payloadBuf, bytesIntoPrp2);
			}
		}
		return true;
	}

	bool PRP::usesPRPList()
	{
		return NumberOfBytes > (MemoryPageSize * 2);
	}

	UINT_32 PRP::getTotalNumberOfItemsInPRPList()
	{
		if (usesPRPList())
		{
			size_t bytesRemaining = NumberOfBytes;
			bytesRemaining -= std::min(MemoryPageSize, NumberOfBytes);

			return (UINT_32)std::ceil(bytesRemaining / (double)MemoryPageSize);
		}

		return 0;
	}

	UINT_32 PRP::getMaxItemsInSinglePRPList()
	{
		return (UINT_32)(MemoryPageSize / sizeof(UINT_64));
	}

	std::vector<std::pair<BYTE*, size_t>> PRP::getPRPListPointers()
	{
		std::vector<std::pair<BYTE*, size_t>> prpListPointers;
		if (usesPRPList())
		{
			size_t bytesRemaining = NumberOfBytes - std::min(MemoryPageSize, NumberOfBytes);

			UINT_32 numberOfChainedPRPs = getNumberOfChainedPRPs();

			UINT_64* singlePrp = MEMORY_ADDRESS_TO_64POINTER(PRP2);
			for (UINT_32 i = 0; i < numberOfChainedPRPs; i++)
			{
				for (UINT_32 j = 0; j < getMaxItemsInSinglePRPList(); j++)
				{
					// Out of data or we need to follow the link to the next chain
					if (bytesRemaining == 0 || ((j + 1) == getMaxItemsInSinglePRPList() && (i + 1) != numberOfChainedPRPs))
					{
						break;
					}

					BYTE* thisPrp = (BYTE*)*singlePrp;
					ASSERT_IF(!thisPrp, "The current PRP appears to be NULL");
					size_t dataSize = std::min(MemoryPageSize, bytesRemaining);
					prpListPointers.emplace_back(thisPrp, dataSize);
					bytesRemaining -= dataSize;
					singlePrp++; // next item in prp list
				}

				if (bytesRemaining == 0)
				{
					break;
				}

				// Follow chain
				singlePrp = MEMORY_ADDRESS_TO_64POINTER(*singlePrp);
			}
		}
		return prpListPointers;
	}

	UINT_32 PRP::getNumberOfChainedPRPs()
	{
		return (UINT_32)std::ceil(getTotalNumberOfItemsInPRPList() / (double)getMaxItemsInSinglePRPList());
	}

	void PRP::constructFromPayloadAndMemoryPageSize(const Payload& payload, size_t memoryPageSize)
	{
		LOG_INFO("Payload with a size of " + std::to_string(payload.getSize()) + " was passed to PRP()");

		FreeOnScopeLoss = true;
		NumberOfBytes = payload.getSize();
		MemoryPageSize = memoryPageSize;

		size_t bytesRemaining = NumberOfBytes;

		// PRP1 will be the first MPS (memory page size) of the data
		size_t prp1DataSize = std::min(payload.getSize(), MemoryPageSize);
		ALLOC_BYTE_ARRAY(prp1Pointer, prp1DataSize);
		// This is sort of not how this works in NVMe. In NVMe, we would have an entire page allocated.
		// Though for the simulation, this can be really slow. If we only need say 512 bytes instead of a full 128MB page
		// We will only allocate the 512 as opposed finding a full page. While here, another oddity is the offset.
		// I'm not using the offset, it is always 0. Assume that we can always allocate a complete, empty page.

		memcpy_s(prp1Pointer, prp1DataSize, payload.getBuffer(), prp1DataSize); //do not copy the whole payload. Just the prp1DataSize.

		bytesRemaining -= prp1DataSize;

		if (bytesRemaining > 0)
		{
			// PRP2 will be the next MPS or a pointer to a PRP list 
			ALLOC_BYTE_ARRAY(prp2Pointer, MemoryPageSize);

			// If the remaining data size is less than a second memory page, just copy to that pointer
			if (!usesPRPList())
			{
				memcpy_s(prp2Pointer, MemoryPageSize, payload.getBuffer() + MemoryPageSize, \
					std::min(payload.getSize() - MemoryPageSize, MemoryPageSize));
			}
			else
			{
				UINT_64* prpListPointer = (UINT_64*)prp2Pointer;
				auto pPrpList = &(*prpListPointer);

				UINT_32 numberOfItemsInPrpList = getTotalNumberOfItemsInPRPList();
				UINT_32 numberOfChainedPrps = getNumberOfChainedPRPs();
				UINT_32 numberOfItemsInSinglePrpList = getMaxItemsInSinglePRPList();

				BYTE* bufPointer = payload.getBuffer() + MemoryPageSize;

				for (UINT_32 i = 0; i < numberOfChainedPrps; i++)
				{
					for (UINT_32 j = 0; j < numberOfItemsInSinglePrpList; j++)
					{
						// Out of data or we need to create the next chain
						if (bytesRemaining == 0 || ((j + 1) == numberOfItemsInSinglePrpList && (i + 1) != numberOfChainedPrps))
						{
							break;
						}

						ALLOC_BYTE_ARRAY(listItem, MemoryPageSize);

						size_t bytesToCopy = std::min(MemoryPageSize, bytesRemaining);

						memcpy_s(listItem, MemoryPageSize, bufPointer, bytesToCopy);

						bytesRemaining -= bytesToCopy;
						bufPointer += bytesToCopy;

						*pPrpList = POINTER_TO_MEMORY_ADDRESS(listItem);
						pPrpList++;
					}

					if (bytesRemaining == 0)
					{
						break;
					}

					// Create new chain
					ALLOC_BYTE_ARRAY(newPrpList, MemoryPageSize);

					*pPrpList = POINTER_TO_MEMORY_ADDRESS(newPrpList);
					pPrpList = &(*(UINT_64*)newPrpList);
				}
			}
			// Only set PRP2 if needed.
			PRP2 = POINTER_TO_MEMORY_ADDRESS(prp2Pointer);
		}
		PRP1 = POINTER_TO_MEMORY_ADDRESS(prp1Pointer);
	}
}
