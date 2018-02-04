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
#include "System.h"
#include "Tests.h"

#include <iostream>

using namespace cnvme;
using namespace cnvme::command;
using namespace cnvme::driver;

int main()
{
	// This is testing code.
	LOG_SET_LEVEL(2);

	/* Test code for later.
	TestDriver driver;
	ASSERT_IF(!driver.firmwareImageDownload(0, tests::helpers::getFirmwareImage("TESTFW02", 4096)).CompletionQueueEntry.succeeded(),
		"FW Download failed");

	ASSERT_IF(!driver.firmwareCommit(constants::commands::fw_commit::commit_action::REPLACE_IN_SLOT_AND_ACTIVATE_ON_RESET, 2).CompletionQueueEntry.succeeded(),
		"FW Commit failed!");

#define GET_FW_STRING() std::string(((identify::structures::IDENTIFY_CONTROLLER*)(driver.identify(constants::commands::identify::cns::CONTROLLER, 0).OutputData.getBuffer()))->FR)

	ASSERT_IF(GET_FW_STRING() == "TESTFW02", "FR updated incorrectly");

	ASSERT_IF(!driver.controllerReset(), "Failed to controller reset");

	ASSERT_IF(GET_FW_STRING() != "TESTFW02", "FR did not update after reset");

	driver.controllerReset();
	*/

	LOG_SET_LEVEL(1);

	bool testsPassing = cnvme::tests::helpers::runTests();
	std::cout << "Tests passing: " << strings::toString(testsPassing) << std::endl;
	return(!testsPassing); // 0 is pass

	// End testing code.
}