//
//  VM.h
//  Assembler
//
//  Created by GregMac on 10/30/14.
//  Copyright (c) 2014 Mac. All rights reserved.
//

#ifndef __Assembler__VM__
#define __Assembler__VM__

#include "Assembler.h"

class VirtualMachine{
private:
    RawData registers[9];
    char* mem;
public:
    VirtualMachine(char* m){mem = m;}
    void Run();
};
#endif /* defined(__Assembler__VM__) */
