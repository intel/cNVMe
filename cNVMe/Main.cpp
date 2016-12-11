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
	cnvme::Payload p(512);

	p.getBuffer()[0] = 0xff;

	std::cout << p.getSize() << std::endl;

	auto p2 = p;

	std::cout << p2.getSize() << std::endl;

	char c = '1';
	std::cin >> c;

	// End testing code.
}