/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
Tests.cpp - An implementation file for all unit testing
*/

#include "Tests.h"

#include <random>

// Macros to fail a test
#define FAIL(s) LOG_ERROR(s);return false;
#define FAIL_IF_FALSE(b, s); if (!b) {FAIL(s);}

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

				if (p.getPciExpressRegisters().PciHeader->ID.VID != newVid)
				{
					FAIL("VID did not update");
				}

				if (p.getPciExpressRegisters().PciHeader->ID.DID != newDid)
				{
					FAIL("DID did not update");
				}

				p.getPciExpressRegisters().PXCAP->PXDC.IFLR = 1; // Issue reset

				// Wait for thread to catch this... also tests that the thread is working
				std::this_thread::sleep_for(std::chrono::milliseconds(CHANGE_CHECK_SLEEP_MS * 3));

				if (p.getPciExpressRegisters().PciHeader->ID.VID != oldVid)
				{
					FAIL("VID did not reset");
				}

				if (p.getPciExpressRegisters().PciHeader->ID.DID != oldDid)
				{
					FAIL("DID did not reset");
				}

				Payload newPayload = p.readHeaderAndCapabilities();

				// The BIST is the last thing before the BARs. The BARs should more than likely be different since they have raw addresses.
				UINT_32 BISTOffset = offsetof(cnvme::pci::header::PCI_HEADER, cnvme::pci::header::PCI_HEADER::BIST);
				if (memcmp(oldPayload.getBuffer(), newPayload.getBuffer(), BISTOffset) != 0)
				{
					FAIL("PCI Header up till BIST did not match original after function level reset")
				}

				return true;
			}
		}
	}
}


