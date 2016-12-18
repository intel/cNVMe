/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
Tests.h - A header file for all unit testing
*/
#pragma once

#include "ControllerRegisters.h"
#include "LoopingThread.h"
#include "Pcie.h"

using namespace cnvme;
using namespace cnvme::controller;
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

			/// <summary>
			/// Gets the current time in milliseconds
			/// </summary>
			UINT_64 getTimeInMilliseconds();

			/// <summary>
			/// Runs all tests
			/// </summary>
			bool runTests();
		}

		namespace general
		{
			/// <summary>
			/// Tests the LoopingThread class
			/// </summary>
			bool testLoopingThread();
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

		namespace controller_registers
		{
			/// <summary>
			/// Tests a controller level reset
			/// </summary>
			bool testControllerReset();
		}

	}
}
