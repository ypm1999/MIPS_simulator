#include "MipsSimulator.h"

void MipsSimulator::__init() {
	memset(reg, 0, sizeof(reg));
	reg[29] = mem->top();
	pc = code->getAddress("main");
	if (pc.i == -1)
		throw without_main();
}
//
//Word MipsSimulator::runCommand(const int &i) {
//	const Command &com = code->command[i];
//	Word tmp, ad;
//
//	if (com.rt.ui != 255u) tmp = reg[com.rt.ui];
//	else tmp = com.cons;
//
//	if (com.address.i != -1) ad = com.address;
//	else ad = reg[com.rd.ui].i + com.offset.i;
//
//	switch (com._type) {
//		//--------------------------# plus and minus #---------------------
//	case CommandType::_add:
//		reg[com.rs.ui].i = reg[com.rd.ui].i + tmp.i;
//		break;
//	case CommandType::_addu:
//	case CommandType::_addiu:
//		reg[com.rs.ui].ui = reg[com.rd.ui].ui + tmp.ui;
//		break;
//	case CommandType::_sub:
//		reg[com.rs.ui].i = reg[com.rd.ui].i - tmp.i;
//		break;
//	case CommandType::_subu:
//		reg[com.rs.ui].ui = reg[com.rd.ui].ui - tmp.ui;
//		break;
//		//--------------------------# mul and div #---------------------
//	case CommandType::_mul:
//		if (com.rs.ui != 255u)
//			reg[com.rs.ui].i = reg[com.rd.ui].i * tmp.i;
//		else {
//			unsigned long long t = 1ll * reg[com.rd.ui].i * tmp.i;
//			hi.i = t >> 32;
//			lo.i = (unsigned int)t;
//		}
//		break;
//	case CommandType::_mulu:
//		if (com.rs.ui != 255u)
//			reg[com.rs.ui].ui = reg[com.rd.ui].ui * tmp.ui;
//		else {
//			unsigned long long t = 1llu * reg[com.rd.ui].ui * tmp.ui;
//			hi.ui = t >> 32;
//			lo.ui = (unsigned int)t;
//		}
//		break;
//	case CommandType::_div:
//		if (com.rs.ui != 255u)
//			reg[com.rs.ui].i = reg[com.rd.ui].i / tmp.i;
//		else {
//			lo.i = reg[com.rd.ui].i / tmp.i;
//			hi.i = reg[com.rd.ui].i - lo.i * tmp.i;
//		}
//		break;
//	case CommandType::_divu:
//		if (com.rs.ui != 255u)
//			reg[com.rs.ui].ui = reg[com.rd.ui].ui / tmp.ui;
//		else {
//			lo.ui = reg[com.rd.ui].ui / tmp.ui;
//			hi.ui = reg[com.rd.ui].ui - lo.ui * tmp.ui;
//		}
//		break;
//		//------------------------# xor #----------------------
//	case CommandType::_xor:
//		reg[com.rs.ui].i = reg[com.rd.ui].i ^ tmp.i;
//		break;
//	case CommandType::_xoru:
//		reg[com.rs.ui].ui = reg[com.rd.ui].ui ^ tmp.ui;
//		break;
//		//----------------------# negitave #---------------
//	case CommandType::_neg:
//		reg[com.rs.ui].i = -reg[com.rd.ui].i;
//		break;
//	case CommandType::_negu:
//		reg[com.rs.ui].ui = ~reg[com.rd.ui].ui;
//		break;
//		//----------------------# %(rest) #---------------
//	case CommandType::_rem:
//		reg[com.rs.ui].i = reg[com.rd.ui].i % tmp.i;
//		break;
//	case CommandType::_remu:
//		reg[com.rs.ui].ui = reg[com.rd.ui].ui % tmp.ui;
//		break;
//		//----------------------# load imm #---------------
//	case CommandType::_li:
//		reg[com.rs.ui].i = com.cons.i;
//		break;
//		//----------------------# cmp #---------------
//	case CommandType::_seq:
//		reg[com.rs.ui].i = reg[com.rd.ui].i == tmp.i;
//		break;
//	case CommandType::_sge:
//		reg[com.rs.ui].i = reg[com.rd.ui].i >= tmp.i;
//		break;
//	case CommandType::_sgt:
//		reg[com.rs.ui].i = reg[com.rd.ui].i > tmp.i;
//		break;
//	case CommandType::_sle:
//		reg[com.rs.ui].i = reg[com.rd.ui].i <= tmp.i;
//		break;
//	case CommandType::_slt:
//		reg[com.rs.ui].i = reg[com.rd.ui].i < tmp.i;
//		break;
//	case CommandType::_sne:
//		reg[com.rs.ui].i = reg[com.rd.ui].i != tmp.i;
//		break;
//
//		//----------------------# branch #---------------
//	case CommandType::_b:
//		return ad;
//		break;
//	case CommandType::_beq:
//		if (reg[com.rd.ui].i == tmp.i)
//			return ad;
//		break;
//	case CommandType::_bne:
//		if (reg[com.rd.ui].i != tmp.i)
//			return ad;
//		break;
//	case CommandType::_bge:
//		if (reg[com.rd.ui].i >= tmp.i)
//			return ad;
//		break;
//	case CommandType::_ble:
//		if (reg[com.rd.ui].i <= tmp.i)
//			return ad;
//		break;
//	case CommandType::_bgt:
//		if (reg[com.rd.ui].i > tmp.i)
//			return ad;
//		break;
//	case CommandType::_blt:
//		if (reg[com.rd.ui].i < tmp.i)
//			return ad;
//		break;
//		//----------------# branch, cmp with zero #---------------
//	case CommandType::_beqz:
//		if (reg[com.rd.ui].i == 0)
//			return ad;
//		break;
//	case CommandType::_bnez:
//		if (reg[com.rd.ui].i != 0)
//			return ad;
//		break;
//	case CommandType::_bgez:
//		if (reg[com.rd.ui].i >= 0)
//			return ad;
//		break;
//	case CommandType::_blez:
//		if (reg[com.rd.ui].i <= 0)
//			return ad;
//		break;
//	case CommandType::_bgtz:
//		if (reg[com.rd.ui].i > 0)
//			return ad;
//		break;
//	case CommandType::_bltz:
//		if (reg[com.rd.ui].i < 0)
//			return ad;
//		break;
//
//		//----------------------# jump #---------------
//	case CommandType::_j:
//		return ad;
//		break;
//	case CommandType::_jr:
//		return reg[com.rs.ui];
//		break;
//	case CommandType::_jal:
//		reg[31] = i + 1;
//		return ad;
//		break;
//	case CommandType::_jalr:
//		reg[31] = i + 1;
//		return reg[com.rs.ui];
//		break;
//		//----------------------# move #---------------
//	case CommandType::_move:
//		reg[com.rs.ui] = reg[com.rd.ui];
//		break;
//	case CommandType::_mfhi:
//		reg[com.rs.ui] = hi;
//		break;
//	case CommandType::_mflo:
//		reg[com.rs.ui] = lo;
//		break;
//		//----------------------# load #---------------
//	case CommandType::_la:
//		reg[com.rs.ui] = ad;
//		break;
//	case CommandType::_lb:
//		reg[com.rs.ui] = mem->getByte(ad).i;
//		break;
//	case CommandType::_lh:
//		reg[com.rs.ui] = mem->getHalf(ad).i;
//		break;
//	case CommandType::_lw:
//		reg[com.rs.ui] = mem->getWord(ad).i;
//		break;
//		//----------------------# store #---------------
//	case CommandType::_sb:
//		mem->writeByte(ad, reg[com.rs.ui].i);
//		break;
//	case CommandType::_sh:
//		mem->writeHalf(ad, reg[com.rs.ui].i);
//		break;
//	case CommandType::_sw:
//		mem->writeWord(ad, reg[com.rs.ui].i);
//		break;
//		//----------------------# move #---------------
//	case CommandType::_nop:
//		break;
//	case CommandType::_syscall: {
//		switch (reg[2].i) {
//		case 1:
//			std::cout << reg[4].i;
//			break;
//		case 4:
//			std::cout << mem->getString(reg[4]);
//			break;
//		case 5:
//			std::cin >> reg[2].i;
//			break;
//		case 8: {
//			string str;
//			std::cin >> str;
//			mem->writeString(reg[4], reg[5].i, str);
//			break;
//		}
//		case 9:
//			mem->algin(2);
//			reg[2] = mem->getSpace(reg[4].i);
//			break;
//		case 10:
//			return EXIT_WITHOUT_VALUE;
//			break;
//		case 17:
//			return EXIT_WITH_VALUE;
//			break;
//		default:
//			throw run_command_error("syscall");
//		}
//		break;
//	}
//
//	case CommandType::none:
//	default:
//		throw command_not_found();
//	}
//	return -1;
//}
//
//Word MipsSimulator::runBlock(const Word &address) {
//	for (int i = address.i; i < (int)code->command.size(); i++) {
//		Word next = runCommand(i);
//		if (next.i != -1)
//			return next;
//	}
//	return 0x3fffffff;
//}


