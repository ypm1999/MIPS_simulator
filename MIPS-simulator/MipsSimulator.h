#ifndef __MipsSimulator
#define __MipsSimulator

#include "Format.h"
#include "MispParser.h"


class MipsSimulator {
	MipsParser *code;
	Memory *mem;
	Word reg[32 + 2];
	Word &hi = reg[32];
	Word &lo = reg[33];

	Word runCommand(const int &i){
		const Command &com = code->command[i];
		Word tmp, ad;

		if (com.rt.ui != 255) tmp = reg[com.rt.ui];
		else tmp = com.cons;

		if (com.address.i != -1) ad = com.address;
		else ad = reg[com.rd.ui].i + com.offset.i;

		switch (com._type){
		//--------------------------# plus and minus #---------------------
		case CommandType::_add:
			reg[com.rs.ui].i = reg[com.rd.ui].i + tmp.i;
			break;
		case CommandType::_addu:
		case CommandType::_addiu:
			reg[com.rs.ui].ui = reg[com.rd.ui].ui + tmp.ui;
			break;
		case CommandType::_sub:
			reg[com.rs.ui].i = reg[com.rd.ui].i - tmp.i;
			break;
		case CommandType::_subu:
		case CommandType::_subiu:
			reg[com.rs.ui].ui = reg[com.rd.ui].ui - tmp.ui;
			break;
		//--------------------------# mul and div #---------------------
		case CommandType::_mul:
			if (com.rt.ui != 255)
				reg[com.rs.ui].i = reg[com.rd.ui].i * tmp.i;
			else {
				long long t = 1ll * reg[com.rd.ui].i * com.cons.i;
				hi.i = t >> 32;
				lo.i = (int)t;
			}
			break;
		case CommandType::_mulu:
			if (com.rt.ui != 255)
				reg[com.rs.ui].ui = reg[com.rd.ui].ui * tmp.ui;
			else {
				unsigned long long t = 1ll * reg[com.rd.ui].ui * com.cons.ui;
				hi.ui = t >> 32;
				lo.ui = (unsigned int)t;
			}
			break;
		case CommandType::_div:
			lo.i = reg[com.rd.ui].i / tmp.i;
			hi.i = reg[com.rd.ui].i - lo.i * tmp.i;
			break;
		case CommandType::_divu:
			lo.ui = reg[com.rd.ui].ui / tmp.ui;
			hi.ui = reg[com.rd.ui].ui - lo.ui * tmp.ui;
			break;
		//------------------------# xor #----------------------
		case CommandType::_xor:
			reg[com.rs.ui].i = reg[com.rd.ui].i ^ tmp.i;
			break;
		case CommandType::_xoru:
			reg[com.rs.ui].ui = reg[com.rd.ui].ui ^ tmp.ui;
			break;
		//----------------------# negitave #---------------
		case CommandType::_neg:
			reg[com.rs.ui].i = -reg[com.rd.ui].i;
		case CommandType::_negu:
			reg[com.rs.ui].ui = ~reg[com.rd.ui].ui;
		//----------------------# %(rest) #---------------
		case CommandType::_rem:
			reg[com.rs.ui].i = reg[com.rd.ui].i % tmp.i;
			break;
		case CommandType::_remu:
			reg[com.rs.ui].ui = reg[com.rd.ui].ui % tmp.ui;
			break;
		//----------------------# load imm #---------------
		case CommandType::_li:
			reg[com.rs.ui].i = com.cons.i;
		//----------------------# cmp #---------------
		case CommandType::_seq:
			reg[com.rs.ui].i = reg[com.rd.ui].i == tmp.i;
			break;
		case CommandType::_sge:
			reg[com.rs.ui].i = reg[com.rd.ui].i >= tmp.i;
			break;
		case CommandType::_sgt:
			reg[com.rs.ui].i = reg[com.rd.ui].i > tmp.i;
			break;
		case CommandType::_sle:
			reg[com.rs.ui].i = reg[com.rd.ui].i <= tmp.i;
			break;
		case CommandType::_slt:
			reg[com.rs.ui].i = reg[com.rd.ui].i < tmp.i;
			break;
		case CommandType::_sne:
			reg[com.rs.ui].i = reg[com.rd.ui].i != tmp.i;
			break;
		
		//----------------------# branch #---------------
		case CommandType::_b:
			return ad;
		case CommandType::_beq:
			if (reg[com.rd.ui].i == tmp.i)
				return ad;
			break;
		case CommandType::_bne:
			if (reg[com.rd.ui].i != tmp.i)
				return ad;
			break;
		case CommandType::_bge:
			if (reg[com.rd.ui].i >= tmp.i)
				return ad;
			break;
		case CommandType::_ble:
			if (reg[com.rd.ui].i <= tmp.i)
				return ad;
			break;
		case CommandType::_bgt:
			if (reg[com.rd.ui].i > tmp.i)
				return ad;
			break;
		case CommandType::_blt:
			if (reg[com.rd.ui].i < tmp.i)
				return ad;
			break;
		//----------------# branch, cmp with zero #---------------
		case CommandType::_beqz:
			if (reg[com.rd.ui].i == 0)
				return ad;
			break;
		case CommandType::_bnez:
			if (reg[com.rd.ui].i != 0)
				return ad;
			break;
		case CommandType::_bgez:
			if (reg[com.rd.ui].i >= 0)
				return ad;
			break;
		case CommandType::_blez:
			if (reg[com.rd.ui].i <= 0)
				return ad;
			break;
		case CommandType::_bgtz:
			if (reg[com.rd.ui].i > 0)
				return ad;
			break;
		case CommandType::_bltz:
			if (reg[com.rd.ui].i < 0)
				return ad;
			break;
		
		//----------------------# jump #---------------
		case CommandType::_j:
			return ad;
		case CommandType::_jr:
			return reg[com.rs.ui];
		case CommandType::_jal:
			reg[31] = i + 1;
			return ad;
		case CommandType::_jalr:
			reg[31] = i + 1;
			return reg[com.rs.ui];
		//----------------------# move #---------------
		case CommandType::_move:
			reg[com.rs.ui] = reg[com.rd.ui];
			break;
		case CommandType::_mfhi:
			reg[com.rs.ui] = hi;
			break;
		case CommandType::_mflo:
			reg[com.rs.ui] = lo;
			break;
		//----------------------# load #---------------
		case CommandType::_la:
			reg[com.rs.ui] = ad;
			break;
		case CommandType::_lb:
			reg[com.rs.ui] = mem->getByte(ad).i;
			break;
		case CommandType::_lh:
			reg[com.rs.ui] = mem->getHalf(ad).i;
			break;
		case CommandType::_lw:
			reg[com.rs.ui] = mem->getWord(ad).i;
			break;
		//----------------------# store #---------------
		case CommandType::_sb:
			mem->writeByte(ad, reg[com.rs.ui].i);
			break;
		case CommandType::_sh:
			mem->writeHalf(ad, reg[com.rs.ui].i);
			break;
		case CommandType::_sw:
			mem->writeWord(ad, reg[com.rs.ui].i);
			break;
		//----------------------# move #---------------
		case CommandType::_nop:
			break;
		case CommandType::_syscall: {
			switch (reg[2].i) {
			case 1:
				std::cout << reg[4].i;
			case 4:
				std::cout << mem->getString(reg[4]);
			case 5:
				std::cin >> reg[4].i;
			case 8: {
				string s;
				std::cin >> s;
				mem->writeString(reg[4], reg[5].i, s.c_str());
			}
			case 9:
				mem->algin(2);
				reg[2] = mem->getSpace(reg[4].i);
			case 10:
				return -1;
			case 17:
				return -2;
			default:
				throw run_command_error("syscall");
			}
		}

		case CommandType::none:
		default:
			throw command_not_found();
		}
	}

	Word runBlock(const Word &adress){
		const Command &tmp = code->command[adress.ui];
		for (int i = tmp.address.i; i < tmp.offset.i; i++) {
			Word next = runCommand(i);
			if (next.i != 0)
				return next;
		}
		return 0x3fffffff;
	}

public:
	MipsSimulator(MipsParser *_code = nullptr, Memory *_mem = nullptr)
		:code(_code), mem(_mem) {}

	bool run(int &value) {
		auto main = code->commandMap.find("main");
		if (main == code->commandMap.end())
			throw without_main();
		auto now = (*main).second;
		while (true) {
			Word next = runBlock(now);
			if (next.i == -1)
				return true;
			if (next.i == -2) {
				value = reg[4].i;
				return true;
			}
			if (next.i < 0 || next.i > code->command.size())
				return false;
			now = next;
		}
	}
};

#endif // !__MipsSimulator
