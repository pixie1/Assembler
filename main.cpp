#include "VM.h"
#include "Assembler.h"

int main(int argc, const char * argv[]) {
    
	bool logging = false;
	string fName = "";

	if (argc == 3)
	{
		fName = argv[2];
		if (string(argv[1]) == "-l")   //flag to add logging at command line
			logging = true;
		else
		{
			cout << "Unkown flag '"<< argv[1] << "'" << endl;
			return 1;
		}
	}
	else if (argc == 2)
	{
		fName = argv[1];
	}
	else if (argc<2)
	{
		cout << "Please give .asm file as argument." << endl;
		return 1;
	}
	else
	{
		cout << "Too many arguments.  Please only provide optional flag and .asm file." << endl;
		return 1;
	}
    
    //run assembler and get compiled code
    Assembler assembler = Assembler();
    char* assembly = assembler.AssembleCode(fName,BYTECODE_SIZE);
    
    //create table for PC-> instructions or data for debugging
	if (logging)
		assembler.MakePC_InstructionTable(fName, "byteCodeTble.txt");
    
    //execute virtual machine using bytecode
    VirtualMachine vm(assembly, assembler.heapStart, BYTECODE_SIZE);
    vm.exeLog = logging;
    vm.Run(0);
    
    return 0;
}




