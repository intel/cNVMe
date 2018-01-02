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
Main.cpp - An implementation file for the Main entry
*/

#include "Driver.h"
#include "Strings.h"
#include "Tests.h"


#include <iostream>

using namespace cnvme;
using namespace cnvme::command;
using namespace cnvme::driver;

int main()
{
	// This is testing code.
	LOG_SET_LEVEL(2);

	Driver driver;
	UINT_32 BUF_SIZE = 8192;
	BYTE* buffer = new BYTE[BUF_SIZE];
	memset(buffer, 0, BUF_SIZE);

	DRIVER_COMMAND* d = (PDRIVER_COMMAND)buffer;
	d->Timeout = 5;
	d->Command.DWord0Breakdown.OPC = cnvme::constants::opcodes::admin::IDENTIFY;
	d->Command.DWord10 = cnvme::constants::commands::identify::cns::NAMESPACE_ACTIVE;
	d->Command.NSID = 1;
	d->TransferDataDirection = READ;
	d->TransferDataSize = 4096;

	driver.sendCommand((UINT_8*)d, BUF_SIZE);

	identify::structures::IDENTIFY_NAMESPACE* i = (identify::structures::IDENTIFY_NAMESPACE*)&d->TransferData;

	LOG_SET_LEVEL(1);

	bool testsPassing = cnvme::tests::helpers::runTests();
	std::cout << "Tests passing: " << strings::toString(testsPassing) << std::endl;

	exit(!testsPassing); // 0 is pass

	// End testing code.
}