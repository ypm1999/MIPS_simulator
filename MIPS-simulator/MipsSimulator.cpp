#include "MipsSimulator.h"

void MipsSimulator::__init() {
	memset(reg, 0, sizeof(reg));
	reg[29].ui = mem->getSize() - 1;
	pc = code->getAddress("main");
	if (pc.i == 0)
		throw without_main();
}


void MipsSimulator::IF_ID::init() {
	res1.b0 = (unsigned char)CommandType::none;
	load.ui = EXreg.ui = MEMreg.ui = 255u;
}

void MipsSimulator::ID_EX::init() {
	com = CommandType::none;
	res.ui = 255u;
}

void MipsSimulator::EX_MEM::init() {
	com = CommandType::none;
	res.ui = 255u;
}

void MipsSimulator::MEM_WB::init() {
	com = CommandType::none;
	res.ui = 255u;
}


bool MipsSimulator::getReg(const IF_ID &in, const unsigned char id, Word &res) const {
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
	return false;
}


bool MipsSimulator::IF(IF_ID &write, bool &mem_access) {
	if (pc.ui == 0 || !mem_access)
		return false;
	if (pc.ui > code->getLimit())
		throw command_address_error(pc.ui);
	write.res1 = mem->getWord(pc.ui << 3);
	write.res2 = mem->getWord((pc.ui << 3) + (commandSize >> 1));
	code->output(pc.ui);
	pc.ui++;
	write.npc = pc;
	return true;
}

