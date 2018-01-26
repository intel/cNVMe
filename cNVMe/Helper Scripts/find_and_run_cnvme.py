'''
Brief:
    Quick script to find / run cNVMe
        Returns the success/failure of main()
        
Author:
    Charles Machalow
'''
import os
import subprocess
import sys

def getExeExtension():
    if os.name == 'nt':
        return 'exe'
    else:
        return 'out'

def getDllExtension():
    if os.name == 'nt':
        return 'dll'
    else:
        return 'so'

def getCnvmePath(ext):
    cnvmeTopDir = os.path.abspath(os.path.join(os.path.abspath(__file__), os.pardir, os.pardir, os.pardir)) # up 3 dirs brings us to cNVMe\
    for root, dirs, files in os.walk(cnvmeTopDir):
        for file in files:
            filePath = os.path.join(root, file)
            if file.startswith('cNVMe') and file.endswith(ext):
                return filePath

    return False

def findAndRunCnvme():
    cnvme = getCnvmePath(getExeExtension())
    if cnvme:
        print ('Found cNVMe: %s' % cnvme)
        return subprocess.call(cnvme, shell=True)
            
    print ('Couldn\'t find cNVMe executable')
    return -1 # failure to find exe

if __name__ == '__main__':
    sys.exit(findAndRunCnvme())