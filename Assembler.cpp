//
//  Assembler.cpp
//  Assembler
//
//  Created by GregMac on 10/29/14.
//  Copyright (c) 2014 Mac. All rights reserved.
//

#include "Assembler.h"

void Assembler::initInstructions()
{
    instructions["TRP"] = TRP;
    instructions["JMP"] = JMP;
    instructions["JMR"] = JMR;
    instructions["BNZ"] = BNZ;
    instructions["BGT"] = BGT;
    instructions["BLT"] = BLT;
    instructions["BRZ"] = BRZ;
    instructions["MOV"] = MOV;
    instructions["LDA"] = LDA;
    instructions["STR"] = STR;
    instructions["LDR"] = LDR;
    instructions["STB"] = STB;
    instructions["LDB"] = LDB;
    instructions["ADD"] = ADD;
    instructions["ADI"] = ADI;
    instructions["SUB"] = SUB;
    instructions["MUL"] = MUL;
    instructions["DIV"] = DIV;
    instructions["AND"] = AND;
    instructions["OR"] = OR;
    instructions["CMP"] = CMP;
    instructions["STRI"] = STRI;
    instructions["LDRI"] = LDRI;
    instructions["STBI"] = STBI;
    instructions["LDBI"] = LDBI;
    instructions["RUN"] = RUN;
    instructions["END"]= END;
    instructions["BLK"] = BLK;
    instructions["LCK"] =LCK;
    instructions["ULK"] = ULK;
}

char* Assembler::AssembleCode(string fileName, int size)
{
    mem = new char[size];
    
    //open and test file
    ifstream inFile(fileName, ifstream::in);
    if(!inFile)
        throw runtime_error("Error in opening file: " + string(fileName));
    
    int PC = 0;
    int memctr = 0;
    int instructionCount =0;
    
    //initialize the instructions to values, built in the code statically
    initInstructions();
    
    //process first pass
    string line = "";
    int lineCtr = 0;
    string previousDirective = "";
    string previousDirectiveType = "";
    
    while (getline(inFile, line))
    {
        ++lineCtr;
        
        ++lineCtr;
        int linetype = TokenUtil::validateLineType(line);
        
        //if not directive ensure validity of code, increment memctrt
        if (linetype == invalid)
        {
            throw runtime_error("Invalid syntax ln" + to_string(lineCtr) + ": " + line);
        }
        else if (linetype == instruction)
        {
            ++instructionCount;
            InstructionLine il = TokenUtil::instructionLine(line);
            //check for label on instruction line
            if(il.label != "")
            {
                if (tokenAddresses.find(il.label) != tokenAddresses.end())
                {
                    throw runtime_error("Multiple definition ln" + to_string(lineCtr) + ": label '" + il.label + "'");
                }
                else
                {
                    tokenAddresses[il.label] = instructionCount*12;
                    gotoLabels.insert(il.label);
                }
            }
        }
        else if (linetype == directive)
        {
            DirectiveLine directive = TokenUtil::directiveLine(line);
            //empty labels should mean an array
            if (directive.label == "")
            {
                if(previousDirective == "")  //thorw an exception if no array started
                    throw runtime_error("Unlabeled data unbindable to a label: " + to_string(lineCtr));
                else //process as an array member bound to last declared directive
                {
                    if(previousDirectiveType == "")
                        previousDirectiveType = directive.type;
                    else if(previousDirectiveType != directive.type) //change in data in bound array, throw err
                        throw runtime_error("Change in previously bound array type from " +previousDirectiveType+" to " + directive.type +": line " + to_string(lineCtr));
                }
            }
            else if (tokenAddresses.find(directive.label) != tokenAddresses.end())
            {
                throw runtime_error("Multiple definition ln" + to_string(lineCtr) + ": label '" + directive.label + "'");
            }
            else
            {
                previousDirective = directive.label;
                previousDirectiveType = directive.type;
            }
            
            if (directive.type == ".INT")
            {
                memcpy(mem + memctr, directive.value.bytes, 4);
                tokenAddresses[directive.label] = memctr;
                memctr += 4;
            }
            else if (directive.type == ".BYT")
            {
                mem[memctr] = directive.value.byte;
                tokenAddresses[directive.label] = memctr;
                ++memctr;
            }
            else
            {
                throw runtime_error("Unsupported datatype ln-" + to_string(lineCtr) + ": " + directive.type);
            }
        }
    }
    //move all memory behind what will be instruction space and it in all the addresses of mapped tokens
    memcpy(mem + instructionCount*12, mem, memctr);
    for (map<string,int>::iterator itr = tokenAddresses.begin(); itr!=tokenAddresses.end(); ++itr)
    {
        if(gotoLabels.find(itr->first) == gotoLabels.end())
            itr->second += instructionCount*12;
    }
    //offset final memory Segment with padding and push it back for instruction space
    memctr += 3 + (instructionCount*12);
    
    //implement second pass
    inFile.clear();
    inFile.seekg(0, ios::beg);
    lineCtr = 0;
    
    while (getline(inFile, line))
    {
        ++lineCtr;
        InstructionLine instr = TokenUtil::instructionLine(line);
        if (instr.instruction != "")
        {
            if (instructions.find(instr.instruction) != instructions.end())
            {
                int val = instructions[instr.instruction];
                memcpy(mem + PC, reinterpret_cast<char*>(&val), 4);
                PC += 4;
                
                //add second arg
                if (instr.source.mode == reg)
                {
                    int offset = stoi(instr.source.value);
                    memcpy(mem + PC, &offset, 4);
                    PC += 4;
                }
                else if (instr.source.mode == immediate)
                {
                    int val = stoi(instr.source.value);
                    memcpy(mem + PC, reinterpret_cast<char*>(&val), 4);
                    PC += 4;
                }
                else if (instr.source.mode == indirect)
                {
                    if (tokenAddresses.find(instr.source.value) != tokenAddresses.end())
                    {
                        tokenAddresses[instr.source.value] = memctr;
                    }
                    int num = stoi(instr.source.value);
                    memcpy(mem + memctr, &num, 4);
                    memctr += 4;
                    PC += 4;
                }
                else if (instr.source.mode == label)
                {
                    //validate symbol
                    if (tokenAddresses.find(instr.source.value) != tokenAddresses.end())
                    {
                        int addr = tokenAddresses[instr.source.value];
                        memcpy(mem + PC, reinterpret_cast<char*>(&addr), 4);
                    }
                    else
                    {
                        throw runtime_error("Encountered unkown label on line" + to_string(lineCtr) + ": " + instr.source.value);
                    }
                    PC += 4;
                }
                else
                {
                    throw runtime_error("Invalid source argument on line " + to_string(lineCtr) + ": " + instr.source.value);
                }
                
                //add third arg, source
                if (instr.destination.mode != none)
                {
                    if (instr.destination.mode == reg)
                    {
                        int offset = stoi(instr.destination.value);
                        memcpy(mem + PC, &offset, 4);
                        PC += 4;
                    }
                    else if (instr.destination.mode == immediate)
                    {
                        int val = stoi(instr.destination.value);
                        memcpy(mem + PC, reinterpret_cast<char*>(&val), 4);
                        PC += 4;
                    }
                    else if (instr.destination.mode == indirect)
                    {
                        if (tokenAddresses.find(instr.destination.value) != tokenAddresses.end())
                        {
                            tokenAddresses[instr.destination.value] = memctr;
                        }
                        int num = stoi(instr.destination.value);
                        memcpy(mem + memctr, &num, 4);
                        memctr += 4;
                        PC += 4;
                    }
                    else if (instr.destination.mode == label)
                    {
                        //validate symbol
                        if (tokenAddresses.find(instr.destination.value) != tokenAddresses.end())
                        {
                            RawData data = RawData();
                            data.num = tokenAddresses[instr.destination.value];
                            memcpy(mem + PC, data.bytes, 4);
                        }
                        else
                        {
                            throw runtime_error("Encountered unkown label on line " + to_string(lineCtr) + ": " + instr.destination.value);
                        }
                        PC += 4;
                    }
                    else
                    {
                        throw runtime_error("Invalid source argument on line " + to_string(lineCtr) + ": " + instr.destination.value);
                    }
                }
                else{
                    PC += 4;
                }
            }
            else
            {
                string err = "Unknown instruction on line " + std::to_string(lineCtr) + ": " + line;
                throw runtime_error(err);
            }
            //codeDump(PC-12);
        }
    }
    
    //assign end of code and data segment as heap start
    heapStart = memctr;
    return mem;
}

