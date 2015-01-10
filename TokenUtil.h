#ifndef __Assembler__TokenUtil__
#define __Assembler__TokenUtil__

#include <regex>
#include <string>
#include <stdio.h>
#include <map>

union RawData
{
    int num;
    char bytes[4];
    char byte;
};

enum addressingModes{none,direct,reg,immediate,indirect,label};
enum lineType{invalid,empty,directive,instruction};
enum specialRegiser{SL = 9, SP = 10, FP = 11, SB = 12};

//structs used to classify data from parsing
struct AssemblyArg
{
    addressingModes mode;
    std::string value;
};

struct DirectiveLine
{
    std::string label;
    std::string type;
    RawData value;
};

struct InstructionLine
{
    std::string label;
    std::string instruction;
    AssemblyArg source;
    AssemblyArg destination ;
};

//class handles all parsing activity used in assembly
class TokenUtil
{
private:
    static bool mapsInitialized;
    static std::map<std::string,std::string> specialRegisterTable;
    static std::regex directive_rgx;
    static std::regex directiveBinary_rgx;
    static std::regex instr_rgx;
    static std::regex register_Rgx;
    static std::regex immediate_Rgx;
    static std::regex indirect_Rgx;
    static std::regex register_indirect;
    static std::regex label_Rgx;
    static std::regex trimmer;
    static std::regex specialRegister_Rgx;
public:
    TokenUtil();
    static void InitializeTranslationTables();
    static InstructionLine instructionLine(std::string);
    static DirectiveLine directiveLine(std::string);
    static lineType validateLineType(std::string);
    static std::string Trim(std::string line);
};
#endif /* defined(__Assembler__TokenUtil__) */
