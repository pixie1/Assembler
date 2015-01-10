#include "TokenUtil.h"
#include <iostream>

std::regex TokenUtil::directive_rgx("([\\w_]*)[\\t ]+(\\.[A-Z]+)[\\t ]+(-?[\\d]+|('\\\\{0,1}.'))[\\t ]*");
std::regex TokenUtil::directiveBinary_rgx("([\\w]*)[\\t ]+(\\.BYT)[\\t ]+0x(\\d\\d)[\\t ]*");
std::regex TokenUtil::instr_rgx("([\\t ]*(\\w+):[\\t ]*)?([A-Z]{2,4})[\\t ]+([R|$]?[\\w]+)([\\t ]*[\\t ,][\\t ]*([R$(]?-?[\\w]+\\)?))?[\\t ]*");
std::regex TokenUtil::register_Rgx("R([0-9]+)");
std::regex TokenUtil::immediate_Rgx("\\$(-?\\d+)");
std::regex TokenUtil::indirect_Rgx("\\d+");
std::regex TokenUtil::register_indirect("\\(R([0-9]+)\\)");
std::regex TokenUtil::label_Rgx("(\\w+)");
std::regex TokenUtil::trimmer("\\s*(.*\\S)\\s*");

bool TokenUtil::mapsInitialized = false;
std::map<std::string,std::string> TokenUtil::specialRegisterTable;

InstructionLine TokenUtil::instructionLine(std::string line)
{
    if(!mapsInitialized)
        InitializeTranslationTables();
    
    InstructionLine i = InstructionLine();
    line = regex_replace(line,std::regex("#.*"),"");  //ignore comments
    line = Trim(line);
    std::smatch capture;
    std::string match;
    
    if(regex_match(line,capture,instr_rgx))
    {
        i.label = capture[2].str();
        i.instruction = capture[3].str();
        std::smatch arg;
        
        //add second arg
        match = capture[4].str();
        match = Trim(match);
        //convert special registers
        if (specialRegisterTable.find(match) != specialRegisterTable.end())
        {
            match = specialRegisterTable[match];
        }

        if(regex_match(match,arg,register_Rgx))
        {
            i.source.mode = reg;
            i.source.value = arg[1].str();
        }
        else if(regex_match(match,arg,immediate_Rgx))
        {
            i.source.mode = immediate;
            i.source.value = arg[1].str();
        }
        else if(regex_match(match,arg,label_Rgx))
        {
            i.source.mode = label;
            i.source.value = arg[1].str();
        }
        else if(regex_match(match,arg,indirect_Rgx))
        {
            i.source.mode = indirect;
            i.source.value = arg[1].str();
        }
        
        //add third arg, source
        match = capture[6].str();
        match = Trim(match);
        if (specialRegisterTable.find(match) != specialRegisterTable.end())
        {
            match = specialRegisterTable[match];
        }
        
        if(match != "")
        {
            if(regex_match(match,arg,register_Rgx))
            {
                i.destination.mode = reg;
                i.destination.value = arg[1].str();
                //check for register indirecting in memory instrucions, if so change instr
                if(i.instruction =="LDR" || i.instruction == "STR" ||
                   i.instruction =="LDB" || i.instruction == "STB")
                {
                    i.instruction += "I";
                }
            }
            else if(regex_match(match,arg,immediate_Rgx))
            {
                i.destination.mode = immediate;
                i.destination.value = arg[1].str();
            }
            else if(regex_match(match,arg,label_Rgx))
            {
                i.destination.mode = label;
                i.destination.value = arg[1].str();
            }
            else if(regex_match(match,arg,indirect_Rgx))
            {
                i.destination.mode = indirect;
                i.destination.value = arg[1].str();
            }
            else if(regex_match(match,arg,register_indirect))
            {
                i.destination.mode = reg;
                i.destination.value = arg[1].str();
                i.instruction += "I";
            }
        }

        else
        {
            i.destination.mode = none;
        }
    }
    else if(line == "BLK" || line == "END")
    {
        i.instruction = line;
        i.destination.mode = immediate;
        i.destination.value = "0";
        i.source.mode = immediate;
        i.source.value = "0";
    }
    return i;
}

DirectiveLine TokenUtil::directiveLine(std::string line)
{
    if(!mapsInitialized)
        InitializeTranslationTables();
    
    std::smatch capture;
    DirectiveLine d = DirectiveLine();
    line = regex_replace(line,std::regex("#.*"),"");  //eliminates comments
    
    if(std::regex_match(line,capture,directive_rgx))
    {
        d.label = capture[1].str();
        d.type = capture[2].str();
        if(d.type == ".INT")
            d.value.num = stoi(capture[3].str());
        else
        {
            std::string val = capture[3].str();
            std::string sub = val.substr(1).substr(0,val.size()-2);
            if(sub[0] == '\\')
            {
                switch(sub[1])
                {
                    case 'n' : sub = "\n"; break;
                    case 'r' : sub = "\r"; break;
                    case 't' : sub = "\t"; break;
                    case 'f' : sub = "\f"; break;
                    case 'a' : sub = "\a"; break;
                    case 'b' : sub = "\b"; break;
                    case '?' : sub = "\?"; break;
                    case '"' : sub = "\""; break;
                    case '0' : sub = "\0"; break;
                }
            }
            d.value.byte = sub[0];
        }
    }
    else if(regex_match(line,capture,directiveBinary_rgx))
    {
        d.label = capture[1].str();
        d.type = capture[2].str();
        std::string val = capture[3].str();
        d.value.byte = (char)(std::stoi(val));
    }
    return d;
}

lineType TokenUtil::validateLineType(std::string line)
{
    line = regex_replace(line, std::regex("#.*"), "");
    
    if (directiveLine(line).type != "")
        return directive;
    else if (instructionLine(line).instruction != "")
        return instruction;
    else if (instructionLine(line).instruction == "")
        return empty;
    else
        return invalid;
}

std::string TokenUtil::Trim(std::string line)
{
    if(line == ""  || regex_match(line,std::regex("\\s+")))
        return line;
    
    std::smatch trim;
    std::regex_match(line, trim, trimmer);
    return trim[1].str();
}

void TokenUtil::InitializeTranslationTables()
{
    specialRegisterTable["PC"] = "R8";
    specialRegisterTable["SL"] = "R9";
    specialRegisterTable["SP"] = "R10";
    specialRegisterTable["FP"] = "R11";
    specialRegisterTable["SB"] = "R12";
    specialRegisterTable["(PC)"] = "(R8)";
    specialRegisterTable["(SL)"] = "(R9)";
    specialRegisterTable["(SP)"] = "(R10)";
    specialRegisterTable["(FP)"] = "(R11)";
    specialRegisterTable["(SB)"] = "(R12)";
    
    mapsInitialized = true;
}
