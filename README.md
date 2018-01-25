# cNVMe
An NVMe Device Simulation Library.

This is a WIP simulation of the protocol-layer of an NVMe Memory Device. The goal is to be able to send NVMe commands down and get full protocol responses. The simulation includes a register level with basic PCIe Controller Registers and simplified Doorbell Registers. It also contains a "Driver" with some abstractions to make it much easier to send commands to the underlying controller. cNVMe is really cool!

## Supported NVMe Specification Commands
### Admin Commands
* Create IO Completion Queue
* Create IO Submission Queue
* Delete IO Completion Queue
* Delete IO Submission Queue
* Delete IO Submission Queue
* Format NVM
* Identify (Namespace, Controller, Namespace Attached List, Namespace Active List)
* Keep Alive

### NVM Commands
* Flush
* Read
* Write

Linux Build Status: [![Build Status](https://travis-ci.org/intel/cNVMe.svg?branch=master)](https://travis-ci.org/intel/cNVMe)

Windows Build Status: [![Build status](https://ci.appveyor.com/api/projects/status/svfanibbsfm94d4f/branch/master?svg=true)](https://ci.appveyor.com/project/csm10495/cnvme-v65dl/branch/master)

The original (and now legacy) repository for this project can be found here, for historical purposes: [csm10495/cNVMe](https://github.com/csm10495/cNVMe)

# Command Response API (CRAPI)
### Rationale (Informative)
Users may be interested to see what happens in a surrounding ecosystem if a certain command fails, passes, or returns data in a Vendor-Unique format. To provide for this need, cNVMe has the Command Response API (also known as CRAPI).

## Format
The format for using the API is actually pretty simple. Provide the Controller a file path via the public method setCommandResponseFilePath(). Then, this file will be invoked via a system call whenever cNVMe gets an NVMe command, before cNVMe would normally process it. Wherever the Command Response API File (CRAPI-F) is, is an important location, because cNVMe will place binary files there, that will be useful to the CRAPI-F. 

### Parameters to the CRAPI-F
- Param 1 - The Submission Queue ID that this command was sent to.

### Local Files
Before calling the CRAPI-F, cNVMe will place some files in the local directory for the CRAPI-F's usage:
- data_payload.bin - A binary file of the transfer data for the command. This file includes the assumed data length for the command based on parameters within the DWords. Sending a command that relies on a namespace size, when the namespace is not attached will result in an assumption of a 512-byte sector size.
- command.bin - A 64-Byte binary of the raw NVMe command. Note that modifying DPTR incorrectly will lead to inconsistent behavior.
- completion.bin - A 16-Byte binary of the raw NVMe completion for the command (Note that cNVMe will fill in the proper CID/SQID in the posted Completion Queue Entry).

Note that none of the given files may be resized by the CRAPI-F. The data_payload.bin is pre-sized to the maximum amount of data supported by the current transfer

## Supported Return Codes
The Command Response API File is expected to return one of the following status codes to denote 'what happened' and furthermore how cNVMe should proceed.
All codes that are not supported will lead to a specific assert being raised.
- 0 - Command Handled by CRAPI-F - cNVMe will use data_payload.bin/completion.bin/command.bin and not process the command via its own logic
- 1 - Command Handled by cNVMe - cNVMe will process the command entirely (nop from the CRAPI-F's POV)
- 9 - Command Handled by CRAPI-F - cNVMe shall raise an assert with a message specified in data_payload.bin as ASCII. Be sure to use a trailing NULL character for the message.