void Assembler::MakePC_InstructionTable(string inF, string outFile)
{
    //open and test file
    map<int,string> instructions;
    map<int,string> data;
    
    ifstream inFile(inF, ifstream::in);
    if(!inFile)
        throw runtime_error("Error in opening file: " + string(inF));
    
    int memctr = 6;
    int instructionCount =0;
    
    //initialize the instructions to values, built in the code statically
    initInstructions();
    
    //process first pass
    string line = "";
    
    while (getline(inFile, line))
    {
        int linetype = TokenUtil::validateLineType(line);
        
        if (linetype == instruction)
        {
            instructions[instructionCount] = line;
            ++instructionCount;
        }
        else if (linetype == directive)
        {
            data[memctr] = line;
            if(regex_match(line,std::regex(".*\\.BYT.*")))
                memctr++;
            else if(regex_match(line,std::regex(".*\\.INT.*")))
                memctr += 4;
        }
    }
    
    ofstream ot(outFile, ofstream::out);
    ot<<"INSTRUCTIONS: "<<endl;
    for (map<int,string>::iterator itr = instructions.begin(); itr!=instructions.end(); ++itr)
    {
        ot << itr->first * 12 << ": " << itr->second << endl;
    }
    
    ot<<"\nDATA: "<<endl;
    for (map<int,string>::iterator itr = data.begin(); itr!=data.end(); ++itr)
    {
        ot << itr->first + ( instructionCount * 12) << ": " << itr->second << endl;
    }
    
    ot.close();
}
