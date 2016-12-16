'''
Brief:
    Script to go from NVMe spec structure to a structure in C++

Author(s):
    Charles Machalow
'''

from cNVMestructToString import setClipboard, runWithText

DEFINITION = \
'''typedef struct %s
{
    %s
    std::string toString() const;
} %s, *P%s;
static_assert(sizeof(%s) == %d, \"%s should be %d byte(s) in size.\");
'''

def getTypeAndName(bits, abbreviation):
    if ':' in bits:
        b1, b2 = map(int, bits.split(":"))
        s = b1 - b2 + 1
    else:
        s = 1

    if s == 64:
        return "UINT_64 %s" % abbreviation
    elif s == 32:
        return "UINT_32 %s" % abbreviation
    elif s == 16:
        return "UINT_16 %s" % abbreviation
    elif s == 8:
        return "UINT_8 %s" % abbreviation

    # bitfields... these may be a bit off
    elif s > 32:
        return "UINT_64 %s : %d" % (abbreviation, s)
    elif s > 16:
        return "UINT_32 %s : %d" % (abbreviation, s)
    elif s > 8:
        return "UINT_16 %s : %d" % (abbreviation, s)
    else:
        return "UINT_8 %s : %d" % (abbreviation, s)

def getVarsString(fields):
    retStr = ""
    for typeAndName, description in fields:
        retStr += "%s; // %s\n" % (typeAndName, description)

    return retStr

def getPciName(description):
    #return ("PCI_%s" % (description.replace(" ", "_").upper())).replace("PCI_PCI_", "PCI_")
    return ("%s" % (description.replace(" ", "_").upper())).replace("PCI_PCI_", "PCI_")

if __name__ == '__main__':
    line = input()
    if 'Offset' in line and ':' in line:
        line = line.replace(" Bits Type Reset Description", "").replace(" Bit Type Reset Description", "").replace("MSI-X", "MSI X").replace("  ", " ")
        offset = line.split('Offset')[1].split(':')[0].replace("h:", "").replace("h", "").strip()
        topAbbreviation = line.split(":")[1].split()[0].strip()
        topDescription = line.split("-")[1].replace("(Optional)", "").replace(", ", " ").replace("/", " ").strip().replace("  ", " ").replace("  ", " ").replace("  ", " ")
    else:
        raise Exception("Incorrect format of first line.")

    while True:
        line = input()
        num = line.split(":")[0]
        try:
            int(num)
            print ('num is %s' % num)
            break
        except:
            print ('except!')
            pass

    bits = line

    totalByteSize = 0
    fields = []
    rsvdCount = 0
    while True:
        if bits is None:
            print ('read in bits')
            bits = input()


        if '.' in bits:
            break

        # look for largest bits to know largest size
        totalByteSize = max(totalByteSize, int((int(bits.split(":")[0]) + 1) / 8))

        theType = input().strip() # ignored
        if theType.endswith("/"):
            theType = input() # read one more line

        reset = input() # ignored

        if 'Impl' in reset and 'Spec' not in reset:
            reset = input() # read one more line

        totalDescription = input()
        description = totalDescription.split("(")[0].strip().rstrip(".").replace("_", " ")
        try:
            abbreviation = totalDescription.split("(")[1].split(")")[0].strip() # between parenthesis
        except:
            abbreviation = "RSVD%d" % rsvdCount
            rsvdCount += 1
        field = (getTypeAndName(bits, abbreviation), description)
        fields.append(field)
        bits = None

    pciName = getPciName(topDescription)
    fields = reversed(fields)
    outStr = DEFINITION % (pciName, getVarsString(fields), pciName, pciName, pciName, totalByteSize, topAbbreviation, totalByteSize)
    print(outStr)
    setClipboard(outStr)

    input("Press Enter to get the toString().")

    runWithText(outStr, offset)




