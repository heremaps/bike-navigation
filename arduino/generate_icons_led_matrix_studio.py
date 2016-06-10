#! /usr/bin/python3

#
# Copyright (c) 2015 HERE Europe B.V.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

import sys

def parseFile(filename, outName):
    file = open(filename, 'r')
    frameNum = 0;
    processingFrame = False
    lineOut = ""
    row = 0
    print("const char %sFrames[][MATRIX_HEIGHT] = {" % outName)
    for line in file:
        line = line.strip();
        if line.startswith("r:"):
            if processingFrame:
                lineOut += ", "
                lineOut += "0x%02x" % (readRow(line.lstrip('r:')))
            else:
                if frameNum > 0:
                    lineOut += ","
                    print(lineOut)
                lineOut = "    {0x%02x" % (readRow(line.lstrip('r:')))
                processingFrame = True
                frameNum = frameNum + 1
        elif processingFrame:
            processingFrame = False
            lineOut += "}"
    if frameNum > 0:
        print(lineOut)
    print("};")
    print("const Icon %s(%d, %sFrames[0]);" % (outName, frameNum, outName))

def readRow(line):
    byte = 0;
    rowBits = line.split()
    for bit in rowBits:
        if bit == "000000":
            byte = byte << 1
        else:
            byte = (byte << 1) | 1
    return byte

def main():
    if len(sys.argv) > 2:
        parseFile(sys.argv[1], sys.argv[2])
    else:
        print("Usage: %s filename outname" % sys.argv[0]) 
        print("\tfilename: LED patterns file generated with LED Matrix Studio (http://maximumoctopus.com/electronics/builder.htm)")
        print("\toutname: name of variable where LED patterns will be stored in C++ code")
if __name__ == '__main__':
    main()
