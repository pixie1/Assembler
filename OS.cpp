#define _CRT_SECURE_NO_WARNINGS
#include "OS.h"
#include <windows.h>
#include <set>
#include <iostream>

# define listcmd "dir /b"

using namespace std;

vector<PCB*> OS::PCBList;

void OS::ls(){
	system(listcmd);
}
void OS::ps(){}
void OS::cd(string path){
	//this is not working
	string current = getCurrentDirectory();
	if (path == ".."){
		size_t pos = current.find_last_of("\\");
		current= current.substr(0, pos);
	}
	else{
		current += "\\";
		current += path;
	}
	const char* cmd = current.c_str();
	SetCurrentDirectory(cmd);
}
void OS::run(std::string num){
	int pNum = stoi(num);
	size_t pos;
	//find correct process in vector
	for (size_t i = 0; i < PCBList.size(); ++i){
		if (PCBList[i]->number == pNum){
			pos = i;
			break;
		}
	}
	//load register

	//issues: need to use base+ offset in vm
	//need to pass start position to vm
	//do we keep size in PCB?


}
void OS::load(std::string name){
	//set up PCB
	PCB* process=new PCB;
	process->name = name;
	process->number = PCBList.size() + 1;
	process->state = loading;
	//open and load file in char* 
	ifstream is(name);
	loadMemory(is);
	//
	process->state = ready;
	PCBList.push_back(process);
}

string OS::getCurrentDirectory(){
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	size_t pos = string(buffer).find_last_of("\\/");
	return string(buffer).substr(0, pos);
}

void OS::runShell(){
	string command = "";
	while (command != "quit"){
		//display current directory? and prompt
		cout << getCurrentDirectory();
		cout << ">";
		getline(cin, command);
		vector<string> instructionTokens = parseCommand(command);
		if (command.size() != 0 && checkCommand(instructionTokens)){
			if (instructionTokens[0] == "quit")
				command = "quit";
			if (instructionTokens[0] == "help")
				displayHelp();
			if (instructionTokens[0] == "ls")
				OS::ls();
			if (instructionTokens[0] == "cd")
				OS::cd(instructionTokens[1]);
			if (instructionTokens[0] == "ps")
				OS::ps();
			if (instructionTokens[0] == "run")
				OS::run(instructionTokens[1]);
			if (instructionTokens[0] == "load")
				OS::run(instructionTokens[1]);
		}
	}
}

vector<string> OS::parseCommand(string com){
	char * cstr = new char[com.length() + 1];
	strcpy(cstr, com.c_str());
	char * p = strtok(cstr, " ");
	vector<string> token;
	while (p != nullptr){
		string temp(p);
		token.push_back(temp);
		p = strtok(NULL, " ");
	}
	delete[] cstr;
	return token;
}

bool OS::checkCommand(vector<string>& tokens){
	set<string> commandSet = { "load", "run", "quit", "help", "ls", "cd", "ps" };
	if (commandSet.find(tokens[0]) == commandSet.end())
		return false;
	if (tokens[0] == "load" || tokens[0] == "run" || tokens[0]=="cd")
		return tokens.size() == 2U ? true : false;
	else
		return tokens.size() == 1U ? true : false;
}

void OS::displayHelp(){
	cout << "load hexName" << endl;
	cout << "run processNum" << endl;
	cout << "quit: exit shell" << endl;
	cout << "help: print this menu" << endl;
	cout << "ls: print out files in the current directory" << endl;
	cout << "cd: change directories" << endl;
	cout << "ps: show processes in memory" << endl;
}

void OS::loadMemory(ifstream& is){
	

}
