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

	auto b = p.readHeaderAndCapabilities();

	memset(b.getBuffer(), 0xff, b.getSize());

	p.writeHeaderAndCapabilities(b);

	std::cout << ((pci::header::PCI_HEADER*)p.readHeaderAndCapabilities().getBuffer())->toString() << std::endl;

	auto j = p;
	std::cout << ((pci::header::PCI_HEADER*)j.readHeaderAndCapabilities().getBuffer())->toString() << std::endl;


	char c = '1';
	std::cin >> c;

	// End testing code.
}