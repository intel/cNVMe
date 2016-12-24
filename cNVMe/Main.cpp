/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
Main.cpp - An implementation file for the Main entry
*/

#include "Strings.h"
#include "Tests.h"

#include <iostream>

using namespace cnvme;

int main()
{
	// This is testing code.

	LOG_SET_LEVEL(2);

	Controller co;
	auto regs = co.getControllerRegisters()->getControllerRegisters();

	// One entry per queue
	regs->AQA.ACQS = 1;
	regs->AQA.ASQS = 1;
	Payload p(64);
	regs->ASQ.ASQB = p.getMemoryAddress();
	regs->ACQ.ACQB = p.getMemoryAddress(); // not used yet.

	// enable controller
	regs->CC.EN = 1;

	// Setup some raw DW info
	p.getBuffer()[0] = 0xFF;
	p.getBuffer()[2] = 0xFF;
	controller::registers::QUEUE_DOORBELLS* doorbells = (controller::registers::QUEUE_DOORBELLS*)((UINT_8*)regs) + sizeof(controller::registers::CONTROLLER_REGISTERS);
	doorbells[0].SQTDBL.SQT = 1;
	// soon after this, we see the DWs come up from the logging 

	co.waitForChangeLoop();


	LOG_SET_LEVEL(1);

	std::cout << "Tests passing: " << strings::toString(cnvme::tests::helpers::runTests()) << std::endl;

	char c = '1';
	std::cin >> c;

	// End testing code.
}