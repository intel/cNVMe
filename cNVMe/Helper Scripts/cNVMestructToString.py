'''
Brief:
    Script to paste in the middle of a structure and get out a ToString text block

Author(s):
    Charles Machalow
'''

import os
import tempfile

TO_STRING = \
'''std::string %s::toString() const
{
    std::string retStr;
    retStr += "%s (%s / Offset %s):\\n";
    %sreturn retStr;
}
'''
RET_STR_LINE = "retStr += strings::toString(ToStringParams(%s, \"%s\"));\n"

def setClipboard(txt):
    if os.name == 'nt':
        with tempfile.NamedTemporaryFile('w', delete=False) as f:
            f.write(txt)
            n = f.name

        os.system('clip < %s' % n)
    else:
        raise Exception("Not supported on non-Windows platform.")

def die():
    os.system("taskkill /f /PID %d" % os.getpid())

def classNameToCleanName(className):
    className = className.replace("_", " ").title().replace("Pci", "PCI")
    return className

def getRetStrLines(params):
    retStr = ""
    for abbreviation, description in params:
        retStr += RET_STR_LINE % (abbreviation, description)

    return retStr

def runWithText(txt, structHexOffset):
    outStr = ""
    params = [] # list of varName, desc

    for line in txt.splitlines():
        line = line.strip()

        if 'typedef struct' in line:
            className = line.split(" ")[-1]

        if ';' in line and '_' in line and '*' not in line and 'const;' not in line and "==" not in line:
            theType = line.split(' ')[0]
            varName = line.split(' ')[1].replace(";", "").strip()
            commentSplit = line.split('//')
            if len(commentSplit) == 2:
                description = commentSplit[-1].strip()
            else:
                description = "Unknown"

            params.append((varName, description,))

        if 'assert' in line and 'should' in line:
            abbreviation = line.split("should")[0].split("\"")[1].strip()
            break

    try:
        int(structHexOffset, 16)
        structHexOffset = '0x' + structHexOffset
    except:
        if '+' in structHexOffset:
            hexNum = "0x" + structHexOffset.split("+")[1].strip().replace("h", "")
            structHexOffset = structHexOffset.split("+")[0] + " + " + hexNum
            structHexOffset = structHexOffset.replace("  ", " ")
        pass # not a number

    outStr = TO_STRING % (className, classNameToCleanName(className), abbreviation, structHexOffset, getRetStrLines(params))
    print (outStr)
    setClipboard(outStr)

def inputGenerator():
    while True:
        yield input()

if __name__ == "__main__":
    txt = object()
    txt.splitlines = inputGenerator
    runWithText(txt, "??")

