#ifndef __MipsParser
#define __MipsParser


#include <map>
#include <vector>
#include <string>
#include "Format.h"
#include "Memory.h"
#include "Exceptions.h"
using std::string;
using std::vector;
using std::map;

// TODO: Format the text into entries
class MipsParser {
	friend class MipsSimulator;
	string s;
	vector<Command> command;
	map<string, Word> commandMap;
	map<string, Word> dataMap;
	map<string, vector<size_t>> table;
	map<string, CommandType> commandNameMap;
	map<string, DataType> dataNameMap;
	map<string, Byte> regMap;

	void __initialization();
	string getSingleString(unsigned int &pos) const;
	Data getData(unsigned int &pos, Memory &mem) const;
	Command getCommand(unsigned int &pos);
	Word getDataBlock(unsigned int &pos, Memory &mem) const;
	Word getCommandBlock(unsigned int &pos, Memory &mem);
public:
	MipsParser(const string &str);
	~MipsParser() = default;

	bool parser(Memory &mem);
	string getname(Word address);
#ifdef DEBUG
#include <iostream>
	void out() {
		for (int i = 0; i < command.size(); i++) {
			std::string na = getname(i);
			if (na != "none")
				std::cout << na << ":  ";
			command[i].out();
		}
	}
#endif
};
#endif // !__MipsParser