void MipsSimulator::IF_ID::init()
{
	res1.b0 = (char)CommandType::none;
	load = EXreg = MEMreg = 255u;
}

void MipsSimulator::ID_EX::init() {
	com = CommandType::none;
	res = 255u;
}

void MipsSimulator::EX_MEM::init()
{
	com = CommandType::none;
	res = 255u;
}


void MipsSimulator::MEM_WB::init()
{
	com = CommandType::none;
	res = 255u;
}


bool MipsSimulator::getReg(const IF_ID &in, const char &id, Word &res) const {
	if (!regLock[id]) {
		res = reg[id];
		return true;
	}
	else {
		if (in.load.ui == id)
			return false;
		if (in.EXreg.ui == id) {
			res = in.EXdata;
			return true;
		}
		if(in.MEMreg.ui == id) {
			res = in.EXdata;
			return true;
		}
	}
}

bool MipsSimulator::IF(IF_ID &write, bool &mem_access) {
	write.init();
	if (pc.i == -1 || !mem_access)
		return false;
	if (pc.i < 0 || pc.i >code->getLimit())
		throw command_address_error(pc.i);
	write.res1 = mem->getWord(pc << 3);
	write.res2 = mem->getWord((pc << 3) + (commandSize >> 1));
	write.npc = pc.i++;
}

