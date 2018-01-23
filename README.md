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

# VU Command Processing API
### Rationale (Informative)
Users may be interested to see what happens in a surrounding ecosystem if a certain command fails, passes, or returns data in a Vendor-Unique format. To provide for this need, cNVMe has the VU Command Processing API... (If I could come up with a better name/acronym I would.) 

## Format
The format for using the API is actually pretty simple. Provide the Controller a file path via the public method setVuCommandProcessingFilePath(). Then, this file will be invoked via a system call whenever cNVMe gets an NVMe command, before cNVMe would normally process it. Wherever the VU Command Processing File (VUCPF) is, is an important location, because cNVMe will place binary files there, that will be useful to the VUCPF. 

### Local Files
Before calling the VU Command Processing File, cNVMe will place some files in the local directory for the VUCPF:
- data_payload.bin - Will be a binary file of the transfer data for the command. This may (pending a will be re-read by cNVMe before being passed back via the completion queue.
- command.bin - Will be the 64-Byte NVMe command.
- completion.bin - Will be the 16-Byte completion for the NVMe command (Note that cNVMe will fill in the proper CID/SQID).

## Supported Return Codes
The VU Command Processing File is expected to return one of the following status codes to denote 'what happened' and cNVMe should proceed.
All codes that are not supported will lead to a specific assert bia raised.
- 0 - Handled by VUCPF - cNVMe will use data.bin/completion.bin/command.bin and not process the command via its own logic
- 1 - Handled by cNVMe - cNVMe will process the command entirely (nop from the VUCPF's POV)
- 9 - Handled by VUCPF - cNVMe shall raise an assert with a message specified in data.bin as ASCII. Be sure to use a trailing NULL character for the message.
