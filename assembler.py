import sys
import re
import struct

if len(sys.argv) == 1:
    print("Usage: {} <assembly>".format(sys.argv[0]))
    exit(0)

instruction_map = [
    "ADD",
    "SUB",
    "LOAD",
    "STORE",
    "MOV",
    "MOVI",
    "XOR",
    "AND",
    "OR",
    "PUTCHAR",
    "GETCHAR",
    "JMP",
    "JZ",
    "HALT",
]
code = b""

with open(sys.argv[1], "r") as f:
    for line in f:
        line = line.strip().split(" ")

        opcode = instruction_map.index(line[0])
        operands = [0, 0]
        for i, arg in enumerate(line[1:]):
            if re.match("R[1-4]", arg):
                operands[i] = int(arg[1])-1
            elif re.match("\[[0-9]+\]", arg):
                operands[i] = int(arg[1:-1])
            else:
                operands[i] = int(arg)

        code += struct.pack("<III", opcode, operands[0], operands[1])

with open(sys.argv[1] + ".bin", "wb") as f:
    f.write(code)

