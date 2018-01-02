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
			/// Perform a format NVM command on the given namespace.
			/// </summary>
			/// <param name="nvmeCommand">Complete NVMe command for the format</param>
			/// <returns>Completion queue entry for command</returns>
			command::COMPLETION_QUEUE_ENTRY formatNVM(command::NVME_COMMAND nvmeCommand);
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
