#include "VM.h"
#include "Assembler.h"

int main(int argc, const char * argv[]) {
    
    if(argc <2)
    {
        cout<<"Please give .asm file as argument."<<endl;
        return 1;
    }
    
    //run assembler and get compiled code
    Assembler assembler = Assembler();
    char* assembly = assembler.AssembleCode(argv[1]);
    
    //execute virtual machine using bytecode
    VirtualMachine vm = VirtualMachine(assembly);
    vm.Run();
    
    return 0;
}




