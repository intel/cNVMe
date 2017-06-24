/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2017
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

	Payload::Payload(BYTE * pointer, UINT_32 byteSize) : Payload::Payload(byteSize)
	{
		memcpy_s(BytePointer, ByteSize, pointer, byteSize);
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

#ifdef PAYLOAD_CMP_DEBUG // Used for debugging comparison issues
			for (UINT_32 i = 0; i < getSize(); i++)
			{
				if (getBuffer()[i] != other.getBuffer()[i])
				{
					printf("Miscompare at index %u : 0x%X != 0x%X\n", i, getBuffer()[i], other.getBuffer()[i]);
				}
			}
#endif //PAYLOAD_CMP_DEBUG
			return memcmp(this->getBuffer(), other.getBuffer(), this->getSize()) == 0;

		}

		return false;
	}

	bool Payload::operator!=(const Payload & other)
	{
		return !(*this == other);
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

	void Payload::append(const Payload &otherPayload)
	{
		UINT_32 oldSize = getSize();
		this->resize(oldSize + otherPayload.getSize());

		// copy other after this
		memcpy_s(this->getBuffer() + oldSize, this->getSize() - oldSize, otherPayload.getBuffer(), otherPayload.getSize());
	}
}