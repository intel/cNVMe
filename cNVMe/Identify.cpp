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
Identify.cpp - An implementation file for the NVMe Identify Commands
*/

#include "Identify.h"
#include "Tests.h"

namespace cnvme
{
	namespace identify
	{
		namespace structures
		{
			void IDENTIFY_CONTROLLER::setSubsystemQualifiedName()
			{
				char buffer[2]; // 1 extra for terminating NULL from snprintf
				std::string nqn(constants::commands::identify::EMPTY_NQN);
				for (size_t i = 0; i < nqn.size(); i++)
				{
					// replace spaces with a random hex char
					if (nqn[i] == ' ')
					{
						UINT_8 randNum = (UINT_8)tests::helpers::randInt(0, 0xf);
						ASSERT_IF_NE(snprintf(buffer, sizeof(buffer), "%x", randNum), 1, "snprintf() failed");
						nqn[i] = buffer[0];
					}
				}

				memcpy_s(&this->SUBNQN, sizeof(this->SUBNQN), nqn.c_str(), nqn.size());
			}
		}
	}
}