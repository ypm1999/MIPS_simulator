#ifndef __MipsSimulator
#define __MipsSimulator

#include <algorithm>
#include "Format.h"
#include "MipsParser.h"

using std::cout;
using std::cin;
using std::endl;

class MipsSimulator {
	MipsParser *code;
	Memory *mem;

	Word reg[32 + 3];
	bool regLock[32];
	Word &hi = reg[32];
	Word &lo = reg[33];
	Word &pc = reg[34];

	bool withReturnValue;
	int returnValue;

	void __init();

	class IF_ID {
		friend class MipsSimulator;
		Word res1, res2;
		Word npc;
		Byte load, EXreg, MEMreg;
		Word EXdata, MEMdata;
		IF_ID() { init(); }
		void init();
	}IFID;

	class ID_EX {
		friend class MipsSimulator;
		Word npc;
		CommandType com;
		Word a, b, imm;
		Byte res;
		ID_EX() { init(); }
		void init();
	}IDEX;

	class EX_MEM {
		friend class MipsSimulator;
		CommandType com;
		Word ALUout, address;
		Byte res;
		EX_MEM() { init(); }
		void init();
	}EXMEM;

	class MEM_WB {
		friend class MipsSimulator;
		CommandType com;
		Word result;
		Byte res;
		MEM_WB() { init(); }
		void init();
	}MEMWB;


	bool getReg(const IF_ID & in, const unsigned char id, Word & res) const;

	bool IF(IF_ID & write, bool & mem_access);
	bool ID(IF_ID & get, ID_EX & write);
	bool EX(ID_EX &get, EX_MEM  &write);
	bool MEM(EX_MEM &get, MEM_WB &write, bool &mem_access);
	bool WB(MEM_WB &get);

public:
	MipsSimulator(MipsParser *_code, Memory *_mem) :code(_code), mem(_mem) {};

	bool with_returnValue() { return withReturnValue; }
	int get_returnValue() { return returnValue; }
	bool run();
};

#endif // !__MipsSimulator
