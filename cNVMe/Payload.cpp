/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
Payload.cpp - An implementation file for the Payload class
*/

#include "Payload.h"

#include <algorithm>

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

	bool Payload::operator==(const Payload &other)
	{
		if (this->getSize() == other.getSize())
		{
			return memcmp(this->getBuffer(), other.getBuffer(), this->getSize()) == 0;
		}

		return false;
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

	UINT_8* Payload::getBuffer() const
	{
		return BytePointer;
	}

	UINT_32 Payload::getSize() const
	{
		return ByteSize;
	}

	void Payload::resize(UINT_32 newSize)
	{
		if (newSize != ByteSize)
		{
			UINT_8* tmp = new UINT_8[newSize];
			memcpy_s(tmp, newSize, BytePointer, std::min(ByteSize, newSize)); // Only copy current size at most... don't overflow
			delete[] BytePointer;
			BytePointer = tmp;
			ByteSize = newSize;
		}
	}
	UINT_64 Payload::getMemoryAddress()
	{
		if (getBuffer())
		{
			return (UINT_64)&(*getBuffer());
		}
		return NULL;
	}
}