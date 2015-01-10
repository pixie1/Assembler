//
//  VM.cpp
//  Assembler
//
//  Created by GregMac on 10/30/14.
//  Copyright (c) 2014 Mac. All rights reserved.
//

#include "VM.h"
#include <thread>

RawData VirtualMachine::registers[13];
WordData VirtualMachine::fetch;
char* VirtualMachine::mem;
bool VirtualMachine::threadpool[THREAD_MAX];
int VirtualMachine::currentThreadID;
ofstream VirtualMachine::log;
ofstream VirtualMachine::contextLog;
bool VirtualMachine::exeLog;
int VirtualMachine::memSize;
int VirtualMachine::totalStackSpace;
int VirtualMachine::heapStart;

VirtualMachine::VirtualMachine(char * m, int heapStart, int size)
{
    //memory to be processed by vm
    mem = m;
    
    //assign dimensions for stack space
    this->heapStart = heapStart;
    
    //store stack space count before partitioned by threads
    totalStackSpace = size - heapStart;
    memSize = size;
    
    //set up the IR fetched instructions
    fetch = WordData();
}

int VirtualMachine::getAvailableThread()
{
    int threadId = -1;
    for(int i=0;i<THREAD_MAX && threadId < 0;++i)
    {
        if(!threadpool[i])
        {
            threadId = i;
            threadpool[i] = true;
        }
    }
    return threadId;
}

void VirtualMachine::ContextSwitch()
{
    int oldId = currentThreadID;
    bool switched = false;
    for(int i=currentThreadID+1;i<THREAD_MAX && ! switched;++i)
    {
        if(threadpool[i])
        {
            currentThreadID= i;
            switched = true;
        }
    }
    for(int i=0;i<currentThreadID && ! switched;++i)
    {
        if(threadpool[i])
        {
            currentThreadID= i;
            switched = true;
        }
    }
    
    //change register context if a switch was made
    if(switched)
    {
        /*contextLog<<endl;
        contextLog<<"Switching from "<<oldId<<" to "<< currentThreadID<<endl;*/
        //write old context
        int oldbase = memSize - (totalStackSpace/THREAD_MAX * oldId);
        int reg = 0;
        for(int i = 0;i<REGISTER_COUNT; i++)
        {
            int offset = oldbase - ((i+1) * REGISTER_SIZE);
            int num = registers[i].num;
            //contextLog<<oldId<<"~"<<offset<<": "<<num<<endl;
            memcpy(mem + offset,reinterpret_cast<char*>(&num),REGISTER_SIZE);
        }
        
        //read new context
        int base = memSize - (totalStackSpace/THREAD_MAX * currentThreadID);
        reg = 0;
        for(int i = 0;i<REGISTER_COUNT; i++)
        {
            int offset = base - ((i+1) * REGISTER_SIZE);
            registers[i].num = *reinterpret_cast<int*>(mem+offset);
           // contextLog<<currentThreadID<<"~"<<offset<<": "<< registers[i].num <<endl;
        }
        
      /*  contextLog<<"Assigning context to: "<<currentThreadID<<endl;
        for(int i=0;i<REGISTER_COUNT;i++)
        {
            contextLog<<"R"<<i<<": "<<registers[i].num<<endl;
        }*/
    }
}

void VirtualMachine::AssignThreadConext(int threadId, int pc, bool copyRegisters)
{
    int base = memSize - (totalStackSpace/THREAD_MAX * threadId);
    int regularRegisterLimit = base - REGISTER_SIZE*8;
    int reg = 0;
    
    int sl;// = registers[9].num;
    int sp;// = registers[10].num;
    int fp;// = registers[11].num;
    int sb;// = registers[12].num;
    
    //contextLog<<endl;
    //contextLog<<"Assigning context to: "<<threadId<<endl;
    
    /*for(int i=0;i<REGISTER_COUNT;i++)
    {
        contextLog<<"R"<<i<<": "<<registers[i].num<<endl;
    }*/
    
    
    //make initial registers in context = 0
    for(int i = base ;i>regularRegisterLimit; i -= REGISTER_SIZE)
    {
        int num = copyRegisters?registers[reg++].num:0;
       // contextLog<<i - REGISTER_SIZE<< ": " << num << endl;
        memcpy(mem + i -REGISTER_SIZE,reinterpret_cast<char*>(&num),REGISTER_SIZE);
    }
    
    int stackiterator = base - REGISTER_SIZE*9;
    //copy PC in
    memcpy(mem + stackiterator,reinterpret_cast<char*>(&pc),REGISTER_SIZE);
    //contextLog<<stackiterator<< ": " << *reinterpret_cast<int*>(mem+stackiterator) << endl;
    stackiterator -= REGISTER_SIZE;
    sl = base - totalStackSpace/THREAD_MAX;
    memcpy(mem + stackiterator,reinterpret_cast<char*>(&sl),REGISTER_SIZE);
    //contextLog<<stackiterator<< ": " << *reinterpret_cast<int*>(mem+stackiterator) << endl;
    stackiterator -= REGISTER_SIZE;
    sp = base - REGISTER_COUNT*REGISTER_SIZE;
    memcpy(mem + stackiterator,reinterpret_cast<char*>(&sp),REGISTER_SIZE);
    //contextLog<<stackiterator<< ": " << *reinterpret_cast<int*>(mem+stackiterator) << endl;
    stackiterator -= REGISTER_SIZE;
    fp = sp;
    memcpy(mem + stackiterator,reinterpret_cast<char*>(&fp),REGISTER_SIZE);
    //contextLog<<stackiterator<< ": " << *reinterpret_cast<int*>(mem+stackiterator) << endl;
    sb = fp;
    stackiterator -= REGISTER_SIZE;
    memcpy(mem + stackiterator,reinterpret_cast<char*>(&sb),REGISTER_SIZE);
    //contextLog<<stackiterator<< ": " << *reinterpret_cast<int*>(mem+stackiterator) << endl;
    
}

