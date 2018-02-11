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
Namespace.h - A header file for a cNVMe Namespace
*/

#pragma once

#include "Command.h"
#include "Identify.h"

#define DEFAULT_SECTOR_SIZE 512

namespace cnvme
{
	namespace ns
	{
		class Namespace
		{
		public:
			/// <summary>
			/// Constructor for namespace
			/// </summary>
			Namespace();

			/// <summary>
			/// Constructor for namespace that takes in a size
			/// </summary>
			Namespace(size_t SizeInBytes);

			/// <summary>
			/// Destructor for namespace
			/// </summary>
			~Namespace();

			/// <summary>
			/// Returns the Identify Namespace structure
			/// </summary>
			/// <returns>IDENTIFY_NAMESPACE</returns>
			identify::structures::IDENTIFY_NAMESPACE& getIdentifyNamespaceStructure();

			/// <summary>
			/// Returns an identify namespace descriptor list for this namespace
			/// </summary>
			/// <returns>Payload</returns>
			Payload getIdentifyNamespaceDescriptorList();

			/// <summary>
			/// Perform a format NVM command on the given namespace.
			/// </summary>
			/// <param name="nvmeCommand">Complete NVMe command for the format</param>
			/// <returns>Completion queue entry for command</returns>
			command::COMPLETION_QUEUE_ENTRY formatNVM(command::NVME_COMMAND nvmeCommand);

			/// <summary>
			/// Performs an NVM Read command on the given namespace
			/// </summary>
			/// <param name="nvmeCommand">Complete NVMe command for the read</param>
			/// <param name="outputPayload">Data read out</param>
			/// <returns>Completion queue entry for command</returns>
			command::COMPLETION_QUEUE_ENTRY read(command::NVME_COMMAND nvmeCommand, Payload& outputPayload);

			/// <summary>
			/// Performs an NVM Write command on the given namespace
			/// </summary>
			/// <param name="nvmeCommand">Complete NVMe command for the write</param>
			/// <param name="memoryPageSize">size of the memory page</param>
			/// <returns>Completion queue entry for command</returns>
			command::COMPLETION_QUEUE_ENTRY write(command::NVME_COMMAND nvmeCommand, UINT_32 memoryPageSize);

			/// <summary>
			/// Gets a generic identify namespace structure (used with NSID==0xFFFFFFFF)
			/// </summary>
			/// <returns>IDENTIFY_NAMESPACE</returns>
			static identify::structures::IDENTIFY_NAMESPACE getGenericIdentifyNamespaceStructure();

		private:

			/// <summary>
			/// Gets this namespace's size in sectors. The sector size is variable and should be grabbed from the ID namespace structure
			/// </summary>
			/// <returns>Number of sectors for this namespace's size</returns>
			UINT_64 getNamespaceSizeInSectors();

			/// <summary>
			/// Gets the sector size for this namespace (in bytes).
			/// </summary>
			/// <returns>sector size</returns>
			UINT_32 getSectorSize();

			/// <summary>
			/// Internal representation of the Identify Namespace structure
			/// </summary>
			identify::structures::IDENTIFY_NAMESPACE IdentifyNamespace;

			/// <summary>
			/// Internal managed media
			/// </summary>
			Payload Media;
		};
	}
}
