/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
Payload.cpp - An implementation file for the Payload class
*/

#include "Payload.h"

namespace cnvme
{
	Payload::Payload(UINT_32 byteSize)
	{
		ByteSize = byteSize;
		BytePointer = new UINT_8[byteSize];
		memset(BytePointer, 0, ByteSize);
	}

	Payload::Payload()
	{
		ByteSize = 0;
		BytePointer = nullptr;
	}

	Payload::Payload(const Payload& other) : Payload::Payload()
	{
		*this = other;
	}

	Payload& Payload::operator=(const Payload& other)
	{
		// check for self-assignment
		if (&other == this)
		{
			return *this;
		}

		ByteSize = other.ByteSize;
		BytePointer = new UINT_8[other.ByteSize];

		memcpy_s(BytePointer, ByteSize, other.BytePointer, other.ByteSize);
		return *this;
	}

	Payload::~Payload()
	{
		if (BytePointer)
		{
			delete[] BytePointer;
			BytePointer = nullptr;
			ByteSize = 0;
		}
	}

	UINT_8* Payload::getBuffer()
	{
		return BytePointer;
	}

	UINT_32 Payload::getSize()
	{
		return ByteSize;
	}
}