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

	std::cout << ((cnvme::pci::header::PCI_HEADER*)(p.getHeaderAndCapabilities().getBuffer()))->toString() << std::endl;

	char c = '1';
	std::cin >> c;

	// End testing code.
}