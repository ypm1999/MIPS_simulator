#ifndef __MipsParser
#define __MipsParser

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Format.h"
#include "Memory.h"
#include "Exceptions.h"
using namespace std;

// TODO: Format the text into entries
class MipsParser {
	vector<CommandBlock> commandBlock;
	vector<DataBlock> dataBlock;
	map<string, CommandType> commandMap;
	map<string, DataType> dataMap;
	
	MipsParser() {
		// TODO initialize dataMap and commandMap;
	}
	~MipsParser() = default;

	string getSingleString(const string &s, unsigned int &pos) {
		//
	}

	Data getData() {
		//
	}

	Command getCommand() {
		//
	}

	DataBlock getDataBlock(const string &s, unsigned int &pos, Memory &mem, const string name) const {
	}

	CommandBlock getCommandBlock(const string &s, unsigned int &pos, Memory &mem, const string name) const {
	}

	bool parser(const string &s, Memory &mem) {
		unsigned int pos = 0;
		bool text = 0;
		while (pos < s.length()) {
			try {
				int tmp = pos;
				string  name = getSingleString(s, pos);
				if (name == ".text") {
					text = true;
					continue;
				}
				if (name == ".data") {
					text = false;
					continue;
				}
				if (tmp == 0) {
					cerr << "ERROR! Code don't have .text or .data at beginning!" << endl;
					return false;
				}
				if (commandMap.find(name) != commandMap.end() || dataMap.find(name) != dataMap.end())
					name = "", pos = tmp;

				if (text) {
					commandBlock.push_back(getCommandBlock(s, pos, mem, name));
				}
				else {
					dataBlock.push_back(getDataBlock(s, pos, mem, name));
				}
			}
			catch (...) {
				cerr << "Unknow Error with parser!" << endl;
				return false;
			}
		}
		return true;
	}
};
#endif // !__MipsParser