bool MipsSimulator::ID(const IF_ID &get, ID_EX &write) {
	if ((CommandType)get.res1.b0 == CommandType::none)
		return false;
	write.init();
	write.npc = get.npc;
	//TODO Lock reg
	switch ((CommandType)get.res1.b0) {
	//rs rd src
	case CommandType::_add:
	case CommandType::_addu:
	case CommandType::_sub:
	case CommandType::_subu:
	case CommandType::_addiu:
	case CommandType::_xor:
	case CommandType::_xoru:
	case CommandType::_rem:
	case CommandType::_remu:
	case CommandType::_seq:
	case CommandType::_sge:
	case CommandType::_sgt:
	case CommandType::_sle:
	case CommandType::_slt:
	case CommandType::_sne: {
		if (!getReg(get, get.res1.b2, write.a))
			return false;
		if (get.res1.b3 != 255u) {
			if (!getReg(get, get.res1.b3, write.b))
				return false;
		}
		else
			write.b = get.res2;
		write.res = get.res1.b1;
		
		break;
	}

	// rd rs
	case CommandType::_neg:
	case CommandType::_negu:
	case CommandType::_move: {
		if (get.res1.b2 != 255u) {
			if (!getReg(get, get.res1.b2, write.a))
				return false;
		}
		else
			write.a = get.res2;
		write.res = get.res1.b1;
		break;
	}
	//rs src / rd rs src
	case CommandType::_mul:
	case CommandType::_mulu:
	case CommandType::_div:
	case CommandType::_divu: {
		if (!getReg(get, get.res1.b2, write.a))
			return false;
		if (get.res1.b3 != 255u) {
			if (!getReg(get, get.res1.b3, write.b))
				return false;
		}
		else
			write.b = get.res2;
		write.res.ui = get.res1.b1;
		break;
	}
	//rd,cons / rd,address
	case CommandType::_li:
	case CommandType::_la:
	case CommandType::_lb:
	case CommandType::_lh:
	case CommandType::_lw:
	case CommandType::_sb:
	case CommandType::_sh:
	case CommandType::_sw: {
		write.res.ui = get.res1.b1;
		write.imm = get.res2;
		break;
	}

	//rd
	case CommandType::_mfhi:
	case CommandType::_mflo:
	case CommandType::_jr:
	case CommandType::_jalr: {
		write.res.ui = get.res1.b1;
		break;
	}

	//label
	case CommandType::_b:
	case CommandType::_j:
	case CommandType::_jal: {
		write.imm = ((unsigned int)get.res1.b2 << 8) + (unsigned int)get.res1.b3;
		break;
	}

	//rs label
	case CommandType::_beqz:
	case CommandType::_bnez:
	case CommandType::_bgez:
	case CommandType::_blez:
	case CommandType::_bgtz:
	case CommandType::_bltz: {
		if (!getReg(get, get.res1.b1, write.a))
			return false;
		write.imm = ((unsigned int)get.res1.b2 << 8) + (unsigned int)get.res1.b3;
		break;
	}

	//rs rt label
	case CommandType::_beq:
	case CommandType::_bne:
	case CommandType::_bge:
	case CommandType::_ble:
	case CommandType::_bgt:
	case CommandType::_blt: {
		if (!getReg(get, get.res1.b1 & 0x3f, write.a))
			return false;
		write.a = get.res1.b1 & 0x3f;
		if (get.res1.b1 & (1 << 7)) {
			if (!getReg(get, get.res2.b0, write.a))
				return false;
		}
		else
			write.b = get.res2;
		write.imm = ((unsigned int)get.res1.b2 << 8) + (unsigned int)get.res1.b3;
		break;
	}
	case CommandType::_syscall: {
		if (!getReg(get, 2, write.imm))
			return false;
		switch (write.imm) {
		case 1:
		case 4:
			if (!getReg(get, 4, write.imm))
				return false;
			break;
		case 5:
			write.res.ui = 2;
			break;
		case 8:
			if (!getReg(get, 4, write.a))
				return false;
			if (!getReg(get, 5, write.b))
				return false;
			break;
		case 9:
			if (!getReg(get, 4, write.imm))
				return false;
			write.res.ui = 2;
			break;
		case 10:
			break;
		case 17:
			if (!getReg(get, 4, write.imm))
				return false;
			break;
		default:
			throw run_command_error(string("syscall with code ") + std::to_string(write.a.i));
		}
	}
	case CommandType::_nop:
		break;
	case CommandType::none:
	default:
		throw command_not_found(std::to_string(get.res1.b0));
	}
	if(write.res.ui != 255u)
		regLock[write.res] = true;
	write.com = (CommandType)get.res1.b0;
	return true;
}

