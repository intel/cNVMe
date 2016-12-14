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

	LOG_SET_LEVEL(1);
	

	// run tests a few times to make sure things are normal
	bool passingTests = true;
	for (int i = 0; i < 50; i++)
	{
		passingTests &= cnvme::tests::pci::testPciHeaderId();
	}
	std::cout << "Tests passing: " << strings::toString(passingTests) << std::endl;


	cnvme::pci::PCIExpressRegisters p;

	char c = '1';
	std::cin >> c;

	// End testing code.
}