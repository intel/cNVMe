/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2017
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

	PRP::PRP(UINT_64 prp1, UINT_64 prp2, UINT_32 numBytes, UINT_32 memoryPageSize) : PRP()
	{
		FreeOnScopeLoss = false;
		PRP1 = prp1;
		PRP2 = prp2;
		NumberOfBytes = numBytes;
		MemoryPageSize = memoryPageSize;
	}

	PRP::PRP(Payload &payload, UINT_32 memoryPageSize) : PRP()
	{
		FreeOnScopeLoss = true;
		NumberOfBytes = payload.getSize();
		MemoryPageSize = memoryPageSize;

		UINT_32 bytesRemaining = NumberOfBytes;

		// PRP1 will be the first MPS (memory page size) of the data
		UINT_32 prp1DataSize = std::min(payload.getSize(), MemoryPageSize);
		ALLOC_BYTE_ARRAY(prp1Pointer, prp1DataSize); 
		// This is sort of not how this works in NVMe. In NVMe, we would have an entire page allocated.
		// Though for the simulation, this can be really slow. If we only need say 512 bytes instead of a full 128MB page
		// We will only allocate the 512 as opposed finding a full page. While here, another oddity is the offset.
		// I'm not using the offset, it is always 0. Assume that we can always allocate a complete, empty page.
		
		memcpy(prp1Pointer, payload.getBuffer(), prp1DataSize);

		bytesRemaining -= prp1DataSize;

		if (bytesRemaining > 0)
		{
			// PRP2 will be the next MPS or a pointer to a PRP list 
			ALLOC_BYTE_ARRAY(prp2Pointer, MemoryPageSize);

			// If the remaining data size is less than a second memory page, just copy to that pointer
			if (!usesPRPList())
			{
				memcpy(prp2Pointer, payload.getBuffer() + MemoryPageSize, \
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

						UINT_32 bytesToCopy = std::min(MemoryPageSize, bytesRemaining);

						memcpy(listItem, bufPointer, bytesToCopy);

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
				std::vector<std::pair<BYTE*, UINT_32>> prpList = getPRPListPointers();
				for (std::pair<BYTE*, UINT_32> &prp : prpList)
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
			UINT_32 bytesRemaining = NumberOfBytes;
			// no matter what, prp 1 is used
			BYTE* prp1Pointer = MEMORY_ADDRESS_TO_8POINTER(PRP1);
			payload.append(Payload(prp1Pointer, std::min(bytesRemaining, MemoryPageSize)));
			bytesRemaining -= std::min(bytesRemaining, MemoryPageSize);
			if (bytesRemaining > 0)
			{
				if (usesPRPList())
				{
					std::vector<std::pair<BYTE*, UINT_32>> prpList = getPRPListPointers();
					for (std::pair<BYTE*, UINT_32> &prp : prpList)
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

	UINT_32 PRP::getNumBytes()
	{
		return NumberOfBytes;
	}

	bool PRP::willFreeUponScopeLoss()
	{
		return FreeOnScopeLoss;
	}

	UINT_32 PRP::getMemoryPageSize()
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

		UINT_32 bytesRemaining = payload.getSize();

		BYTE* prp1Pointer = MEMORY_ADDRESS_TO_8POINTER(PRP1);

		// Copy in first page of data
		UINT_32 bytesIntoPrp1 = std::min(MemoryPageSize, bytesRemaining);
		BYTE* payloadBuf = payload.getBuffer();
		memcpy_s(prp1Pointer, MemoryPageSize, payloadBuf, bytesIntoPrp1);
		payloadBuf += bytesIntoPrp1;
		bytesRemaining -= bytesIntoPrp1;

		if (bytesRemaining)
		{
			BYTE* prp2Pointer = MEMORY_ADDRESS_TO_8POINTER(PRP2);
			if (usesPRPList())
			{
				std::vector<std::pair<BYTE*, UINT_32>> prpList = getPRPListPointers();
				for (std::pair<BYTE*, UINT_32> &prp : prpList)
				{
					memcpy_s(prp.first, prp.second, payloadBuf, prp.second);
					payloadBuf += prp.second;
				}
			}
			else
			{
				// Copy the rest of the data into PRP2
				UINT_32 bytesIntoPrp2 = std::min(MemoryPageSize, bytesRemaining);
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
			UINT_32 bytesRemaining = NumberOfBytes;
			bytesRemaining -= std::min(MemoryPageSize, NumberOfBytes);

			return (UINT_32)std::ceil(bytesRemaining / (double)MemoryPageSize);
		}

		return 0;
	}

	UINT_32 PRP::getMaxItemsInSinglePRPList()
	{
		return (UINT_32)(MemoryPageSize / sizeof(UINT_64));
	}

	std::vector<std::pair<BYTE*, UINT_32>> PRP::getPRPListPointers()
	{
		std::vector<std::pair<BYTE*, UINT_32>> prpListPointers;
		if (usesPRPList())
		{
			UINT_32 bytesRemaining = NumberOfBytes - std::min(MemoryPageSize, NumberOfBytes);

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
					UINT_32 dataSize = std::min(MemoryPageSize, bytesRemaining);
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
}
