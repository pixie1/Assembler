//
//  VM.cpp
//  Assembler
//
//  Created by GregMac on 10/30/14.
//  Copyright (c) 2014 Mac. All rights reserved.
//

#include "VM.h"

void VirtualMachine::Run()
{
    bool running = true;
    registers[8].num = 0;  //PC register starts at 0 for expected byteCode
    
    while(running) {
        const int PC = registers[8].num;
        int codeNum = *reinterpret_cast<char*>(mem+PC);
        //codeDump(PC);
        int temp1 = 0;
        int temp2 = 0;
        
        RawData raw1 = RawData();
        
        switch(codeNum) {
            case ADD:
                temp1 = *reinterpret_cast<int*>(&mem[PC+4]);
                raw1.num = registers[temp1].num;
                temp2 = *reinterpret_cast<int*>(&mem[PC+8]);
                raw1.num = registers[temp2].num + raw1.num;
                memcpy(registers+temp1, raw1.bytes, 4);
                break;
            case ADI:
                temp1 = *reinterpret_cast<int*>(&mem[PC+4]);
                raw1.num = *reinterpret_cast<int*>(registers+temp1);
                temp2 = *reinterpret_cast<int*>(&mem[PC+8]);
                raw1.num +=  temp2;
                memcpy(registers+temp1, raw1.bytes, 4);
                break;
            case MUL:
                temp1 = *reinterpret_cast<int*>(&mem[PC+4]);
                raw1.num = *reinterpret_cast<int*>(registers+temp1);
                temp2 = *reinterpret_cast<int*>(&mem[PC+8]);
                raw1.num = *(reinterpret_cast<int*>(registers+temp2)) * raw1.num;
                memcpy(registers+temp1, raw1.bytes, 4);
                break;
            case DIV:
                temp1 = *reinterpret_cast<int*>(&mem[PC+4]);
                raw1.num = *reinterpret_cast<int*>(registers+temp1);
                temp2 = *reinterpret_cast<int*>(&mem[PC+8]);
                raw1.num = *(reinterpret_cast<int*>(registers+temp2)) / raw1.num;
                memcpy(registers+temp1, raw1.bytes, 4);
                break;
            case SUB:
                temp1 = *reinterpret_cast<int*>(&mem[PC+4]);
                raw1.num = *reinterpret_cast<int*>(registers+temp1);
                temp2 = *reinterpret_cast<int*>(&mem[PC+8]);
                raw1.num = *(reinterpret_cast<int*>(registers+temp2)) - raw1.num;
                memcpy(registers+temp1, raw1.bytes, 4);
                break;
            case MOV:
                temp1 = *reinterpret_cast<int*>(&mem[PC+4]);
                temp2 = *reinterpret_cast<int*>(&mem[PC+8]);
                memcpy(registers+temp1,registers+temp2,4);
                break;
            case LDR:
                temp1 = *reinterpret_cast<int*>(&mem[PC+4]);
                temp2 = *reinterpret_cast<int*>(&mem[PC+8]);
                memcpy(registers+temp1,mem+temp2,4);
                break;
            case JMP:
                registers[8].num = *reinterpret_cast<int*>(&mem[PC+4])-24;
                break;
            case CMP: //same as subtract
                temp1 = *reinterpret_cast<int*>(&mem[PC+4]);
                raw1.num = *reinterpret_cast<int*>(registers+temp1);
                temp2 = *reinterpret_cast<int*>(&mem[PC+8]);
                raw1.num = *(reinterpret_cast<int*>(registers+temp2)) - raw1.num;
                memcpy(registers+temp1, raw1.bytes, 4);
                break;
            case LDB :
                temp1 = *reinterpret_cast<int*>(&mem[PC+4]);
                temp2 = *reinterpret_cast<int*>(&mem[PC+8]);
                raw1.num = 0;
                raw1.byte = mem[temp2];
                memcpy(registers+temp1, raw1.bytes,4);
                break;
            case STB:
                raw1.num = *reinterpret_cast<int*>(&mem[PC+4]);
                temp2 = *reinterpret_cast<int*>(&mem[PC+8]);
                mem[temp2] = raw1.byte;
                break;
            case LDA:
                temp1 = *reinterpret_cast<int*>(&mem[PC+4]);
                memcpy(registers+temp1, &mem[PC+8],4);
                break;
            case AND:
                temp1 = *reinterpret_cast<int*>(&mem[PC+4]);
                raw1.num = *reinterpret_cast<int*>(registers+temp1);
                temp2 = *reinterpret_cast<int*>(&mem[PC+8]);
                raw1.num = *(reinterpret_cast<int*>(registers+temp2)) & raw1.num;
                memcpy(registers+temp1, raw1.bytes, 4);
                break;
            case BNZ:
                temp1 = *reinterpret_cast<int*>(&mem[PC+4]);
                raw1.num = *reinterpret_cast<int*>(registers+temp1);
                temp2 = *reinterpret_cast<int*>(&mem[PC+8]);
                memcpy(registers+temp1, raw1.bytes, 4);
                if(raw1.num == 0)
                {
                    //change pc to value - one instruction
                    registers[8].num = temp2-24;
                }
                break;
            case STR:
                temp1 = *reinterpret_cast<int*>(&mem[PC+4]);
                temp2 = *reinterpret_cast<int*>(&mem[PC+8]);
                memcpy(mem+temp2,registers+temp1,4);
                break;
            case LDRI:
                temp1 = *reinterpret_cast<int*>(&mem[PC+4]);
                temp2 = *reinterpret_cast<int*>(&mem[PC+8]);
                temp2 = *reinterpret_cast<int*>(&registers[temp2]); //indirect part
                memcpy(registers+temp1,mem+temp2,4);
                break;
            case LDBI:
                temp1 = *reinterpret_cast<int*>(&mem[PC+4]);
                temp2 = *reinterpret_cast<int*>(&mem[PC+8]);
                temp2 = *reinterpret_cast<int*>(&registers[temp2]); //indirect part
                raw1.num = 0;
                raw1.byte = mem[temp2];
                memcpy(registers+temp1, raw1.bytes,4);
                break;
            case STRI:
                temp1 = *reinterpret_cast<int*>(&mem[PC+4]);
                temp2 = *reinterpret_cast<int*>(&mem[PC+8]);
                temp2 = *reinterpret_cast<int*>(&registers[temp2]); //indirect part
                memcpy(mem+temp2,registers+temp1,4);
                break;
            case STBI:
                temp1 = *reinterpret_cast<int*>(&mem[PC+4]);
                temp2 = *reinterpret_cast<int*>(&mem[PC+8]);
                temp2 = *reinterpret_cast<int*>(&registers[temp2]); //indirect part
                mem[temp2] = registers[temp1].byte;
                break;
            case TRP:
                temp1 = *reinterpret_cast<int*>(&mem[PC+4]);
                switch (temp1)
            {
                case 0:
                    running = false;
                    break;
                case 1:
                    cout<< *reinterpret_cast<int*>(registers)<<flush;
                    break;
                case 3:
                    cout << registers[0].byte << flush;
                    break;
                default:
                    break;
            }
                break;
            default:
                throw runtime_error("Unexecutable instruction occured - " + to_string(codeNum));
                break;
        }
        registers[8].num +=12;
    }

}