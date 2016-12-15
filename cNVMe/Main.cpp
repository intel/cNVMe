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


	std::cout << "Tests passing: " << strings::toString(cnvme::tests::helpers::runTests()) << std::endl;

	char c = '1';
	std::cin >> c;

	// End testing code.
}