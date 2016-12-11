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
	
	LOG_SET_LEVEL(2);
	LOG_ERROR("Hey");
	LOG_SET_LEVEL(0);
	LOG_ERROR("Hey2");
	LOG_SET_LEVEL(2);
	LOG_INFO("Hey3");


	char c = '1';
	std::cin >> c;

	// End testing code.
}