bool MipsSimulator::EX(const ID_EX &get, EX_MEM  &write) {
	if (get.com == CommandType::none)
		return false;
	write.init();
	//TODO .init();
}

bool MipsSimulator::MEM(const EX_MEM &get, MEM_WB &write, bool &mem_access) {
	if (get.com == CommandType::none)
		return false;
	write.init();
	//TODO .init();
}

bool MipsSimulator::WB(const MEM_WB &get) {
	if (get.com == CommandType::none)
		return false;

	if (get.res.ui != 255u) {
		regLock[get.res.ui] = false;
		reg[get.res.ui] = get.ALUout;
	}

	return true;
}

MipsSimulator::MipsSimulator(MipsParser *_code, Memory *_mem)
	:code(_code), mem(_mem) {}

bool MipsSimulator::MipsSimulator::with_returnValue() {
	return withReturnValue;
}

int MipsSimulator::get_returnValue() {
	return returnValue;
}

bool MipsSimulator::run() {
	__init();
	int now = 0;
	while (pc.i != -1) {
		bool mem_access = true;
		WB(MEMWB[now]);

		if (MEM(EXMEM[now], MEMWB[now ^ 1], mem_access)) {
			if (EXMEM[now].res.ui != 255u) {
				switch (EXMEM[now].com) {
				case CommandType::_lb:
				case CommandType::_lh:
				case CommandType::_lw:
					IFID[now].load = MEMWB[now ^ 1].res;
					break;
				default:
					IFID[now].MEMreg = MEMWB[now ^ 1].res;
					IFID[now].MEMdata = MEMWB[now ^ 1].ALUout;
				}
			}
		}
		if (EX(IDEX[now], EXMEM[now ^ 1])) {
			if (IDEX[now].res.ui != 255u) {
				IFID[now].EXreg = EXMEM[now ^ 1].res;
				IFID[now].EXdata = EXMEM[now ^ 1].ALUout;
			}
		}

		if (ID(IFID[now], IDEX[now ^ 1])) {

		}
		
		IF(IFID[now ^ 1], mem_access);
		
		now ^= 1;
	}
}
//
//bool MipsSimulator::run_without_pipeline(){
//	__init();
//	auto now = pc;
//	withReturnValue = false;
//	while (true) {
//		//string name = code->getname(now);
//		Word next = runBlock(now);
//		if (next.i == EXIT_WITHOUT_VALUE)
//			return true;
//		if (next.i == EXIT_WITH_VALUE) {
//			withReturnValue = true;
//			returnValue = reg[4].i;
//			return true;
//		}
//		if (next.i < 0 || next.ui > code->command.size()) {
//			throw command_address_error(next.i);
//		}
//		now = next;
//	}
//	return false;
//}