bool MipsSimulator::ID(IF_ID &get, ID_EX &write) {
	if ((CommandType)get.res1.b0 == CommandType::none)
		return false;
	write.npc = get.npc;
	write.com = (CommandType)get.res1.b0;
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
		write.res.ui = get.res1.b1;
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
		write.res.ui = get.res1.b1;
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
	//rd,cons
	case CommandType::_li:
		write.imm = get.res2;
		write.res.ui = get.res1.b1;
		break;
	// rd, address
	case CommandType::_la:
	case CommandType::_lb:
	case CommandType::_lh:
	case CommandType::_lw:
		if (get.res1.b2 != 255u) {
			if (!getReg(get, get.res1.b1, write.imm))
				return false;
			write.imm.ui += get.res2.i;
		}
		else
			write.imm = get.res2;
		write.res.ui = get.res1.b1;
		break;
	case CommandType::_sb:
	case CommandType::_sh:
	case CommandType::_sw:
		if (!getReg(get, get.res1.b1, write.a))
			return false;
		if (get.res1.b2 != 255u) {
			if (!getReg(get, get.res1.b2, write.imm))
				return false;
			write.imm.ui += get.res2.i;
		}
		else
			write.imm = get.res2;
		break;
	//rd
	case CommandType::_mfhi:
	case CommandType::_mflo:
		write.res.ui = get.res1.b1;
	case CommandType::_jr:
		if (!getReg(get, get.res1.b1, write.imm))
			return false;
		break;
	case CommandType::_jalr:
		if (!getReg(get, get.res1.b1, write.imm))
			return false;
		write.res.ui = 31u;
		break;

	//label
	case CommandType::_b:
	case CommandType::_j:
		write.imm.ui = ((unsigned int)get.res1.b3 << 8) + (unsigned int)get.res1.b2;
		break;
	case CommandType::_jal:
		write.imm.ui = ((unsigned int)get.res1.b3 << 8) + (unsigned int)get.res1.b2;
		write.res.ui = 31u;
		break;

	//rs label
	case CommandType::_beqz:
	case CommandType::_bnez:
	case CommandType::_bgez:
	case CommandType::_blez:
	case CommandType::_bgtz:
	case CommandType::_bltz: {
		if (!getReg(get, get.res1.b1, write.a))
			return false;
		write.imm.ui = ((unsigned int)get.res1.b3 << 8) + (unsigned int)get.res1.b2;
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
		if (get.res1.b1 & (1u << 7u)) {
			if (!getReg(get, get.res2.b0, write.b))
				return false;
		}
		else
			write.b = get.res2;
		write.imm.ui = ((unsigned int)get.res1.b3 << 8) + (unsigned int)get.res1.b2;
		break;
	}
	case CommandType::_syscall: {
		if (!getReg(get, 2u, write.imm))
			return false;
		switch (write.imm.i) {
		case 1:
			if (!getReg(get, 4u, write.a))
				return false;
			write.com = CommandType::_syscall1;
			break;
		case 4:
			if (!getReg(get, 4u, write.a))
				return false;
			write.com = CommandType::_syscall4;
			break;
		case 5:
			write.res.ui = 2;
			write.com = CommandType::_syscall5;
			break;
		case 8:
			if (!getReg(get, 4u, write.a))
				return false;
			if (!getReg(get, 5u, write.b))
				return false;
			write.com = CommandType::_syscall8;
			break;
		case 9:
			if (!getReg(get, 4u, write.a))
				return false;
			write.res.ui = 2;
			write.com = CommandType::_syscall9;
			break;
		case 10:
			write.com = CommandType::_syscall10;
			break;
		case 17:
			if (!getReg(get, 4u, write.a))
				return false;
			write.com = CommandType::_syscall17;
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
		regLock[write.res.ui] = true;
	get.init();
	return true;
}

bool MipsSimulator::EX(ID_EX &get, EX_MEM  &write) {
	if (get.com == CommandType::none)
		return false;
	write.com = get.com;
	write.res = get.res;
	switch (get.com) {
	//--------------------------# plus and minus #---------------------
	case CommandType::_add:
		write.ALUout.i = get.a.i + get.b.i;
		break;
	case CommandType::_addu:
	case CommandType::_addiu:
		write.ALUout.ui = get.a.ui + get.b.ui;
		break;
	case CommandType::_sub:
		write.ALUout.i = get.a.i - get.b.i;
		break;
	case CommandType::_subu:
		write.ALUout.ui = get.a.ui - get.b.ui;
		break;
	//--------------------------# mul and div #---------------------
	case CommandType::_mul:
		if(get.res.ui != 255u)
			write.ALUout.i = get.a.i * get.b.i;
		else {
			unsigned long long t = 1ll * get.a.i * get.b.i;
			hi.ui = t >> 32;
			lo.ui = (unsigned int)t;
		}
		break;
	case CommandType::_mulu:
		if (get.res.ui != 255u)
			write.ALUout.ui = get.a.ui * get.b.ui;
		else {
			unsigned long long t = 1llu * get.a.ui * get.b.ui;
			hi.ui = t >> 32;
			lo.ui = (unsigned int)t;
		}
		break;
	case CommandType::_div:
		if (get.res.ui != 255u)
			write.ALUout.i = get.a.i / get.b.i;
		else {
			lo.i = get.a.i / get.b.i;
			hi.i = get.a.i - lo.i * get.b.i;
		}
		break;
	case CommandType::_divu:
		if (get.res.ui != 255u)
			write.ALUout.ui = get.a.ui / get.b.ui;
		else {
			lo.ui = get.a.ui / get.b.ui;
			hi.ui = get.a.ui - lo.ui * get.b.ui;
		}
		break;
	//------------------------# xor #----------------------
	case CommandType::_xor:
		write.ALUout.i = get.a.i ^ get.b.i;
		break;
	case CommandType::_xoru:
		write.ALUout.ui = get.a.ui ^ get.b.ui;
		break;
	//----------------------# negitave #---------------
	case CommandType::_neg:
		write.ALUout.i = -get.a.i;
		break;
	case CommandType::_negu:
		write.ALUout.ui = ~get.a.ui;
		break;
	//----------------------# %(rest) #---------------
	case CommandType::_rem:
		write.ALUout.i = get.a.i % get.b.i;
		break;
	case CommandType::_remu:
		write.ALUout.ui = get.a.ui % get.b.ui;
		break;
	//----------------------# load imm #---------------
	case CommandType::_li:
	case CommandType::_la:
	case CommandType::_lb:
	case CommandType::_lh:
	case CommandType::_lw:
		write.address = get.imm;
		break;
	case CommandType::_sb:
	case CommandType::_sh:
	case CommandType::_sw:
		write.address = get.imm;
		write.ALUout = get.a;
		break;
	//----------------------# cmp #---------------
	case CommandType::_seq:
		write.ALUout.i = get.a.i == get.b.i;
		break;
	case CommandType::_sge:
		write.ALUout.i = get.a.i >= get.b.i;
		break;
	case CommandType::_sgt:
		write.ALUout.i = get.a.i > get.b.i;
		break;
	case CommandType::_sle:
		write.ALUout.i = get.a.i <= get.b.i;
		break;
	case CommandType::_slt:
		write.ALUout.i = get.a.i < get.b.i;
		break;
	case CommandType::_sne:
		write.ALUout.i = get.a.i != get.b.i;
		break;
	//----------------------# branch #---------------
	case CommandType::_beq:
		if (get.a == get.b)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_bne:
		if (get.a != get.b)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_bge:
		if (get.a >= get.b)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_ble:
		if (get.a <= get.b)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_bgt:
		if (get.a > get.b)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_blt:
		if (get.a < get.b)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	//----------------# branch, cmp with zero #---------------
	case CommandType::_beqz:
		if (get.a == 0)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_bnez:
		if (get.a != 0)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_bgez:
		if (get.a >= 0)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_blez:
		if (get.a <= 0)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_bgtz:
		if (get.a > 0)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_bltz:
		if (get.a < 0)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	//----------------------# jump #---------------
	// b/j/jal/jr/jalr will jumb after ID;
	// so they don't need record address.
	case CommandType::_jal:
	case CommandType::_jalr:
		write.address = get.npc;
		break;
	//----------------------# move #---------------
	case CommandType::_move:
		write.ALUout = get.a;
		break;
	case CommandType::_mfhi:
		write.ALUout = hi;
		break;
	case CommandType::_mflo:
		write.ALUout = lo;
		break;
		//----------------------# syscall #---------------
	case CommandType::_syscall1:
	case CommandType::_syscall9:
		write.ALUout = get.a;
		break;
	case CommandType::_syscall4:
		write.address = get.a;
		break;
	case CommandType::_syscall8:
		write.address = get.a;
		write.ALUout = get.b;
		break;
	case CommandType::_syscall10:
		withReturnValue = false;
		pc = 0;
		break;
	case CommandType::_syscall17:
		returnValue = get.a;
		withReturnValue = true;
		pc = 0;
		break;
	//case CommandType::_nop:
	default:
		break;
	}
	get.init();
	return true;
}

bool MipsSimulator::MEM(EX_MEM &get, MEM_WB &write, bool &mem_access) {
	if (get.com == CommandType::none || !mem_access)
		return false;
	write.com = get.com;
	write.res = get.res;
	mem_access = false;
	switch (get.com) {
		//----------------------# load #-------------------
	case CommandType::_lb:
		write.result = (Word)mem->getByte(get.address);
		break;
	case CommandType::_lh:
		write.result = (Word)mem->getHalf(get.address);
		break;
	case CommandType::_lw:
		write.result = (Word)mem->getWord(get.address);
		break;
		//----------------------# store #---------------
	case CommandType::_sb:
		mem->writeByte(get.address, (Byte)get.ALUout);
		break;
	case CommandType::_sh:
		mem->writeHalf(get.address, (Half)get.ALUout);
		break;
	case CommandType::_sw:
		mem->writeWord(get.address, (Word)get.ALUout);
		break;
		//----------------------# syscall #---------------
	case CommandType::_syscall1:
		cout << get.ALUout.i;
		break;
	case CommandType::_syscall4:
		cout << mem->getString(get.address);
		break;
	case CommandType::_syscall5:
		cin >> write.result.i;
		break;
	case CommandType::_syscall8: {
		string str;
		cin >> str;
		mem->writeString(get.address, get.ALUout, str + '\0');
		break;
	}
	case CommandType::_syscall9:
		mem->algin(2);
		write.result = mem->getSpace(get.ALUout);
		break;
	default:
		mem_access = true;
		if ((get.com >= CommandType::_beq && get.com <= CommandType::_bltz)
			|| (get.com >= CommandType::_jal && get.com <= CommandType::_la))
			write.result = get.address;
		else
			write.result = get.ALUout;
	}
	get.init();
	return true;
}

bool MipsSimulator::WB(MEM_WB &get) {
	if (get.com == CommandType::none)
		return false;

	if (get.res.ui != 255u) {
		regLock[get.res.ui] = false;
		reg[get.res.ui] = get.result;
	}

	get.init();
	return true;
}


bool MipsSimulator::run() {
	__init();
	while (pc.i != 0) {
		bool mem_access = true;
		WB(MEMWB);

		if (MEM(EXMEM, MEMWB, mem_access)) {
			if (EXMEM.res.ui != 255u) {
				switch (EXMEM.com) {
				case CommandType::_lb:
				case CommandType::_lh:
				case CommandType::_lw:
					IFID.load = MEMWB.res;
					break;
				default:
					IFID.MEMreg = MEMWB.res;
					IFID.MEMdata = MEMWB.result;
				}
			}
		}

		if (EX(IDEX, EXMEM)) {
			if (IDEX.res.ui != 255u) {
				IFID.EXreg = EXMEM.res;
				IFID.EXdata = EXMEM.ALUout;
				if ((EXMEM.com >= CommandType::_beq && EXMEM.com <= CommandType::_bltz)
					|| (EXMEM.com >= CommandType::_jal && EXMEM.com <= CommandType::_la))
					IFID.EXdata = EXMEM.address;
				else
					IFID.EXdata = EXMEM.ALUout;
			}
			else {
				switch(EXMEM.com){
				case CommandType::_beq:
				case CommandType::_bne:
				case CommandType::_bge:
				case CommandType::_ble:
				case CommandType::_bgt:
				case CommandType::_blt:
				case CommandType::_beqz:
				case CommandType::_bnez:
				case CommandType::_bgez:
				case CommandType::_blez:
				case CommandType::_bgtz:
				case CommandType::_bltz:
					if (IFID.npc.ui - 1 != EXMEM.address.ui) {
						IFID.init();
						pc = EXMEM.address;
					}
					break;
				default:
					break;
				}
			}
		}

		if (ID(IFID, IDEX)) {
			switch (IDEX.com) {
			case CommandType::_b:
			case CommandType::_j:
			case CommandType::_jal:
			case CommandType::_jr:
			case CommandType::_jalr:
				pc = IDEX.imm;
				break;
			case CommandType::_beq:
			case CommandType::_bne:
			case CommandType::_bge:
			case CommandType::_ble:
			case CommandType::_bgt:
			case CommandType::_blt:
			case CommandType::_beqz:
			case CommandType::_bnez:
			case CommandType::_bgez:
			case CommandType::_blez:
			case CommandType::_bgtz:
			case CommandType::_bltz:
				if(IDEX.imm < pc)
					pc = IDEX.imm;
				break;
			default:
				break;
			}
		}
		
		IF(IFID, mem_access);
	}
	return pc == 0;
}
