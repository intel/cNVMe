/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
Payload.h - A header file for the Payload class
*/

#pragma once

#include "Types.h"

namespace cnvme
{
	class Payload
	{
	public:
		/// <summary>
		/// Create a payload with byteSize bytes
		/// </summary>
		/// <param name="byteSize">Number of bytes for the payload</param>
		Payload(UINT_32 byteSize);

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
		/// Destructor, deallocates memory
		/// </summary>
		~Payload();

		/// <summary>
		/// Get the byte pointer
		/// </summary>
		/// <returns>The underlying buffer</returns>
		UINT_8* getBuffer();

		/// <summary>
		/// Returns the size of the underlying buffer
		/// </summary>
		/// <returns>Size in bytes</returns>
		UINT_32 getSize();

	private:

		/// <summary>
		/// The byte pointer
		/// </summary>
		UINT_8* BytePointer;

		/// <summary>
		/// The number of bytes owned
		/// </summary>
		UINT_32 ByteSize;

	};
}
