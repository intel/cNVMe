'''
Brief:
    Script to do a basic test of cNVMe
        
Author:
    Charles Machalow
'''

import os
import pytest
from ctypes import CDLL
from find_and_run_cnvme import findAndRunCnvme, getCnvmePath, getDllExtension

def getOutputType():
    if os.name != 'nt': # travis
        return os.environ['OUTPUT_TYPE']

    # appveyor
    config = os.environ['CONFIGURATION']
    if 'DLL' in config:
        return 'DLL'

    return 'EXE'

def test_exe():
    if getOutputType() != 'EXE':
        pytest.skip("Skipping test since this is a dll build")
        
    assert findAndRunCnvme() == 0

def test_dll():
    if getOutputType() != 'DLL':
        pytest.skip("Skipping test since this is an exe build")
        
    cnvme = getCnvmePath(getDllExtension())
    assert cnvme
    print 'Found cNVMe dll: %s' % cnvme
    dll = CDLL(cnvme)
    assert dll.Initialize() == 0
    assert dll.Uninitialize() == 0
    assert dll.Uninitialize() != 0
