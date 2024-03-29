#ifndef __MipsSimulator
#define __MipsSimulator

#include <algorithm>
#include <bitset>
#include "Format.h"
#include "MipsParser.h"
using std::bitset;
using std::cout;
using std::cin;
using std::endl;

class MipsSimulator {
	MipsParser *code;
	Word Entry;
	unsigned int codeLimit;
	Memory *mem;
	bool withReturnValue;
	int returnValue;
	
	Word reg[32 + 3];
	bool regLock[32];
	Word &hi = reg[32];
	Word &lo = reg[33];
	Word &pc = reg[34];

	static const unsigned int BIT = 1023;
	bitset<1 << (10 + 1)> BHT;

	class IF_ID {
		friend class MipsSimulator;
		Word res1, res2;
		Word npc;
		Byte load, EXreg, MEMreg;
		Word EXdata, MEMdata;
		bool empty();
		IF_ID() noexcept { init(); }
		void init();
	}IFID;

	class ID_EX {
		friend class MipsSimulator;
		CommandType com;
		Word npc, a, b, imm;
		Byte res;
		bool empty();
		ID_EX() noexcept { init(); }
		void init();
	}IDEX;

	class EX_MEM {
		friend class MipsSimulator;
		CommandType com;
		Word ALUout, address;
		Byte res;
		bool empty();
		EX_MEM() noexcept { init(); }
		void init();
	}EXMEM;

	class MEM_WB {
		friend class MipsSimulator;
		CommandType com;
		Word result;
		Byte res;
		bool empty();
		MEM_WB() noexcept { init(); }
		void init();
	}MEMWB;


	bool getBranch(unsigned int i);
	void changeBranch(unsigned int i, bool sta);

	bool getReg(const IF_ID & in, const unsigned char id, Word & res) const;

	bool IF(IF_ID & write);
	bool ID(const IF_ID & get, ID_EX & write);
	bool EX(const ID_EX & get, EX_MEM  & write);
	bool MEM(const EX_MEM & get, MEM_WB & write);
	bool WB(const MEM_WB & get);

	bool tik_tok();

public:
	MipsSimulator(MipsParser *_code = nullptr) :code(_code) {};
	bool with_returnValue() { return withReturnValue; }
	int get_returnValue() { return returnValue; }
	bool run(Word _Entry, unsigned int len, Memory *_mem);
};

#endif // !__MipsSimulator
