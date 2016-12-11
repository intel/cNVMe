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
	cnvme::pci::capabilities::PCI_ADVANCED_ERROR_REPORTING_CAPABILITY p = { 0 };


	std::cout << p.toString() << std::endl;

	char c = '1';
	std::cin >> c;
}