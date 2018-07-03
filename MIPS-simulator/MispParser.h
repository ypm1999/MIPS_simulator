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
	map<string, unsigned int> commandMap;
	map<string, unsigned int> dataMap;
	
	MipsParser() {
		
		
	}
	~MipsParser() = default;

	string getSingleString(const string &s, unsigned int &pos) const{
		while (s[pos] == ' ' || s[pos] == ',' || s[pos] == '\t' || s[pos] == '\n' || s[pos] == '_r')
			pos++;
		string res = "";
		bool flag = true;
		while (pos < s.length() && flag) {
			switch (s[pos]) {
				case ' ': case ',': case '\t': case '\n': case '\r':
					flag = false;
					break;
				default:
					res += s[pos];
			}
		}
		return std::move(res);
	}

	//This function will put the data in mem.
	Data getData(const string &s, unsigned int &pos, Memory &mem) const{
		//
	}

	Command getCommand(const string &s, unsigned int &pos) const {
		Command res;
		unsigned int lastpos = pos;
		if (pos >= s.length())
			return std::move(res);
		string tmp = getSingleString(s, pos);
		auto it = commandNameMap.find(tmp);
		if (it == commandNameMap.end()) {
			pos = lastpos;
			return std::move(res);
		}
		res._type = (*it).second;
		switch (res._type) {
			//rs rd src
			case CommandType::_add:
			case CommandType::_addu:
			case CommandType::_sub:
			case CommandType::_subu:
			case CommandType::_xor:
			case CommandType::_xoru:
			case CommandType::_rem:
			case CommandType::_seq:
			case CommandType::_sge:
			case CommandType::_sgt: 
			case CommandType::_sle:
			case CommandType::_slt:
			case CommandType::_sne:
			case CommandType::_addiu:
			case CommandType::_subiu: {
				string rs = getSingleString(s, pos);
				string rd = getSingleString(s, pos);
				string rt = getSingleString(s, pos);
				res.rs = regMap[rs];
				res.rd = regMap[rd];
				if (rt[0] == '$')
					res.rt = regMap[rt];
				else
					res.cons = stoi(rt);
				break;
			}
			
			//rs rd
			case CommandType::_neg:
			case CommandType::_negu:
			case CommandType::_move: {
				string rs = getSingleString(s, pos);
				string rd = getSingleString(s, pos);
				res.rs = regMap[rs];
				res.rd = regMap[rd];
				break;
			}
			//rs rd / rs rd rt
			case CommandType::_mul:
			case CommandType::_mulu:
			case CommandType::_div:
			case CommandType::_divu: {
				string rs = getSingleString(s, pos);
				string rd = getSingleString(s, pos);
				int lastpos = pos;
				string rt = getSingleString(s, pos);
				res.rs = regMap[rs];
				if (rt[0] == '$' || (rt[0] >= '0' && rt[0] <= '9')) {
					res.rd = regMap[rd];
					if (rt[0] == '$')
						res.rt = regMap[rt];
					else
						res.cons = stoi(rt);
				}
				else {
					pos = lastpos;
					if (rd[0] == '$')
						res.rd = regMap[rd];
					else
						res.cons = stoi(rd);
				}
				break;
			}
			//rs con
			case CommandType::_li: {
				string rs = getSingleString(s, pos);
				string con = getSingleString(s, pos);
				res.rs = regMap[rs];
				res.cons = stoi(con);
				break;
			}
								
			//rs
			case CommandType::_mfhi:
			case CommandType::_mflo: {
				string rs = getSingleString(s, pos);
				res.rs = regMap[rs];
				break;
			}

			//label
			case CommandType::_b:
			case CommandType::_j: 
			case CommandType::_jal: {
				res.label = getSingleString(s, pos);
				break;
			}

			//rd label
			case CommandType::_beqz:
			case CommandType::_bnez:
			case CommandType::_bgez:
			case CommandType::_blez:
			case CommandType::_bgtz:
			case CommandType::_bltz: {
				string rd= getSingleString(s, pos);
				res.label = getSingleString(s, pos);
				res.rd = regMap[rd];
				break;
			}

			//rd rt label
			case CommandType::_beq:
			case CommandType::_bne:
			case CommandType::_bge:
			case CommandType::_ble:
			case CommandType::_bgt:
			case CommandType::_blt: {
				string rd = getSingleString(s, pos);
				string rt = getSingleString(s, pos);
				res.label = getSingleString(s, pos);
				res.rt = regMap[rt];
				res.rd = regMap[rd];
				break;
			}
				
			//rs address
			case CommandType::_la:
			case CommandType::_lb: 
			case CommandType::_lh: 
			case CommandType::_lw: 
			case CommandType::_sb: 
			case CommandType::_sh: 
			case CommandType::_sw: {
				string rs = getSingleString(s, pos);
				string ad = getSingleString(s, pos);
				res.rs = regMap[rs];
				//TODO calc address;
				break;
			}

			case CommandType::_nop: 
			case CommandType::_syscall: {
				break;
			}

			default :
				throw command_not_found(tmp);
		}
		return std::move(res);
	}

	DataBlock getDataBlock(const string &s, unsigned int &pos, Memory &mem, const string name) const {
		DataBlock res;
		res.name = name;
		while (true) {
			unsigned int lastpos = pos;
			Data tmp = getData(s, pos, mem);
			if (tmp._type == DataType::none) {
				pos = lastpos;
				break;
			}
			res.data.push_back(tmp);
		}
		if (res.data.size() == 0)
			throw empty_label(name);
		return std::move(res);
	}

	CommandBlock getCommandBlock(const string &s, unsigned int &pos, Memory &mem, const string name) const {
		CommandBlock res;
		res.name = name;
		while (true) {
			unsigned int lastpos = pos;
			Command tmp = getCommand(s, pos);
			if (tmp._type == CommandType::none) {
				pos = lastpos;
				break;
			}
			res.data.push_back(tmp);
		}
		if (res.data.size() == 0)
			throw empty_label(name);
		return std::move(res);
	}

	bool parser(const string &s, Memory &mem) {
		unsigned int pos = 0;
		bool text = 0;
		while (pos < s.length()) {
			try {
				int tmp = pos;
				string  name = getSingleString(s, pos);
				if (name == "")
					break;
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
				if (name.back() != ':')
					name = "", pos = tmp;
				else
					name.pop_back();
				if (text) {
					commandBlock.push_back(getCommandBlock(s, pos, mem, name));
					if (name != "")
						commandMap[name] = commandBlock.size() - 1;
				}
				else {
					dataBlock.push_back(getDataBlock(s, pos, mem, name));
					if (name != "")
						dataMap[name] = dataBlock.size() - 1;
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



/*
switch (res._type) {
//rs rd rt
case CommandType::_add:
case CommandType::_addu:
case CommandType::_sub:
case CommandType::_subu:
case CommandType::_xor:
case CommandType::_xoru:
case CommandType::_rem:
case CommandType::_seq:
case CommandType::_sge:
case CommandType::_sgt:
case CommandType::_sle:
case CommandType::_slt:
case CommandType::_sne: {
break;
}
//rs rd con
case CommandType::_addiu:
case CommandType::_subiu: {
break;
}
//rs rd
case CommandType::_neg:
case CommandType::_negu:
case CommandType::_move: {
break;
}
//rs rd / rs rd rt
case CommandType::_mul:
case CommandType::_mulu:
case CommandType::_div:
case CommandType::_divu: {
break;
}
//rs con
case CommandType::_li: {
break;
}

//rs
case CommandType::_mfhi:
case CommandType::_mflo: {
break;
}

//label
case CommandType::_b:
case CommandType::_j:
case CommandType::_jal: {
break;
}

//rd label
case CommandType::_beqz:
case CommandType::_bnez:
case CommandType::_bgez:
case CommandType::_blez:
case CommandType::_bgtz:
case CommandType::_bltz: {
break;
}

//rd rt label
case CommandType::_beq:
case CommandType::_bne:
case CommandType::_bge:
case CommandType::_ble:
case CommandType::_bgt:
case CommandType::_blt: {
break;
}

//rs address
case CommandType::_la:
case CommandType::_lb:
case CommandType::_lh:
case CommandType::_lw:
case CommandType::_sb:
case CommandType::_sh:
case CommandType::_sw: {
break;
}

case CommandType::_nop:
case CommandType::_syscall: {
break;
}

default :
throw command_not_found(tmp);
}
*/