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
	Word &hi = reg[32];
	Word &lo = reg[33];
	Word &pc = reg[34];
	bool withReturnValue;
	int returnValue;
	
	static const int EXIT_WITHOUT_VALUE = -2;
	static const int EXIT_WITH_VALUE = -3;

	void __init();

	Word runCommand(const int &i);

	Word runBlock(const Word &address);

	class IF_ID {
		friend class MipsSimulator;
		Word IR, npc;
	}IFID[2];
	class ID_EX {
		friend class MipsSimulator;
		CommandType com;
		Word a, b, imm;
		Byte res;
	}IDEX[2];
	class EX_MEM {
		friend class MipsSimulator;
		CommandType com;
		Word ALUout;
		Byte res;
	}EXMEM[2];
	class MEM_WB {
		friend class MipsSimulator;
		CommandType com;
		Word ALUout;
		Byte res;
	}MEMWB[2];

	void IF(IF_ID &write);
	void ID(const IF_ID &get, ID_EX &write);
	void EX(const ID_EX &get, EX_MEM  &write);
	void MEM(const EX_MEM &get, MEM_WB &write);
	void WB(const MEM_WB &get);

public:
	MipsSimulator(MipsParser *_code = nullptr, Memory *_mem = nullptr);
	bool with_returnValue();
	int get_returnValue();
	bool run();
	bool run_without_pipeline();
};

#endif // !__MipsSimulator
