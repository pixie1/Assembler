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

#define BYTECODE_SIZE 400000

using namespace std;

//asm instruction values
enum instructionWords{
    TRP = 0, JMP = 1, JMR = 2, BNZ = 3, BGT = 4, BLT = 5, BRZ = 6, MOV = 7,
    LDA = 8, STR = 9, LDR = 10, STB = 11, LDB = 12, ADD = 13, ADI = 14, SUB = 15,
    MUL = 16, DIV = 17, AND = 18, OR = 19, CMP = 20, STRI = 21, LDRI =22, STBI = 23, LDBI = 24,
};

class Assembler{
private:
    //variables
    map<string, int> instructions = map<string, int>();
    map<string, int> tokenAddresses = map<string, int>();
    set<string> gotoLabels = set<string>();
    char* mem = new char[BYTECODE_SIZE];
    
    //helper functions
    void initInstructions();
public:
    char* AssembleCode(string fileName);
    ~Assembler(){delete mem;}
};

#endif /* defined(__Assembler__Assembler__) */
