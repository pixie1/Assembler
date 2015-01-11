#pragma once
#include <string>
#include <vector>
#include "VM.h"
#include <fstream>

enum processStates { loading, ready, waiting, running, suspended, terminating };

struct PCB{
	std::string name;
	int number;
	int reg[13];
	processStates state;
};


class OS
{
private:
	static std::string getCurrentDirectory();
	static std::vector<std::string> parseCommand(std::string);
	static bool checkCommand(std::vector<std::string>&);
	static void displayHelp();
	
	static void loadMemory(std::ifstream&);
	static char* memory;
public:
	static std::vector<PCB*> PCBList;
	static void ls();
	static void ps();
	static void cd(std::string);
	static void run(std::string);
	static void load(std::string);
	static void runShell();
};

