//
//  VM.h
//  Assembler
//
//  Created by GregMac on 10/30/14.
//  Copyright (c) 2014 Mac. All rights reserved.
//

#ifndef __Assembler__VM__
#define __Assembler__VM__

#define THREAD_MAX 7
#define REGISTER_COUNT 13
#define REGISTER_SIZE 4

#include "Assembler.h"
#include <vector>
#include <mutex>

struct WordData
{
    RawData InstructionCode;
    RawData DestArg;
    RawData SourceArg;
};

class VirtualMachine{
private:
    static RawData registers[REGISTER_COUNT];
    static WordData fetch;
    static char* mem;
    static bool threadpool[THREAD_MAX];
    static int currentThreadID;
    static int totalStackSpace;
    static int memSize;
    static int heapStart;
    static int getAvailableThread();
    static void ContextSwitch();
    static void AssignThreadConext(int,int,bool);
    static ofstream log;
    static ofstream contextLog;
public:
    static bool exeLog;
    VirtualMachine(char* m,int heapStart,int size);
    void Run(int start);
    static void RunThread(int start, int threadId);
};
#endif /* defined(__Assembler__VM__) */
