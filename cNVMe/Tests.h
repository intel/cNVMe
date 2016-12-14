/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
Tests.h - A header file for all unit testing
*/
#pragma once

#include "Pcie.h"

using namespace cnvme;
using namespace cnvme::pci;

namespace cnvme
{
	namespace tests
	{
		namespace helpers
		{
			/// <summary>
			/// Generates a (sort-of) random number between lower and upper, inclusive
			/// </summary>
			UINT_64 randInt(UINT_64 lower, UINT_64 upper);
		}

		namespace pci
		{
			/// <summary>
			/// Tests updating the pci header.
			/// Issues function level reset
			/// Checks that things are back to original values
			/// </summary>
			bool testPciHeaderId();
		}

	}
}