/*
switch () {
	//--------------------------# plus and minus #---------------------
case CommandType::_add:
case CommandType::_addu:
case CommandType::_addiu:
case CommandType::_sub:
case CommandType::_subu:
	//--------------------------# mul and div #---------------------
case CommandType::_mul:
case CommandType::_mulu:
case CommandType::_div:
case CommandType::_divu:
	//------------------------# xor #----------------------
case CommandType::_xor:
case CommandType::_xoru:
	//----------------------# negitave #---------------
case CommandType::_neg:
case CommandType::_negu:
	//----------------------# %(rest) #---------------
case CommandType::_rem:
case CommandType::_remu:
	//----------------------# load imm #---------------
case CommandType::_li:
	//----------------------# cmp #---------------
case CommandType::_seq:
case CommandType::_sge:
case CommandType::_sgt:
case CommandType::_sle:
case CommandType::_slt:
case CommandType::_sne:
	//----------------------# branch #---------------
case CommandType::_b:
case CommandType::_beq:
case CommandType::_bne:
case CommandType::_bge:
case CommandType::_ble:
case CommandType::_bgt:
case CommandType::_blt:
	//----------------# branch, cmp with zero #---------------
case CommandType::_beqz:
case CommandType::_bnez:
case CommandType::_bgez:
case CommandType::_blez:
case CommandType::_bgtz:
case CommandType::_bltz:
	//----------------------# jump #---------------
case CommandType::_j:
case CommandType::_jr:
case CommandType::_jal:
case CommandType::_jalr:
	//----------------------# move #---------------
case CommandType::_move:
case CommandType::_mfhi:
case CommandType::_mflo:
	//----------------------# load #---------------
case CommandType::_la:
case CommandType::_lb:
case CommandType::_lh:
case CommandType::_lw:
	//----------------------# store #---------------
case CommandType::_sb:
case CommandType::_sh:
case CommandType::_sw:
case CommandType::_nop:
case CommandType::_syscall:
default:
	throw command_not_found();
}
*/

