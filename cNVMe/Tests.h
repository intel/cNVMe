/*
###########################################################################################
// cNVMe - An Open Source NVMe Device Simulation - MIT License
// Copyright 2017 - Intel Corporation

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
############################################################################################
Tests.h - A header file for all unit testing
*/
#pragma once

#include "Command.h"
#include "Constants.h"
#include "Controller.h"
#include "ControllerRegisters.h"
#include "Driver.h"
#include "Identify.h"
#include "LoopingThread.h"
#include "PCIe.h"
#include "PRP.h"

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

			/// <summary>
			/// Fills a payload with random data
			/// </summary>
			void randomizePayload(Payload &payload);
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

		namespace commands
		{
			/// <summary>
			/// Tests the general NVMe Command parsing
			/// </summary>
			bool testNVMeCommandParsing();

			/// <summary>
			/// Tests that the controller can fail a command with an unknown command opcode
			/// </summary>
			bool testNVMeCommandOpcodeInvalid();

			/// <summary>
			/// Tests that if we create an SQ/CQ we have to delete the SQ first
			/// </summary>
			bool testNVMeQueueDeletionFailures();

			/// <summary>
			/// Tests that we can read/write from a given namespace and then we can format it.
			/// </summary>
			bool testNVMeIo();
		}

		namespace driver
		{
			/// <summary>
			/// Tests sending no data commands via the driver
			/// <summary>
			bool testNoDataCommandViaDriver();

			/// <summary>
			/// Tests sending read commands via the driver
			/// <summary>
			bool testReadCommandViaDriver();
		}

		namespace prp
		{
			/// <summary>
			/// Tests that creating a PRP and retriving the resulting payload yield
			///   the same results.
			/// </summary>
			bool testDifferentPRPSizes();

			/// <summary>
			/// Test copying an existing payload into an existing PRP.
			/// </summary>
			bool testDataIntoExistingPRP();
		}

		namespace logging
		{
			/// <summary>
			/// Tests that on this build we can asserts work as expected.
			/// Debug: throw, Release: don't throw.
			/// </summary>
			bool testAsserting();
		}
	}
}
