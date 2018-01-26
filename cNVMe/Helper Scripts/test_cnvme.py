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

def test_exe():
    if os.environ['OUTPUT_TYPE'] != 'EXE':
        pytest.skip("Skipping test since this is a dll build")
        
    assert findAndRunCnvme() == 0

def test_dll():
    if os.environ['OUTPUT_TYPE'] != 'DLL':
        pytest.skip("Skipping test since this is an exe build")
        
    cnvme = getCnvmePath(getDllExtension())
    assert cnvme
    print 'Found cNVMe dll: %s' % cnvme
    dll = CDLL(cnvme)
    assert dll.Initialize() == 0
    assert dll.Uninitialize() == 0
    assert dll.Uninitialize() != 0
