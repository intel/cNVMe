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

void sendIdentifyController(Driver &driver, UINT_16 queue=0)
{
	UINT_32 BUF_SIZE = 8192;
	BYTE* buffer = new BYTE[BUF_SIZE];
	memset(buffer, 0, BUF_SIZE);

	DRIVER_COMMAND* d = (PDRIVER_COMMAND)buffer;
	d->Timeout = 6000;
	d->QueueId = queue; // for testing
	d->Command.DWord0Breakdown.OPC = cnvme::constants::opcodes::admin::IDENTIFY;
	d->Command.DW10_Identify.CNS = constants::commands::identify::cns::CONTROLLER;
	d->TransferDataDirection = READ;
	d->TransferDataSize = 4096;

	driver.sendCommand((UINT_8*)d, BUF_SIZE);

	auto statusCode = d->CompletionQueueEntry.SC || d->DriverStatus;
	
	delete[]buffer;

	ASSERT_IF(statusCode != 0, "Identify Controller Failed!");
}

void sendCreateIOCompletionQueue(Driver &driver, UINT_16 QID)
{
	UINT_32 BUF_SIZE = 8192;
	BYTE* buffer = new BYTE[BUF_SIZE];
	memset(buffer, 0, BUF_SIZE);

	DRIVER_COMMAND* d = (PDRIVER_COMMAND)buffer;
	d->Timeout = 6000;
	d->Command.DWord0Breakdown.OPC = cnvme::constants::opcodes::admin::CREATE_IO_COMPLETION_QUEUE;
	d->Command.DW10_CreateIoQueue.QID = QID;
	d->Command.DW10_CreateIoQueue.QSIZE = 15; // 0-based 16
	d->Command.DW11_CreateIoCompletionQueue.IEN = 1;
	d->Command.DW11_CreateIoCompletionQueue.PC = 1;

	d->TransferDataDirection = NO_DATA;

	driver.sendCommand((UINT_8*)d, BUF_SIZE);

	auto statusCode = d->CompletionQueueEntry.SC || d->DriverStatus;

	delete[]buffer;

	ASSERT_IF(statusCode != 0, "Create IO Completion Queue Failed!");
}
	
void sendCreateIOSubmissionQueue(Driver &driver, UINT_16 QID, UINT_16 CQID)
{
	UINT_32 BUF_SIZE = 8192;
	BYTE* buffer = new BYTE[BUF_SIZE];
	memset(buffer, 0, BUF_SIZE);

	DRIVER_COMMAND* d = (PDRIVER_COMMAND)buffer;
	d->Timeout = 6000;
	d->Command.DWord0Breakdown.OPC = cnvme::constants::opcodes::admin::CREATE_IO_SUBMISSION_QUEUE;
	d->Command.DW10_CreateIoQueue.QID = QID;
	d->Command.DW10_CreateIoQueue.QSIZE = 15; // 0-based 16
	d->Command.DW11_CreateIoSubmissionQueue.CQID = CQID;
	d->Command.DW11_CreateIoSubmissionQueue.PC = 1;

	d->TransferDataDirection = NO_DATA;

	driver.sendCommand((UINT_8*)d, BUF_SIZE);

	auto statusCode = d->CompletionQueueEntry.SC || d->DriverStatus;
	
	delete[]buffer;

	ASSERT_IF(statusCode != 0, "Create IO Submission Queue Failed!");
}

int main()
{
	// This is testing code.
	LOG_SET_LEVEL(2);

	//Driver driver;

	//sendIdentifyController(driver);
	//sendIdentifyController(driver);

	//sendCreateIOCompletionQueue(driver, 1);
	//sendCreateIOSubmissionQueue(driver, 1, 1);
	//sendIdentifyController(driver, 1);

	/*
	UINT_32 BUF_SIZE = 8192;
	BYTE* buffer = new BYTE[BUF_SIZE];
	memset(buffer, 0, BUF_SIZE);

	DRIVER_COMMAND* d = (PDRIVER_COMMAND)buffer;
	d->Timeout = 6000;
	d->Command.DWord0Breakdown.OPC = cnvme::constants::opcodes::admin::CREATE_IO_COMPLETION_QUEUE;
	d->Command.DW10_CreateIoQueue.QID = 1;
	d->Command.DW10_CreateIoQueue.QSIZE = 15; // 0-based 16
	d->Command.DW11_CreateIoCompletionQueue.IEN = 1;
	d->Command.DW11_CreateIoCompletionQueue.PC = 1;

	d->TransferDataDirection = NO_DATA;

	driver.sendCommand((UINT_8*)d, BUF_SIZE);

	delete[]buffer;
	*/

	LOG_SET_LEVEL(1);

	bool testsPassing = cnvme::tests::helpers::runTests();
	std::cout << "Tests passing: " << strings::toString(testsPassing) << std::endl;
	return(!testsPassing); // 0 is pass

	// End testing code.
}