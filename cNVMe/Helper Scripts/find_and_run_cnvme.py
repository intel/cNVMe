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
        
if __name__ == '__main__':
    cnvmeTopDir = os.path.abspath(os.path.join(os.path.abspath(__file__), os.pardir, os.pardir, os.pardir)) # up 3 dirs brings us to cNVMe\
    for root, dirs, files in os.walk(cnvmeTopDir):
        for file in files:
            filePath = os.path.join(root, file)
            if file == ('cNVMe.%s' % getExeExtension()):
                print 'Found cNVMe: %s' % filePath
                sys.exit(subprocess.call(filePath, shell=True))
                
    print 'Couldn\'t find cNVMe'
    sys.exit(-1) # faillure to find exe
                