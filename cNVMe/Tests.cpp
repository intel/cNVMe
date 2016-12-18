/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
Tests.cpp - An implementation file for all unit testing
*/

#include "Tests.h"

#include <random>
#include <future>

// Macros to fail a test
#define FAIL(s) LOG_ERROR(s);return false;
#define FAIL_IF(b, s); if (b) {FAIL(s);}

namespace cnvme
{
	namespace tests
	{
		namespace helpers
		{
			UINT_64 randInt(UINT_64 lower, UINT_64 upper)
			{
				std::random_device randomDevice;
				std::default_random_engine randomNumberEngine(randomDevice());

				std::uniform_int_distribution<UINT_64> distribution(lower, upper);
				return distribution(randomNumberEngine);
			}

			UINT_64 getTimeInMilliseconds()
			{
				return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			}

			bool runTests()
			{
				std::vector<std::future<bool>> results;

				// Run all tests 100 times, multi-threaded
				for (int i = 0; i < 100; i++)
				{
					results.push_back(std::async(pci::testPciHeaderId));
					results.push_back(std::async(general::testLoopingThread));
					results.push_back(std::async(controller_registers::testControllerReset));
				}

				bool retVal = true;
				for (auto &i : results)
				{
					retVal &= i.get();
				}

				return retVal;
			}
		}

		namespace general
		{
			bool testLoopingThread()
			{
				LoopingThread LT([&] {return; }, 10);
				for (int i = 0; i < 3; i++)
				{
					FAIL_IF(LT.isRunning(), "LoopingThread shouldn't be running before start");
					LT.start();
					FAIL_IF(!LT.isRunning(), "LoopingThread didn't immediately start running");
					FAIL_IF(!LT.waitForFlip(), "waitForFlip() should return true since it is running");
					LT.end();
					FAIL_IF(LT.waitForFlip(), "waitForFlip() should return false since it has ended");
					FAIL_IF(LT.isRunning(), "LoopingThread is still running after calling end()");
				}

				return true;
			}
		}

		namespace pci
		{
			bool testPciHeaderId()
			{
				PCIExpressRegisters p;
				Payload oldPayload = p.readHeaderAndCapabilities();

				UINT_16 oldVid = p.getPciExpressRegisters().PciHeader->ID.VID;
				UINT_16 oldDid = p.getPciExpressRegisters().PciHeader->ID.DID;

				UINT_16 newVid = oldVid;
				UINT_16 newDid = oldDid;

				// Make sure we set it to something different
				while (newVid == oldVid && newDid == oldDid)
				{
					newDid = (UINT_16)helpers::randInt(0, (UINT_16)-1);
					newVid = (UINT_16)helpers::randInt(0, (UINT_16)-1);
				}

				p.getPciExpressRegisters().PciHeader->ID.VID = newVid;
				p.getPciExpressRegisters().PciHeader->ID.DID = newDid;

				FAIL_IF(p.getPciExpressRegisters().PciHeader->ID.VID != newVid, "VID did not update");
				FAIL_IF(p.getPciExpressRegisters().PciHeader->ID.DID != newDid, "DID did not update");

				p.getPciExpressRegisters().PXCAP->PXDC.IFLR = 1; // Issue reset

				// Wait for thread to catch this... also tests that the thread is working
				p.waitForChangeLoop();

				FAIL_IF(p.getPciExpressRegisters().PciHeader->ID.VID != oldVid, "VID did not reset");
				FAIL_IF(p.getPciExpressRegisters().PciHeader->ID.DID != oldDid, "DID did not reset");

				Payload newPayload = p.readHeaderAndCapabilities();

				// The BIST is the last thing before the BARs. The BARs should more than likely be different since they have raw addresses.
				UINT_32 BISTOffset = offsetof(cnvme::pci::header::PCI_HEADER, cnvme::pci::header::PCI_HEADER::BIST);
				FAIL_IF(memcmp(oldPayload.getBuffer(), newPayload.getBuffer(), BISTOffset) != 0, \
					"PCI Header up till BIST did not match original after function level reset");

				return true;
			}
		}

		namespace controller_registers
		{
			bool testControllerReset()
			{
				Payload payload(sizeof(controller::registers::CONTROLLER_REGISTERS));
				UINT_64 memoryAddress = payload.getMemoryAddress();

				controller::registers::ControllerRegisters controllerRegisters(memoryAddress);
				auto CR = controllerRegisters.getControllerRegisters();
				auto timeoutMs = CR->CAP.TO * 500; // CAP.TO is in 500 millisecond intervals

				FAIL_IF(CR->CC.EN == 1, "CC.EN should not automatically move to 1");
				FAIL_IF(CR->CSTS.RDY != 0, "CSTS.RDY should be 0 after reset");

				CR->CC.EN = 1;

				bool rdyTo1 = false;
				UINT_64 deathTime = helpers::getTimeInMilliseconds() + timeoutMs;
				while (helpers::getTimeInMilliseconds() < deathTime)
				{
					if (CR->CSTS.RDY == 1)
					{
						rdyTo1 = true;
						break;
					}
				}
				FAIL_IF(rdyTo1 == false, "CSTS.RDY did not transition to 1 after CC.EN was set to 1");

				UINT_32 savedAMS = CR->CC.AMS;  
				UINT_32 savedACQB = (UINT_32)helpers::randInt(0, 0xFFFF);  // Make sure this does not get reset
				CR->CC.AMS = helpers::randInt(0, 0b111);          // Make sure most things get reset
				CR->ACQ.ACQB = savedACQB;

				CR->CC.EN = 0; // Begin Reset
				deathTime = helpers::getTimeInMilliseconds() + timeoutMs;
				bool rdyTo0 = false;
				while (helpers::getTimeInMilliseconds() < deathTime)
				{
					if (CR->CSTS.RDY == 0)
					{
						rdyTo0 = true;
						break;
					}
				}
				FAIL_IF(rdyTo0 == false, "CSTS.RDY did not transition to 0 after CC.EN was set to 0");

				CR->CC.EN = 1; // Enable controller and wait till ready
				rdyTo1 = false;
				deathTime = helpers::getTimeInMilliseconds() + timeoutMs;
				while (helpers::getTimeInMilliseconds() < deathTime)
				{
					if (CR->CSTS.RDY == 1)
					{
						rdyTo1 = true;
						break;
					}
				}
				FAIL_IF(rdyTo1 == false, "CSTS.RDY did not transition to 1 after CC.EN was set to 1");

				// Check that proper things reset
				FAIL_IF(CR->CC.AMS != savedAMS, "CC.AMS did not reset");
				FAIL_IF(CR->ACQ.ACQB != savedACQB, "ACQ.ACQB reset when it should not have");

				// Since the reset is complete, should have at most 1 queue
				FAIL_IF(controllerRegisters.getQueueDoorbells().size() > 1, "Somehow we have more than 1 doorbell after a controller reset");

				return true;
			}

		}
	}
}

