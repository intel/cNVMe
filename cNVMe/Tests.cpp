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
Tests.cpp - An implementation file for all unit testing
*/

#include "Tests.h"

// Macros to fail a test
#define FAIL_IF_AND_HIDE_LOG(b, s) _HIDE_LOG_THREAD(); FAIL_IF(b, s); _UNHIDE_LOG_THREAD(); // FAIL_IF except don't show the log output. Hide the LOG_ERROR we think would happen.

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

				// Run all tests 2 times, multi-threaded
				for (int i = 0; i < 2; i++)
				{
					results.push_back(std::async(pci::testPciHeaderId));
					results.push_back(std::async(general::testLoopingThread));
					results.push_back(std::async(controller_registers::testControllerReset));
					results.push_back(std::async(commands::testNVMeCommandOpcodeInvalid));
					results.push_back(std::async(commands::testNVMeCommandParsing));
					results.push_back(std::async(commands::testNVMeFirmwareDownloadAndCommit));
					results.push_back(std::async(commands::testNVMeIo));
					results.push_back(std::async(commands::testNVMeQueueDeletionFailures));
					results.push_back(std::async(driver::testNoDataCommandViaDriver));
					results.push_back(std::async(driver::testReadCommandViaDriver));
					results.push_back(std::async(prp::testDifferentPRPSizes));
					results.push_back(std::async(prp::testDataIntoExistingPRP));
					results.push_back(std::async(logging::testAsserting));
				}

				bool retVal = true;
				for (auto &i : results)
				{
					retVal &= i.get();
				}

				return retVal;
			}

			void randomizePayload(Payload & payload)
			{
				// Can't afford to take the time to randomize a 128MB buffer... so just randomly flip some bytes
				UINT_32 jumpNum = (UINT_32)randInt((UINT_64)payload.getSize() / 4, (UINT_64)payload.getSize() / 2);
				for (UINT_32 i = 0; i < payload.getSize(); i += jumpNum)
				{
					payload.getBuffer()[i] = (BYTE)randInt(0, 0xFF);
				}
			}

			Payload getFirmwareImage(std::string firmwareRevision, size_t fileSizeInBytes)
			{
				ASSERT_IF_LT(fileSizeInBytes, sizeof(identify::structures::IDENTIFY_CONTROLLER::FR) + sizeof(FIRMWARE_EYE_CATCHER), "Cannot make firmware image with the given small size");
				ASSERT_IF_GT(firmwareRevision.size(), sizeof(identify::structures::IDENTIFY_CONTROLLER::FR), "Given firmware revision is sized too large to fit in the FR field");

				Payload retPayload((UINT_8*)FIRMWARE_EYE_CATCHER, sizeof(FIRMWARE_EYE_CATCHER));
				retPayload.append(Payload(fileSizeInBytes - sizeof(FIRMWARE_EYE_CATCHER) - sizeof(identify::structures::IDENTIFY_CONTROLLER::FR)));
				retPayload.append(Payload((UINT_8*)firmwareRevision.c_str(), firmwareRevision.size()));

				ASSERT_IF_NE(retPayload.getSize(), fileSizeInBytes, "FW image payload was incorrectly sized");

				return retPayload;
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
				UINT_32 BISTOffset = offsetof(cnvme::pci::header::PCI_HEADER, BIST);
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

				return true;
			}
		}

		namespace commands
		{
			bool testNVMeCommandParsing()
			{
				cnvme::command::NVME_COMMAND command = { 0 };
				command.DWord0Breakdown.OPC = (UINT_8)helpers::randInt(0, 100);
				command.DWord1 = (UINT_32)helpers::randInt(0, 0xFFFFFFFF);
				command.DWord10 = (UINT_32)helpers::randInt(0, 0xFF);
				command.DWord11 = (UINT_32)helpers::randInt(0, 0xFFF);
				command.DWord12 = (UINT_32)helpers::randInt(0, 0xFFFF);
				command.DWord13 = (UINT_32)helpers::randInt(0, 0xFFFFF);
				command.DWord14 = (UINT_32)helpers::randInt(0, 0xFFFFFF);
				command.DWord15 = (UINT_32)helpers::randInt(0, 0xFFFFFFF);

				std::string retStr = command.toString();
				FAIL_IF(!retStr.find(std::to_string(command.DWord0Breakdown.OPC)), "Generated OPC not found in parsing");
				FAIL_IF(!retStr.find(std::to_string(command.DWord1)), "Generated DWord1 not found in parsing");
				FAIL_IF(!retStr.find(std::to_string(command.DWord10)), "Generated DWord10 not found in parsing");
				FAIL_IF(!retStr.find(std::to_string(command.DWord11)), "Generated DWord11 not found in parsing");
				FAIL_IF(!retStr.find(std::to_string(command.DWord12)), "Generated DWord12 not found in parsing");
				FAIL_IF(!retStr.find(std::to_string(command.DWord13)), "Generated DWord13 not found in parsing");
				FAIL_IF(!retStr.find(std::to_string(command.DWord14)), "Generated DWord14 not found in parsing");
				FAIL_IF(!retStr.find(std::to_string(command.DWord15)), "Generated DWord15 not found in parsing");

				return true;
			}

			bool testNVMeCommandOpcodeInvalid()
			{
				cnvme::driver::Driver driver;

				UINT_32 BUF_SIZE = sizeof(cnvme::driver::DRIVER_COMMAND);
				Payload p(BUF_SIZE);
				BYTE* buffer = p.getBuffer();

				auto pDriverCommand = (cnvme::driver::PDRIVER_COMMAND)buffer;
				pDriverCommand->QueueId = ADMIN_QUEUE_ID;

				UINT_32 timeout = 5; // arbitrary
				pDriverCommand->Timeout = timeout;
				pDriverCommand->TransferDataDirection = cnvme::driver::NO_DATA;
				pDriverCommand->Command.DWord0Breakdown.OPC = 0xFE; // invalid.. hopefully

				driver.sendCommand(buffer, BUF_SIZE);

				auto status = pDriverCommand->CompletionQueueEntry.SC;

				FAIL_IF(status != constants::status::codes::generic::INVALID_COMMAND_OPCODE, "Controller did not fail invalid opcode correctly");

				return true;
			}

			bool testNVMeQueueDeletionFailures()
			{
				cnvme::driver::Driver driver;

				UINT_32 BUF_SIZE = sizeof(cnvme::driver::DRIVER_COMMAND);
				Payload p(BUF_SIZE);
				BYTE* buffer = p.getBuffer();

				auto pDriverCommand = (cnvme::driver::PDRIVER_COMMAND)buffer;
				pDriverCommand->QueueId = ADMIN_QUEUE_ID;

				UINT_32 timeout = 5; // arbitrary
				pDriverCommand->Timeout = timeout;
				pDriverCommand->TransferDataDirection = cnvme::driver::NO_DATA;
				pDriverCommand->Command.DWord0Breakdown.OPC = constants::opcodes::admin::DELETE_IO_COMPLETION_QUEUE;
				pDriverCommand->Command.DW10_DeleteIoQueue.QID = ADMIN_QUEUE_ID;

				driver.sendCommand(buffer, BUF_SIZE);

				auto status = pDriverCommand->CompletionQueueEntry.SC;
				FAIL_IF(status != constants::status::codes::specific::INVALID_QUEUE_IDENTIFIER, "Expected controller to fail IO queue deletion with admin queue ID, but did not receive INVALID_QUEUE_IDENTIFIER status");

				// Create CQ 1
				pDriverCommand->Command.DW10_CreateIoQueue.QSIZE = 0xF;
				pDriverCommand->Command.DW10_CreateIoQueue.QID = 1;
				pDriverCommand->Command.DW11_CreateIoCompletionQueue.IEN = 1;
				pDriverCommand->Command.DW11_CreateIoCompletionQueue.PC = 1;
				pDriverCommand->Command.DWord0Breakdown.OPC = constants::opcodes::admin::CREATE_IO_COMPLETION_QUEUE;
				driver.sendCommand(buffer, BUF_SIZE);

				status = pDriverCommand->CompletionQueueEntry.SC;
				FAIL_IF(status != 0, "Controller failed creating an io completion queue");

				// Create SQ 1
				pDriverCommand->Command.DW10_CreateIoQueue.QSIZE = 0xF;
				pDriverCommand->Command.DW10_CreateIoQueue.QID = 1;
				pDriverCommand->Command.DW11_CreateIoSubmissionQueue.PC = 1;
				pDriverCommand->Command.DW11_CreateIoSubmissionQueue.CQID = 1; // Link to CQ 1
				pDriverCommand->Command.DWord0Breakdown.OPC = constants::opcodes::admin::CREATE_IO_SUBMISSION_QUEUE;
				driver.sendCommand(buffer, BUF_SIZE);

				status = pDriverCommand->CompletionQueueEntry.SC;
				FAIL_IF(status != 0, "Controller failed creating an io completion queue");

				// Deleting CQ 1 should fail since we need to delete SQ 1 first
				pDriverCommand->Command.DWord0Breakdown.OPC = constants::opcodes::admin::DELETE_IO_COMPLETION_QUEUE;
				pDriverCommand->Command.DW10_DeleteIoQueue.QID = 1;
				driver.sendCommand(buffer, BUF_SIZE);

				status = pDriverCommand->CompletionQueueEntry.SC;
				FAIL_IF(status != constants::status::codes::specific::INVALID_QUEUE_DELETION, "Controller did not fail deleting a CQ before SQ");

				// Should be ok to delete SQ 1
				pDriverCommand->Command.DWord0Breakdown.OPC = constants::opcodes::admin::DELETE_IO_SUBMISSION_QUEUE;
				driver.sendCommand(buffer, BUF_SIZE);

				status = pDriverCommand->CompletionQueueEntry.SC;
				FAIL_IF(status != 0, "Controller did not allow deleting a SQ");

				// Now we can delete CQ 1
				pDriverCommand->Command.DWord0Breakdown.OPC = constants::opcodes::admin::DELETE_IO_COMPLETION_QUEUE;
				driver.sendCommand(buffer, BUF_SIZE);

				status = pDriverCommand->CompletionQueueEntry.SC;
				FAIL_IF(status != 0, "Controller did not allow deleting a CQ after its SQ was deleted");

				return true;
			}

			bool testNVMeIo()
			{
				cnvme::driver::Driver driver;

				Payload payload(8192); // generic large size
				auto pDriverCommand = (cnvme::driver::PDRIVER_COMMAND)payload.getBuffer();
				pDriverCommand->QueueId = ADMIN_QUEUE_ID;

				UINT_32 timeout = 5; // arbitrary
				pDriverCommand->Timeout = timeout;
				pDriverCommand->TransferDataDirection = cnvme::driver::NO_DATA;
				pDriverCommand->Command.DWord0Breakdown.OPC = constants::opcodes::admin::CREATE_IO_COMPLETION_QUEUE;

				// Create CQ 1
				pDriverCommand->Command.DW10_CreateIoQueue.QSIZE = 0xF;
				pDriverCommand->Command.DW10_CreateIoQueue.QID = 1;
				pDriverCommand->Command.DW11_CreateIoCompletionQueue.IEN = 1;
				pDriverCommand->Command.DW11_CreateIoCompletionQueue.PC = 1;
				driver.sendCommand(payload.getBuffer(), payload.getSize());

				FAIL_IF(!pDriverCommand->CompletionQueueEntry.succeeded(), "Controller failed creating an io completion queue");

				// Create SQ 1
				pDriverCommand->Command.DW10_CreateIoQueue.QSIZE = 0xF;
				pDriverCommand->Command.DW10_CreateIoQueue.QID = 1;
				pDriverCommand->Command.DW11_CreateIoSubmissionQueue.PC = 1;
				pDriverCommand->Command.DW11_CreateIoSubmissionQueue.CQID = 1; // Link to CQ 1
				pDriverCommand->Command.DWord0Breakdown.OPC = constants::opcodes::admin::CREATE_IO_SUBMISSION_QUEUE;
				driver.sendCommand(payload.getBuffer(), payload.getSize());

				FAIL_IF(!pDriverCommand->CompletionQueueEntry.succeeded(), "Controller failed creating an io completion queue");

				// Now we have IO Queue Pair 1
				pDriverCommand->QueueId = 1;
				pDriverCommand->TransferDataSize = 512; // default sector size

				// Write all 0xCD to the first sector, 0xDC to the second sector
				UINT_8 sector1Value = 0xCD;
				UINT_8 sector2Value = 0xDC;
				memset(&pDriverCommand->Command, 0, sizeof(pDriverCommand->Command));
				pDriverCommand->Command.NSID = 1;
				pDriverCommand->Command.DWord0Breakdown.OPC = constants::opcodes::nvm::WRITE;
				pDriverCommand->Command.DW12_IO.NLB = ZERO_BASED_FROM_ONE_BASED(1);
				pDriverCommand->Command.SLBA = 0; // first sector
				pDriverCommand->TransferDataDirection = cnvme::driver::WRITE;
				// set the data to 0xCD
				memset(&pDriverCommand->TransferData, sector1Value, 512);
				driver.sendCommand(payload.getBuffer(), payload.getSize());

				FAIL_IF(!pDriverCommand->CompletionQueueEntry.succeeded(), "Failed to write the first sector");

				// set the data to 0xDC (for the second sector)
				memset(&pDriverCommand->TransferData, sector2Value, 512);
				pDriverCommand->Command.SLBA = 1; // 2nd sector

				driver.sendCommand(payload.getBuffer(), payload.getSize());

				FAIL_IF(!pDriverCommand->CompletionQueueEntry.succeeded(), "Failed to write the second sector");

				// Read back, and confirm the data was what we expected
				pDriverCommand->TransferDataDirection = cnvme::driver::READ;
				pDriverCommand->Command.DWord0Breakdown.OPC = constants::opcodes::nvm::READ;
				pDriverCommand->Command.DW12_IO.NLB = ZERO_BASED_FROM_ONE_BASED(2);
				pDriverCommand->Command.SLBA = 0; // start at first sector
				pDriverCommand->TransferDataSize = 1024; // 2 sectors

				driver.sendCommand(payload.getBuffer(), payload.getSize());

				FAIL_IF(!pDriverCommand->CompletionQueueEntry.succeeded(), "Failed to read back the 2 sectors");

				for (size_t i = 0; i < 1024; i++)
				{
					UINT_8 cmpVal = 0xFF;
					if (i < 512)
					{
						cmpVal = sector1Value;
					}
					else
					{
						cmpVal = sector2Value;
					}
					FAIL_IF(pDriverCommand->TransferData[i] != cmpVal, "Comparison failure: the read data didn't match what was written");
				}

				FAIL_IF(pDriverCommand->TransferData[1024] != 0, "We specified that we were reading 1024 bytes of data and yet the byte after that is set to something other than 0. Did we read too much?");

				// Call Format NVM, and confirm the data doesn't match anymore
				memset(&pDriverCommand->Command, 0, sizeof(pDriverCommand->Command));
				pDriverCommand->Command.DWord0Breakdown.OPC = constants::opcodes::admin::FORMAT_NVM;
				pDriverCommand->Command.NSID = 1;
				pDriverCommand->QueueId = ADMIN_QUEUE_ID;
				pDriverCommand->TransferDataDirection = cnvme::driver::NO_DATA;
				pDriverCommand->TransferDataSize = 0;

				driver.sendCommand(payload.getBuffer(), payload.getSize());
				FAIL_IF(!pDriverCommand->CompletionQueueEntry.succeeded(), "Format NVM failed");

				// Read back, and confirm the data was not what we original wrote
				memset(&pDriverCommand->Command, 0, sizeof(pDriverCommand->Command));
				pDriverCommand->TransferDataDirection = cnvme::driver::READ;
				pDriverCommand->QueueId = 1;
				pDriverCommand->Command.DWord0Breakdown.OPC = constants::opcodes::nvm::READ;
				pDriverCommand->Command.NSID = 1;
				pDriverCommand->Command.DW12_IO.NLB = ZERO_BASED_FROM_ONE_BASED(2);
				pDriverCommand->TransferDataSize = 1024; // 2 sectors

				driver.sendCommand(payload.getBuffer(), payload.getSize());
				FAIL_IF(!pDriverCommand->CompletionQueueEntry.succeeded(), "Failed to read back data after Format NVM");

				for (size_t i = 0; i < 1024; i++)
				{
					FAIL_IF(pDriverCommand->TransferData[i] != 0, "Format NVM didn't zero out the area we wrote to");
				}

				return true;
			}

			bool testNVMeFirmwareDownloadAndCommit()
			{
				cnvme::driver::TestDriver driver;

				// Test fw/dl activate on reset
				std::string updatedFirmware = "NEWFW001";
				std::string startingFirmware = driver.getFirmwareString();
				FAIL_IF(!driver.firmwareImageDownload(0, helpers::getFirmwareImage(updatedFirmware, 4096)).CompletionQueueEntry.succeeded(), "Failed to download the FW image");
				FAIL_IF(driver.getFirmwareString() != startingFirmware, "FW changed without a commit being sent");
				FAIL_IF(!driver.firmwareCommit(constants::commands::fw_commit::commit_action::REPLACE_IN_SLOT_AND_ACTIVATE_ON_RESET, 4).CompletionQueueEntry.succeeded(), "Failed to commit with a CA to replace and activate on reset");
				FAIL_IF(driver.getFirmwareString() != startingFirmware, "FW changed without the needed reset being sent");
				FAIL_IF(!driver.controllerReset(), "Controller reset failed!");
				FAIL_IF(driver.getFirmwareString() != updatedFirmware, "FW did not change after reset");

				// Test fw/dl commit to slot then activate
				updatedFirmware = "NEWFW002";
				startingFirmware = driver.getFirmwareString();
				size_t fwSize = 4096;
				auto fwPayloadSegments = helpers::getFirmwareImage(updatedFirmware, fwSize).split(fwSize / 2);
				FAIL_IF(!driver.firmwareImageDownload(0, fwPayloadSegments[0]).CompletionQueueEntry.succeeded(), "Failed to download the FW image part 1/2");
				FAIL_IF(!driver.firmwareImageDownload((UINT_32)DWORDS_FROM_BYTES(fwPayloadSegments[0].getSize()), fwPayloadSegments[1]).CompletionQueueEntry.succeeded(), "Failed to download the FW image part 2/2");
				FAIL_IF(driver.getFirmwareString() != startingFirmware, "FW changed without a commit being sent");
				FAIL_IF(!driver.firmwareCommit(constants::commands::fw_commit::commit_action::REPLACE_IN_SLOT_NO_ACTIVATE, 5).CompletionQueueEntry.succeeded(), "Failed to commit with a CA to replace in slot without activation");
				FAIL_IF(driver.getFirmwareString() != startingFirmware, "FW changed without actually activating the replaced slot");
				FAIL_IF(!driver.firmwareCommit(constants::commands::fw_commit::commit_action::ACTIVATE_GIVEN_SLOT_NOW, 5).CompletionQueueEntry.succeeded(), "Failed to activate the FW in slot 5");
				FAIL_IF(driver.getFirmwareString() != updatedFirmware, "FW did not change after activation");
				FAIL_IF(!driver.controllerReset(), "Controller reset failed!");
				FAIL_IF(driver.getFirmwareString() != updatedFirmware, "FW changed after reset when it shouldn't have");

				// Test fw/dl can return overlapping range
				FAIL_IF(!driver.firmwareImageDownload(0, fwPayloadSegments[0]).CompletionQueueEntry.succeeded(), "Failed to download the FW image part 1/2");
				FAIL_IF(driver.firmwareImageDownload(1, fwPayloadSegments[1]).CompletionQueueEntry.SC != constants::status::codes::specific::OVERLAPPING_RANGE, "Overlapping range was not returned");

				// Test commit fails with an invalid image
				FAIL_IF(!driver.firmwareImageDownload(0, Payload(4096)).CompletionQueueEntry.succeeded(), "Failed to download empty FW image");
				FAIL_IF(driver.firmwareCommit(constants::commands::fw_commit::commit_action::REPLACE_IN_SLOT_NO_ACTIVATE, 5).CompletionQueueEntry.SC != constants::status::codes::specific::INVALID_FIRMWARE_IMAGE, "FW did not return invalid firmware image on an invalid image");

				return true;
			}
		}

		namespace driver
		{
			bool testNoDataCommandViaDriver()
			{
				cnvme::driver::Driver driver;
				UINT_32 BUF_SIZE = sizeof(cnvme::driver::DRIVER_COMMAND);
				Payload p(BUF_SIZE);
				BYTE* buffer = p.getBuffer();

				auto pDriverCommand = (cnvme::driver::PDRIVER_COMMAND)buffer;
				pDriverCommand->QueueId = ADMIN_QUEUE_ID;

				UINT_32 timeout = 5; // arbitrary
				pDriverCommand->Timeout = timeout;
				pDriverCommand->TransferDataDirection = cnvme::driver::NO_DATA;
				pDriverCommand->Command.DWord0Breakdown.OPC = cnvme::constants::opcodes::admin::KEEP_ALIVE;

				for (UINT_32 i = 0; i < 8; i++)
				{
					driver.sendCommand(buffer, BUF_SIZE);
					FAIL_IF(pDriverCommand->DriverStatus != cnvme::driver::SENT_SUCCESSFULLY, "Command did not send successfully");
					FAIL_IF(pDriverCommand->TransferDataDirection != cnvme::driver::NO_DATA, "Command transfer data direction somehow changed");
					FAIL_IF(pDriverCommand->Timeout != timeout, "Timeout value should not have changed");
					FAIL_IF(pDriverCommand->TransferDataSize != 0, "Data transfer size should not have changed");
					FAIL_IF(pDriverCommand->Command.DWord0Breakdown.CID != i, "Command's CID should match loop iteration");
					FAIL_IF(pDriverCommand->CompletionQueueEntry.CID != pDriverCommand->Command.DWord0Breakdown.CID, "Completion CID should match that of the submission");
					FAIL_IF(pDriverCommand->CompletionQueueEntry.SC != 0, "Status wan't success for sending Identify Controller");
					FAIL_IF(pDriverCommand->CompletionQueueEntry.SCT != 0, "Status code type wan't generic for sending Identify Controller");
					FAIL_IF(pDriverCommand->Command.DPTR.DPTR1 != 0, "DPTR1 was somehow not 0 even though this is non-data");
					FAIL_IF(pDriverCommand->Command.DPTR.DPTR2 != 0, "DPTR2 was somehow not 0 even though this is non-data");
				}

				return true;
			}

			bool testReadCommandViaDriver()
			{
				cnvme::driver::Driver driver;

				UINT_32 BUF_SIZE = sizeof(cnvme::identify::structures::IDENTIFY_CONTROLLER) + sizeof(cnvme::driver::DRIVER_COMMAND);
				Payload p(BUF_SIZE);
				BYTE* buffer = p.getBuffer();

				auto pDriverCommand = (cnvme::driver::PDRIVER_COMMAND)buffer;
				pDriverCommand->QueueId = ADMIN_QUEUE_ID;

				UINT_32 timeout = 5; // arbitrary
				pDriverCommand->Timeout = timeout;
				pDriverCommand->TransferDataSize = sizeof(cnvme::identify::structures::IDENTIFY_CONTROLLER);
				pDriverCommand->TransferDataDirection = cnvme::driver::READ;
				pDriverCommand->Command.DWord0Breakdown.OPC = cnvme::constants::opcodes::admin::IDENTIFY;
				pDriverCommand->Command.DW10_Identify.CNS = cnvme::constants::commands::identify::cns::CONTROLLER;

				for (UINT_32 i = 0; i < 8; i++)
				{
					driver.sendCommand(buffer, BUF_SIZE);
					FAIL_IF(pDriverCommand->DriverStatus != cnvme::driver::SENT_SUCCESSFULLY, "Command did not send successfully");
					FAIL_IF(pDriverCommand->TransferDataDirection != cnvme::driver::READ, "Command transfer data direction somehow changed");
					FAIL_IF(pDriverCommand->Timeout != timeout, "Timeout value should not have changed");
					FAIL_IF(pDriverCommand->TransferDataSize != sizeof(cnvme::identify::structures::IDENTIFY_CONTROLLER), "Data transfer size should not have changed");
					FAIL_IF(pDriverCommand->Command.DWord0Breakdown.CID != i, "Command's CID should match loop iteration");
					FAIL_IF(pDriverCommand->CompletionQueueEntry.CID != pDriverCommand->Command.DWord0Breakdown.CID, "Completion CID should match that of the submission");
					FAIL_IF(pDriverCommand->CompletionQueueEntry.SC != 0, "Status wan't success for sending Identify Controller");
					FAIL_IF(pDriverCommand->CompletionQueueEntry.SCT != 0, "Status code type wan't generic for sending Identify Controller");

					auto pIdentifyController = (cnvme::identify::structures::PIDENTIFY_CONTROLLER)pDriverCommand->TransferData;
					FAIL_IF(std::string(pIdentifyController->MN) != std::string(DEFAULT_MODEL), "Model didn't match expectations");
					FAIL_IF(std::string(pIdentifyController->SN) != std::string(DEFAULT_SERIAL), "Serial didn't match expectations");
					FAIL_IF(std::string(pIdentifyController->FR) != std::string(DEFAULT_FIRMWARE), "Firmware didn't match expectations");
				}

				return true;
			}
		}

		namespace prp
		{
			bool testDifferentPRPSizes()
			{
				// Wide range of sizes to test les than a page, equal to a page, two pages, 
				//  and a scenario to queue a chained PRP list.
				std::vector<UINT_32> dataXfrSizes = { 512, 4095, 4096, 4097, 8192, 8193, 4096 * 100 };

				// memory page size is is 2 ^ (12 + CC.MPS). CC.MPS is 4 bits.
				//  Test all valid page sizes
				std::vector<UINT_32> memoryPageSizes;
				for (int i = 0; i <= 0b1111; i++)
				{
					memoryPageSizes.push_back((UINT_32)std::pow(2, 12 + i));
				}

				for (UINT_32 dataSize : dataXfrSizes)
				{
					Payload payload(dataSize);
					helpers::randomizePayload(payload);

					for (UINT_32 pageSize : memoryPageSizes)
					{
						PRP prp(payload, pageSize);

						FAIL_IF(payload != prp.getPayloadCopy(), "With pageSize (" + std::to_string(pageSize) + \
							") and payload size (" + std::to_string(dataSize) + "), the PRP's payload didn't match the original!");
					}
				}

				return true;
			}

			bool testDataIntoExistingPRP()
			{
				// Wide range of sizes to test les than a page, equal to a page, two pages, 
				//  and a scenario to queue a chained PRP list.
				std::vector<UINT_32> dataXfrSizes = { 512, 4095, 4096, 4097, 8192, 8193, 4096 * 100 };

				// memory page size is is 2 ^ (12 + CC.MPS). CC.MPS is 4 bits.
				//  Test all valid page sizes
				std::vector<UINT_32> memoryPageSizes;
				for (int i = 0; i <= 0b1111; i++)
				{
					memoryPageSizes.push_back((UINT_32)std::pow(2, 12 + i));
				}

				for (UINT_32 dataSize : dataXfrSizes)
				{
					Payload payloadWithData(dataSize);
					Payload payloadWithDataTooLarge = payloadWithData;
					payloadWithDataTooLarge.append(Payload(512));
					helpers::randomizePayload(payloadWithData);
					Payload payloadWithoutData(dataSize);

					for (UINT_32 pageSize : memoryPageSizes)
					{
						PRP prp(payloadWithoutData, pageSize);
						prp.placePayloadInExistingPRPs(payloadWithData);

#if _DEBUG
						bool didAssert = false;
#else
						bool didAssert = true; // preset to True since release will not assert.
#endif // _DEBUG
						_START_ASSERT_QUIET();
						try
						{
							FAIL_IF_AND_HIDE_LOG(prp.placePayloadInExistingPRPs(payloadWithDataTooLarge), "Placing a larger than allocated Payload into PRPs should have failed!");
						}
						catch (...)
						{
							didAssert = true;
						}
						_END_ASSERT_QUIET();

						FAIL_IF(!didAssert, "There should have been an assert on an invalid payload size");

						// Status will be messed with here. So clear.
						cnvme::logging::theLogger.clearStatus();

						FAIL_IF(payloadWithData != prp.getPayloadCopy(), "With pageSize (" + std::to_string(pageSize) + \
							") and payload size (" + std::to_string(dataSize) + "), the PRP's payload didn't match the original!");
					}
				}

				return true;
			}
		}

		namespace logging
		{
			bool testAsserting()
			{
				_START_ASSERT_QUIET();

				bool retVal = false;
				try
				{
					ASSERT("asserting.");
				}
				catch (...)
				{
					retVal = true;
				}

#ifdef _DEBUG
				FAIL_IF(!retVal, "ASSERT() didn't lead to an exception, even though this is a debug");
#else // !_DEBUG
				FAIL_IF(retVal, "ASSERT() led to an exception, even though this is a release");
#endif // !_DEBUG
				try
				{
					ASSERT_IF(true, "Should assert");
				}
				catch (...)
				{
					retVal = true;
				}
#ifdef _DEBUG
				FAIL_IF(!retVal, "ASSERT_IF() didn't lead to an exception, even though this is a debug");
#else // !_DEBUG
				FAIL_IF(retVal, "ASSERT_IF() led to an exception, even though this is a release");
#endif // !_DEBUG

				_END_ASSERT_QUIET();

				return true;
			}
		}
	}
}

