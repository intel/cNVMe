'''
Brief:
    Command Response API file example

Author(s):
    Charles Machalow
'''

import os
import sys

THIS_DIRECTORY = os.path.abspath(os.path.dirname(__file__))
COMMAND_FILE = os.path.join(THIS_DIRECTORY, 'command.bin')
COMPLETION_FILE = os.path.join(THIS_DIRECTORY, 'completion.bin')
DATA_PAYLOAD_FILE = os.path.join(THIS_DIRECTORY, 'data_payload.bin')

COMPLETION_SIZE = 16 # bytes / 4 DWs

EXIT_CODE_CRAPI_WILL_HANDLE = 0
EXIT_CODE_CNVME_WILL_HANDLE = 1

def getNVMeCommand():
    with open(COMMAND_FILE, 'rb') as f:
        return map(ord, f.read())

def failTheCommand():

    # write all FFs to the completion... which should lead to a failure of some sort
    with open(COMPLETION_FILE, 'wb') as f:
        f.write(bytearray([0xFF] * COMPLETION_SIZE))

    sys.exit(EXIT_CODE_CRAPI_WILL_HANDLE)


if __name__ == '__main__':
    nvmeCommand = getNVMeCommand()
    if nvmeCommand[0] != 0xFF:
        print 'CRAPI will fail this command'
        failTheCommand()

    print 'CRAPI will let cNVMe process this command'
    sys.exit(EXIT_CODE_CNVME_WILL_HANDLE)

