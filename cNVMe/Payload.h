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
Payload.h - A header file for the Payload class
*/

#pragma once

#include "Types.h"

namespace cnvme
{
	/// <summary>
	/// Payload is a safe dynamic memory allocation class
	/// </summary>
	class Payload
	{
	public:
		/// <summary>
		/// Create a payload with byteSize bytes
		/// </summary>
		/// <param name="byteSize">Number of bytes for the payload</param>
		Payload(UINT_32 byteSize);

		/// <summary>
		/// Create a payload from a pointer/length. This copies the data.
		/// </summary>
		/// <param name="pointer">byte array</param>
		/// <param name="byteSize">size of the array</param>
		Payload(BYTE* pointer, UINT_32 byteSize);

		/// <summary>
		/// Default constructor
		/// </summary>
		Payload();

		/// <summary>
		/// Copy constructor
		/// </summary>
		/// <param name="other">Another Payload to copy from</param>
		Payload(const Payload &other);

		/// <summary>
		/// Assignment operator
		/// </summary>
		/// <param name="other">Another payload to copy from</param>
		/// <returns>Payload</returns>
		Payload& operator=(const Payload& other);

		/// <summary>
		/// Checks if the two payloads are equivalent
		/// </summary>
		/// <param name="other">The other payload</param>
		/// <returns>true if equal size and contents</returns>
		bool operator==(const Payload& other);

		/// <summary>
		/// Checks if the two payloads are not equivalent
		/// </summary>
		/// <param name="other">The other payload</param>
		/// <returns>false if equal size and contents</returns>
		bool operator!=(const Payload& other);

		/// <summary>
		/// Destructor, deallocates memory
		/// </summary>
		~Payload();

		/// <summary>
		/// Get the byte pointer
		/// </summary>
		/// <returns>The underlying buffer</returns>
		UINT_8* getBuffer();

		/// <summary>
		/// Get the byte pointer (as const)
		/// </summary>
		/// <returns>The underlying buffer</returns>
		UINT_8* getBuffer() const;

		/// <summary>
		/// Returns the size of the underlying buffer
		/// </summary>
		/// <returns>Size in bytes</returns>
		UINT_32 getSize() const;

		/// <summary>
		/// Resizes the payload
		/// </summary>
		/// <param name="newSize">The new size in bytes</param>
		void resize(UINT_32 newSize);

		/// <summary>
		/// Returns the memory address for the payload
		/// </summary>
		/// <returns>Memory address</returns>
		UINT_64 getMemoryAddress();

		/// <summary>
		/// Append another payload after this one
		/// </summary>
		/// <param name="otherPayload">another payload</param>
		void append(const Payload &otherPayload);

		/// <summary>
		/// Sets if we should free memory on scope loss 
		/// </summary>
		/// <param name="delOnScopeLoss">If true, delete on scope loss. If false dont.</param>
		void setDeleteOnScopeLoss(bool delOnScopeLoss);

	private:

		/// <summary>
		/// The byte pointer
		/// </summary>
		UINT_8* BytePointer;

		/// <summary>
		/// The number of bytes owned
		/// </summary>
		UINT_32 ByteSize;

		/// <summary>
		/// If True, delete memory on scope loss, otherwise don't.
		/// </summary>
		bool DeleteOnScopeLoss;
	};
}
