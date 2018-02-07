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
Payload.cpp - An implementation file for the Payload class
*/

#include "Payload.h"

#include <algorithm>

namespace cnvme
{
	Payload::Payload(size_t byteSize) : Payload()
	{
		ByteSize = byteSize;
		BytePointer = new UINT_8[byteSize];
		memset(BytePointer, 0x00, ByteSize);
	}

	Payload::Payload(BYTE * pointer, size_t byteSize) : Payload::Payload(byteSize)
	{
		memcpy_s(BytePointer, ByteSize, pointer, byteSize);
	}

	Payload::Payload()
	{
		ByteSize = 0;
		BytePointer = nullptr;
		DeleteOnScopeLoss = true;
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
			for (size_t i = 0; i < getSize(); i++)
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
		if (BytePointer && DeleteOnScopeLoss)
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

	size_t Payload::getSize() const
	{
		return ByteSize;
	}

	void Payload::resize(size_t newSize)
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
		size_t oldSize = getSize();
		this->resize(oldSize + otherPayload.getSize());

		// copy other after this
		memcpy_s(this->getBuffer() + oldSize, this->getSize() - oldSize, otherPayload.getBuffer(), otherPayload.getSize());
	}

	void Payload::setDeleteOnScopeLoss(bool delOnScopeLoss)
	{
		DeleteOnScopeLoss = delOnScopeLoss;
	}

	void Payload::clear()
	{
		memset(this->getBuffer(), 0, this->getSize());
	}

	std::vector<Payload> Payload::split(size_t splitSize) const
	{
		ASSERT_IF(this->getSize() % splitSize != 0, "Payload is not evenly splitable by the given splitSize (" + std::to_string(splitSize) + ")");

		std::vector<Payload> retVec;
		size_t retVecSize = this->getSize() / splitSize;
		size_t individualPayloadSize = this->getSize() / retVecSize;

		for (size_t i = 0; i < retVecSize; i++)
		{
			retVec.emplace_back(this->getBuffer() + (i * individualPayloadSize), individualPayloadSize);
		}

		return retVec;
	}
}