void VirtualMachine::Run(int start)
{
	if (exeLog)
		log = ofstream("VMlog.txt", ofstream::out);
    //contextLog = ofstream("contextlog.txt", ofstream::out);
    int mainThread = getAvailableThread();
    AssignThreadConext(mainThread, 0, false);
    RunThread(0, mainThread);
    //thread t = thread(VirtualMachine::RunThread,start,mainThread);
    //t.join();
}

void VirtualMachine::RunThread(int start,int tId)
{
    bool running = true;
    
    //make aliases for special registers
    int& PC = registers[8].num;
    int& SL = registers[9].num;
    int& SP = registers[10].num;
    int& FP = registers[11].num;
    int& SB = registers[12].num;
    
    //assign references to fetch arguments
    int& instructionCode = fetch.InstructionCode.num;
    int& destArg = fetch.DestArg.num;
    int& srcArg = fetch.SourceArg.num;
    
    int base = memSize - (totalStackSpace/THREAD_MAX * currentThreadID);
    for(int i = 0;i<REGISTER_COUNT; i++)
    {
        int offset = base - ((i+1) * REGISTER_SIZE);
        registers[i].num = *reinterpret_cast<int*>(mem+offset);
        contextLog<<currentThreadID<<"~"<<offset<<": "<< registers[i].num <<endl;
    }
    
    while(running) {
        //do round robin context switch each cycle
        ContextSwitch();
        
        fetch = *reinterpret_cast<WordData*>(mem + PC);
        if(exeLog)//output data for debugging and logging
        {
            log<<"PC-"<<currentThreadID<<"~"<<PC<<": "<<instructionCode<<"\t"<<destArg<<"\t"<<srcArg<<endl;
            log<<endl;
            log<<"R0: "<<registers[0].num<<endl;
            log<<"R1: "<<registers[1].num<<endl;
            log<<"R2: "<<registers[2].num<<endl;
            log<<"R3: "<<registers[3].num<<endl;
            log<<"R4: "<<registers[4].num<<endl;
            log<<"R5: "<<registers[5].num<<endl;
            log<<"R6: "<<registers[6].num<<endl;
            log<<"R7: "<<registers[7].num<<endl;
            log<<"SP: "<<SP<<endl;
            log<<"FP: "<<FP<<endl;
            log<<"SB: "<<SB<<endl;
            log<<"SL: "<<SL<<"\n"<<endl;
        }

        PC+= 12;
        
        RawData raw1 = RawData();
        
        switch(instructionCode) {
            case ADD:
                registers[destArg].num += registers[srcArg].num ;
                break;
            case ADI:
                registers[destArg].num += srcArg;
                break;
            case MUL:
                registers[destArg].num *= registers[srcArg].num;
                break;
            case DIV:
                registers[destArg].num = registers[srcArg].num/registers[destArg].num;
                break;
            case SUB:
                registers[destArg].num = registers[srcArg].num - registers[destArg].num;
                break;
            case MOV:
                memcpy(registers+destArg,registers+srcArg,WORDSIZE);
                break;
            case LDR:
                memcpy(registers+destArg,mem+ srcArg,WORDSIZE);
                break;
            case JMP:
                PC = destArg-12;
                break;
            case JMR:
                PC = registers[destArg].num;
                break;
            case CMP: //same as subtract
                registers[destArg].num = registers[srcArg].num - registers[destArg].num;
                break;
            case LDB :
                registers[destArg].num = 0;
                registers[destArg].byte = mem[srcArg];
                break;
            case STB:
                for(int i =0;i<7;++i)
                mem[srcArg] = registers[destArg].byte;
                break;
            case LDA:
                memcpy(registers+destArg, &srcArg,WORDSIZE);
                break;
            case AND:
                if(registers[srcArg].num==0)
                    registers[destArg].num = 0;
                break;
            case BRZ:
                raw1.num = *reinterpret_cast<int*>(registers+destArg);
                if(raw1.num == 0)
                {
                    PC = srcArg-12;
                }
                break;
            case BNZ:
                raw1.num = *reinterpret_cast<int*>(registers+destArg);
                if(raw1.num != 0)
                {
                    PC = srcArg-12;
                }
                break;
            case BGT:
                if(registers[destArg].num > 0)
                {
                    PC = srcArg-12;
                }
                break;
            case BLT:
                raw1.num = *reinterpret_cast<int*>(registers+destArg);
                if(raw1.num < 0)
                {
                    PC = srcArg-12;
                }
                break;
            case STR:
                memcpy(mem+srcArg,registers+destArg,WORDSIZE);
                break;
            case LDRI:
                srcArg = *reinterpret_cast<int*>(&registers[srcArg]); //indirect part
                memcpy(registers+destArg,mem+srcArg,WORDSIZE);
                break;
            case LDBI:
                srcArg = *reinterpret_cast<int*>(&registers[srcArg]); //indirect part
                raw1.num = 0;
                raw1.byte = mem[srcArg];
                memcpy(registers+destArg, raw1.bytes,WORDSIZE);
                break;
            case STRI:
                srcArg = *reinterpret_cast<int*>(&registers[srcArg]); //indirect part
                memcpy(mem+srcArg,registers+destArg,WORDSIZE);
                break;
            case STBI:
                srcArg = *reinterpret_cast<int*>(&registers[srcArg]); //indirect part
                mem[srcArg] = registers[destArg].byte;
                break;
            case RUN:
                destArg = getAvailableThread();
                if(destArg > -1)
                    AssignThreadConext(destArg, srcArg-12, true);
                else
                    throw runtime_error("Error: Exceeded thread maximum: " + THREAD_MAX);
                break;
            case END:
                if(currentThreadID >0)
                    threadpool[currentThreadID] = false;
                break;
            case BLK:
                if(currentThreadID==0)
                {
                    bool onlyThread = true;
                    for(int i=1; i <THREAD_MAX && onlyThread;++i)
                    {
                        onlyThread = !(threadpool[i]);
                    }
                    if(!onlyThread)
                        PC -= 12;
                }
                break;
            case LCK:
                srcArg = *reinterpret_cast<int*>(&mem[destArg]);
                if(srcArg==-1)
                {
                    memcpy(mem+destArg, reinterpret_cast<char*>(&currentThreadID), 4);
                }
                else{
                    PC -= 12;  //repeat until lock is resolved
                }
                break;
            case ULK:
                srcArg = *reinterpret_cast<int*>(&mem[destArg]);
                if(srcArg==currentThreadID)
                {
                    srcArg =-1;
                    memcpy(mem+destArg, reinterpret_cast<char*>(&srcArg), 4);
                }
                break;
            case TRP:
                switch (destArg)
                {
                case 0:
                    running = false;
                    break;
                case 1:
                    cout<< *reinterpret_cast<int*>(registers)<<flush;
                    break;
                case 2:
                    cin >> registers[0].num;
					break;
                case 3:
                    cout << registers[0].byte << flush;
                    break;
                case 4:
                        registers[0].num = 0;
                    registers[0].byte = getchar();
                        break;
                case 99://debug dump
                        cout<<"PC: "<<PC<<endl;
                        cout<<endl;
                        cout<<"R0: "<<registers[0].num<<endl;
                        cout<<"R1: "<<registers[1].num<<endl;
                        cout<<"R2: "<<registers[2].num<<endl;
                        cout<<"R3: "<<registers[3].num<<endl;
                        cout<<"R4: "<<registers[4].num<<endl;
                        cout<<"R5: "<<registers[5].num<<endl;
                        cout<<"R6: "<<registers[6].num<<endl;
                        cout<<"R7: "<<registers[7].num<<"\n"<<endl;
                default:
                    break;
                }
                break;
            default:
                throw runtime_error("Unexecutable instruction occured: " + to_string(fetch.InstructionCode.num));
                break;
        }
    }
}
