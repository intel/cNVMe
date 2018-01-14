'''
Brief:
    Simple script to build cNVMe on Linux... because I don't really like Makefiles.
'''
import argparse
import os
import subprocess

VALID_BITNESS = 32, 64
VALID_CONFIG_TYPE = 'Debug', 'Release'
VALID_OUTPUT_TYPE = 'DLL', 'EXE'

THIS_FOLDER = os.path.abspath(os.path.dirname(os.path.abspath(__file__)))

def build(bitness, configType, outputType):
    '''
    Brief:
        Simple function to build cNVMe via g++/Linux
    '''
    gppArgs = '*.cpp -w -std=c++11 -fpermissive -pthread -fPIC '

    if bitness not in VALID_BITNESS:
        raise ValueError("Invalid bitness")
    else:
        gppArgs += "-m%d " % bitness

    if configType not in VALID_CONFIG_TYPE:
        raise ValueError("Invalid config type")
    elif configType == 'Debug':
        gppArgs += '-g -O0 -D_DEBUG ' # define preprocessor directive

    if outputType not in VALID_OUTPUT_TYPE:
        raise ValueError("Invalid output type.")
    elif outputType == 'DLL':
        gppArgs += '-DDLL_BUILD -shared '
        ext = 'so'
    else:
        ext = 'out'

    print ("About to build cNVMe as: %d-bit %s %s" % (bitness, configType, outputType))

    gppArgs += "-o cNVMe%d.%s" % (bitness, ext) 

    origFolder = os.getcwd()
    try:
        os.chdir(THIS_FOLDER)
        cmd = 'g++ %s' % gppArgs
        print ("About to call: %s" % cmd)
        subprocess.check_call(cmd, shell=True)
    finally:
        os.chdir(origFolder)

    print ('Build Complete!')

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("-bitness", "-b", help="Bitness to build cNVMe for (32 or 64)", type=int, default=64)
    parser.add_argument("-config_type", "-c", help="Config type to build (Release or Debug)", type=str, default="Debug")
    parser.add_argument("-output_type", "-o", help="Output binary type (EXE or DLL)", type=str, default='EXE')
    parser.add_argument("-clean", help="Clean all outputs (and don't build)", action='store_true')
    args = parser.parse_args()

    if args.clean:
        origFolder = os.getcwd()
        try:
            os.chdir(THIS_FOLDER)
            cmd = 'rm -f *.out *.o *.so'
            print ("About to call: %s" % cmd)
            subprocess.check_call(cmd, shell=True)
            print ('Clean Complete!')
        finally:
            os.chdir(origFolder)
    else:
        build(args.bitness, args.config_type, args.output_type)