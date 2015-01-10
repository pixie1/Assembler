//
//  Assembler.h
//  Assembler
//
//  Created by GregMac on 10/29/14.
//  Copyright (c) 2014 Mac. All rights reserved.
//

#ifndef __Assembler__Assembler__
#define __Assembler__Assembler__

#include <iostream>
#include <map>
#include <fstream>
#include <set>
#include <algorithm>

#include "TokenUtil.h"

#define WORDSIZE 4
#define BYTECODE_SIZE 400000

using namespace std;

const int INSTRUCTIONSIZE = WORDSIZE*3;

//asm instruction values
enum instructionWords{
    TRP = 0, JMP = 1, JMR = 2, BNZ = 3, BGT = 4, BLT = 5, BRZ = 6, MOV = 7,
    LDA = 8, STR = 9, LDR = 10, STB = 11, LDB = 12, ADD = 13, ADI = 14, SUB = 15,
    MUL = 16, DIV = 17, AND = 18, OR = 19, CMP = 20, STRI = 21, LDRI =22, STBI = 23, LDBI = 24,
    RUN = 25, END = 26, BLK=27, LCK = 28, ULK = 29
};

class Assembler{
private:
    //variables
    map<string, int> instructions = map<string, int>();
    map<string, int> tokenAddresses = map<string, int>();
    set<string> gotoLabels = set<string>();
    char* mem;
    
    //helper functions
    void initInstructions();
public:
    int heapStart;
    char* AssembleCode(string fileName, int size);
    void MakePC_InstructionTable(string inFile, string outFile);
    ~Assembler(){delete mem;}
};

#endif /* defined(__Assembler__Assembler__) */
