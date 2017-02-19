/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2017
PRP.h - A header file for the NVMe PRPs
*/

#pragma once

#include "Types.h"

namespace cnvme
{
	class PRP
	{
	public:
		/// <summary>
		/// Base constructor
		/// </summary>
		PRP();

		/// <summary>
		/// Constructor from a memory address
		/// </summary>
		/// <param name="prp1">Memory Address of PRP1</param>
		/// <param name="prp2">Memory Address of PRP2</param>
		/// <param name="numBytes">Number of bytes for the PRP</param>
		/// <param name="memoryPageSize">Size in bytes of a memory page (CC.MPS)</param>
		PRP(UINT_64 prp1, UINT_64 prp2, UINT_32 numBytes, UINT_32 memoryPageSize);

		/// <summary>
		/// Constructor from a payload
		/// </summary>
		/// <param name="payload">Payload to copy data to created PRP list</param>
		/// <param name="memoryPageSize">Size in bytes of a memory page (CC.MPS)</param>
		PRP(Payload &payload, UINT_32 memoryPageSize);

		/// <summary>
		/// Destructor. Will only deallocate the PRP if it was created by a payload
		/// The logic is if getting it by address, you don't own it (drive). 
		/// If getting it from payload, you own it, so you should delete it (host).
		/// </summary>
		~PRP();

		/// <summary>
		/// Get a copy of the linked PRP data in the form of a payload
		/// </summary>
		/// <returns>A payload with data from the PRP</returns>
		Payload getPayloadCopy();

		/// <summary>
		/// Returns the number of bytes represented by the PRP
		/// </summary>
		/// <returns>Number of bytes</returns>
		UINT_32 getNumBytes();

		/// <summary>
		/// Returns if the PRP memory will be freed upon scope loss 
		/// </summary>
		/// <returns>Boolean</returns>
		bool willFreeUponScopeLoss();

		/// <summary>
		/// Returns the set memory page size (CC.MPS)
		/// </summary>
		/// <returns>Memory Page Size in bytes</returns>
		UINT_32 getMemoryPageSize();

		/// <summary>
		/// Returns PRP1 
		/// </summary>
		/// <returns>Address of PRP1</returns>
		UINT_64 getPRP1();

		/// <summary>
		/// Returns PRP2 
		/// </summary>
		/// <returns>Address of PRP2</returns>
		UINT_64 getPRP2();

		/// <summary>
		/// Takes in a payload and copies over the data into the existing PRP addresses
		/// </summary>
		/// <param name="payload">Data to copy to PRPs</param>
		/// <returns>True if the FULL payload has been sent to the PRPs. False otherwise.</returns>
		bool placePayloadInExistingPRPs(Payload &payload);

	private:

		/// <summary>
		/// If True: PRP object was allocated by Payload and will have linked memory be deleted
		/// If False: PRP object was allocated via an address that is not owned so linked memory will not be deleted
		/// </summary>
		bool FreeOnScopeLoss;

		/// <summary>
		/// Address for PRP1
		/// </summary>
		UINT_64 PRP1;

		/// <summary>
		/// Address for PRP2
		/// </summary>
		UINT_64 PRP2;

		/// <summary>
		/// Number of bytes represented by the PRP
		/// </summary>
		UINT_32 NumberOfBytes;

		/// <summary>
		/// CC.MPS. Needed to know the max size of PRP pages / lists
		/// </summary>
		UINT_32 MemoryPageSize;

		/// <summary>
		/// Returns True if this uses a PRP list in PRP2
		/// </summary>
		/// <returns>Boolean</returns>
		bool usesPRPList();

		/// <summary>
		/// Returns the total number of items in the PRP(2) list.
		/// This will include the chained PRP lists
		/// </summary>
		/// <returns>Number of items</returns>
		UINT_32 getTotalNumberOfItemsInPRPList();

		/// <summary>
		/// Gets the max number of PRPs in an unchained PRP list
		/// </summary>
		/// <returns>max PRPs in unchained list</returns>
		UINT_32 getMaxItemsInSinglePRPList();

		/// <summary>
		/// Gets a vector of pointers from the PRP2 list
		/// </summary>
		/// <returns>vector of byte pointers and the size of the data they point to</returns>
		std::vector<std::pair<BYTE*, UINT_32>> getPRPListPointers();

		/// <summary>
		/// Returns the number of chained PRPs needed
		/// </summary>
		/// <returns>UINT_32 representing the number of chained PRPs</returns>
		UINT_32 getNumberOfChainedPRPs();
	};
}
