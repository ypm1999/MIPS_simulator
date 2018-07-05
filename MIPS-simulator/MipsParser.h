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
using std::pair;

// TODO: Format the text into entries
class MipsParser {
	friend class MipsSimulator;

	class Data {
		friend class MipsParser;
		friend class MipsSimulator;
		int n;
		string data;
	public:
		Data() :n(-1) {}
		~Data() = default;
	#ifdef DEBUG
		#include <cstdio>
		void out() {
			printf("data: %d %d\n", _type, address.i);
		}
	#endif
	};

	int codeLimit;
	string s;
	Memory *mem;
	vector< pair<string, vector<Data> > > *data;
	map<string, vector<int> > *table;
	map<string, Word> commandMap;
	map<string, Word> dataMap;
	map<string, CommandType> commandNameMap;
	map<string, DataType> dataNameMap;
	map<string, Byte> regMap;


	void __initialization();
	string getSingleString(unsigned int &pos) const;
	bool getData(unsigned int &pos, Data & res) const;
	bool getCommand(unsigned int &pos);
	void getDataBlock(unsigned int &pos, vector<Data> &res) const;
	Word getCommandBlock(unsigned int &pos);
	void getText();
	void writeData();
	void matchLabel();

public:
	MipsParser(const string &str, Memory *_mem);
	~MipsParser() = default;
	bool parser();
	int getLimit() { return codeLimit; }
	int getAddress(string name);
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
