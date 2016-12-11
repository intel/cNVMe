/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
Main.cpp - An implementation file for the Main entry
*/

#include "Strings.h"
#include "PCIe.h"

#include <iostream>

using namespace cnvme;

int main()
{
	// This is testing code.
	
	pci::PCIExpressRegisters p;

	p.getPciExpressRegisters().PciHeader->ID.VID = 0xBB;
	p.getPciExpressRegisters().PciHeader->ID.DID = 0xFF;

	std::cout << p.getPciExpressRegisters().PciHeader->toString() << std::endl;

	Payload other(99999);
	memset(other.getBuffer(), 0xff, 99999);
	p.writeHeaderAndCapabilities(other); // should do FLR

	std::cout << p.getPciExpressRegisters().PciHeader->toString() << std::endl;

	char c = '1';
	std::cin >> c;

	// End testing code.
}