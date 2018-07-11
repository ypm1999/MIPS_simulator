#include "MipsSimulator.h"
int cnt = 0;

 bool MipsSimulator::IF_ID::empty(){
	return res1.b0 == (unsigned char)CommandType::none;
}

 void MipsSimulator::IF_ID::init() {
	res1.b0 = (unsigned char)CommandType::none;
	npc = 0;
}

 bool MipsSimulator::ID_EX::empty(){
	return com == CommandType::none;
}

 void MipsSimulator::ID_EX::init() {
	com = CommandType::none;
	res.ui = 255u;
}

 bool MipsSimulator::EX_MEM::empty(){
	return com == CommandType::none;
}

 void MipsSimulator::EX_MEM::init() {
	com = CommandType::none;
	res.ui = 255u;
}

 bool MipsSimulator::MEM_WB::empty(){
	return res.ui == 255u;
}

 void MipsSimulator::MEM_WB::init() {
	com = CommandType::none;
	res.ui = 255u;
}


 bool MipsSimulator::getBranch(unsigned int i) {
	i &= addressBIT;
	return BHT[(i << pridLen) + BH[i]];
}

 void MipsSimulator::changeBranch(unsigned int i, bool sta) {
	i &= addressBIT;
	BHT[(i << pridLen) + BH[i]] = sta;
	BH[i] = ((BH[i] << 1) | (int)sta) & pridBIT;
}


bool MipsSimulator::getReg(const unsigned char id, Word &res){
	
	unique_lock<mutex> lck1(MEMout);
	unique_lock<mutex> lck2(EXout);
	if (load.ui == id)
		return false;
	if (EXreg.ui == id) {
		res = EXdata;
		return true;
	}
	if (MEMreg.ui == id) {
		res = MEMdata;
		return true;
	}
	if (!regLock[id]) {
		res = reg[id];
		return true;
	}
	return false;
}

void MipsSimulator::IF(IF_ID &write) {
	if (pc0.ui > codeLimit)
		throw command_address_error(pc0.ui);
#ifdef DEBUG
	if(cnt >= 15267830)
		code->output(pc0.ui);
#endif // DEBUG
	write.res1 = mem->getWord(pc0.ui << 3);
	write.res2 = mem->getWord((pc0.ui << 3) + (commandSize >> 1));
	write.npc = pc0.ui + 1;
}

void MipsSimulator::ID(const IF_ID &get, ID_EX &write) {
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
		if (!getReg(get.res1.b2, write.a))
			return;
		if (get.res1.b3 != 255u) {
			if (!getReg(get.res1.b3, write.b))
				return;
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
			if (!getReg(get.res1.b2, write.a))
				return;
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
		if (!getReg(get.res1.b2, write.a))
			return;
		if (get.res1.b3 != 255u) {
			if (!getReg(get.res1.b3, write.b))
				return;
		}
		else
			write.b = get.res2;
		write.res.ui = get.res1.b1;
		break;
	}
	//rd,cons
	case CommandType::_li:
		write.imm = get.res2;
		write.a.ui = 0;
		write.res.ui = get.res1.b1;
		break;
	// rd, address
	case CommandType::_la:
	case CommandType::_lb:
	case CommandType::_lh:
	case CommandType::_lw:
		if (get.res1.b2 != 255u) {
			if (!getReg(get.res1.b2, write.imm))
				return;
			write.a.ui = get.res2.i;
		}
		else
			write.imm = get.res2, write.a.ui = 0;
		write.res.ui = get.res1.b1;
		break;
	case CommandType::_sb:
	case CommandType::_sh:
	case CommandType::_sw:
		if (!getReg(get.res1.b1, write.b))
			return;
		if (get.res1.b2 != 255u) {
			if (!getReg(get.res1.b2, write.imm))
				return;
			write.a.ui = get.res2.i;
		}
		else
			write.imm = get.res2, write.a.ui = 0;
		break;
	//rd
	case CommandType::_mfhi:
	case CommandType::_mflo:
		write.res.ui = get.res1.b1;
		break;
	case CommandType::_jr:
		if (!getReg(get.res1.b1, write.imm))
			return;
		break;
	case CommandType::_jalr:
		if (!getReg(get.res1.b1, write.imm))
			return;
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
		if (!getReg(get.res1.b1, write.a))
			return;
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
		if (!getReg(get.res1.b1 & 0x3fu, write.a))
			return;
		if (get.res1.b1 & (1u << 7u)) {
			if (!getReg(get.res2.b0, write.b))
				return;
		}
		else
			write.b = get.res2;
		write.imm.ui = ((unsigned int)get.res1.b3 << 8) + (unsigned int)get.res1.b2;
		break;
	}
	case CommandType::_syscall: {
		if (!getReg(2u, write.imm))
			return;
		switch (write.imm.i) {
		case 1:
			if (!getReg(4u, write.a))
				return;
			write.com = CommandType::_syscall1;
			break;
		case 4:
			if (!getReg(4u, write.a))
				return;
			write.com = CommandType::_syscall4;
			break;
		case 5:
			write.res.ui = 2u;
			write.com = CommandType::_syscall5;
			break;
		case 8:
			if (!getReg(4u, write.a))
				return;
			if (!getReg(5u, write.b))
				return;
			write.com = CommandType::_syscall8;
			break;
		case 9:
			if (!getReg(4u, write.a))
				return;
			write.res.ui = 2u;
			write.com = CommandType::_syscall9;
			break;
		case 10:
			withReturnValue = false;
			write.com = CommandType::_syscall10;
			return;
			break;
		case 17:
			if (!getReg(4u, write.a))
				return;
			returnValue = write.a;
			withReturnValue = true;
			write.com = CommandType::_syscall17;
			return;
			break;
		default:
			throw run_command_error(string("syscall with code ") + std::to_string(write.imm.i));
		}
		break;
	}
	case CommandType::_nop:
		break;
	case CommandType::none:
	default:
		throw command_not_found(std::to_string(get.res1.b0));
	}
	write.npc = get.npc;
	if(write.com == CommandType::none)
		write.com = (CommandType)get.res1.b0;
	if(write.res.ui != 255u)
		regLock[write.res.ui]++;
}

void MipsSimulator::EX(const ID_EX &get, EX_MEM  &write) {
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
			hi.ui = (unsigned int)(t >> 32);
			lo.ui = (unsigned int)t;
		}
		break;
	case CommandType::_mulu:
		if (get.res.ui != 255u)
			write.ALUout.ui = get.a.ui * get.b.ui;
		else {
			unsigned long long t = 1llu * get.a.ui * get.b.ui;
			hi.ui = (unsigned int)(t >> 32);
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
		write.address.ui = get.imm.ui + get.a.ui;
		break;
	case CommandType::_sb:
	case CommandType::_sh:
	case CommandType::_sw:
		write.address.ui = get.imm.ui + get.a.ui;
		write.ALUout = get.b;
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
		if (get.a.i == get.b.i)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_bne:
		if (get.a.i != get.b.i)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_bge:
		if (get.a.i >= get.b.i)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_ble:
		if (get.a.i <= get.b.i)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_bgt:
		if (get.a.i > get.b.i)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_blt:
		if (get.a.i < get.b.i)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	//----------------# branch, cmp with zero #---------------
	case CommandType::_beqz:
		if (get.a.i == 0)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_bnez:
		if (get.a.i != 0)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_bgez:
		if (get.a.i >= 0)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_blez:
		if (get.a.i <= 0)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_bgtz:
		if (get.a.i > 0)
			write.address = get.imm;
		else
			write.address = get.npc;
		break;
	case CommandType::_bltz:
		if (get.a.i < 0)
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

	//case CommandType::_nop:
	default:
		break;
	}
}

void MipsSimulator::MEM(const EX_MEM &get, MEM_WB &write) {
	write.com = get.com;
	write.res = get.res;
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
		mem->writeByte(get.address, (Byte)get.ALUout.ui);
		break;
	case CommandType::_sh:
		mem->writeHalf(get.address, (Half)get.ALUout.ui);
		break;
	case CommandType::_sw:
		mem->writeWord(get.address, (Word)get.ALUout.ui);
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
		mem->writeString(get.address, get.ALUout.ui, str + '\0');
		break;
	}
	case CommandType::_syscall9:
		mem->algin(2);
		write.result = mem->getSpace(get.ALUout.ui);
		break;
	default:
		if (get.com >= CommandType::_jal && get.com <= CommandType::_la)
			write.result = get.address;
		else
			write.result = get.ALUout;
	}
}
#include <cassert>
void MipsSimulator::WB(const MEM_WB &get) {
	if (!regLock[get.res.ui])
		throw Error("in WB");
	regLock[get.res.ui]--;
	reg[get.res.ui] = get.result;
}

#ifdef DEBUG
int cnt1 = 0, cnt2 = 0;
#endif // DEBUG



bool MipsSimulator::run(Word Entry, unsigned int len, Memory *_mem) {
	memset(reg, 0, sizeof(reg));
	memset(regLock, 0, sizeof(regLock));
	reg[29].ui = _mem->getSize() - 1;
	BHT.set();
	memset(BH, 0, sizeof(BH));

	pc = Entry;
	codeLimit = len,
	mem = _mem;

	finished = false;

	while (!finished)
		tik_tok();
	cerr << endl<< cnt << endl;
	return pc == 0;
}


void MipsSimulator::thread1() {
	IFID.init();
	if (pc.ui != 0 && IFID.empty())
		IF(IFID), finished = false;
}

void MipsSimulator::thread2() {
	if(!MEMWB1.empty())
		WB(MEMWB1);
	IDEX.init();
	if(!IFID1.empty())
		ID(IFID1, IDEX);
}

void MipsSimulator::thread3() {
	EXMEM.init();
	if (IDEX1.empty())
		return;
	EX(IDEX1, EXMEM);
	if (EXMEM.res.ui != 255u) {
		EXreg = EXMEM.res;
		switch (EXMEM.com) {
		case CommandType::_lb:
		case CommandType::_lh:
		case CommandType::_lw:
		case CommandType::_syscall5:
		case CommandType::_syscall9:
			load = EXMEM.res;
			break;
		case CommandType::_jal:
		case CommandType::_jalr:
		case CommandType::_li:
		case CommandType::_la:
			EXreg = EXMEM.res;
			EXdata = EXMEM.address;
			break;
		default:
			EXreg = EXMEM.res;
			EXdata = EXMEM.ALUout;
			break;
		}
	}
}

void MipsSimulator::thread4() {
	MEMWB.init();
	if (EXMEM1.empty())
		return;
	MEM(EXMEM1, MEMWB);
	MEMreg = MEMWB.res;
	MEMdata = MEMWB.result;
}


//controler
void MipsSimulator::tik_tok() {
	cnt++;
	if (cnt % 1000 == 0)
		cerr << cnt << endl;
	finished = true;
	load = EXreg = MEMreg = 255u;
	pc0 = pc;
	IFID1 = IFID;
	IDEX1 = IDEX;
	EXMEM1 = EXMEM;
	MEMWB1 = MEMWB;
	bool predictionFiled = false, jump = false;

	if (!EXMEM1.empty() && EXMEM1.res != 255u)
		MEMout.lock();
	if (!IDEX1.empty() && IDEX1.res != 255u)
		EXout.lock();

	thread th1(&MipsSimulator::thread1, this);
	thread th2(&MipsSimulator::thread2, this);
	thread th3(&MipsSimulator::thread3, this);
	thread th4(&MipsSimulator::thread4, this);
	
	th4.join();
	if (!EXMEM1.empty() && EXMEM1.res != 255u)
		MEMout.unlock();
	th3.join();
	if (!IDEX1.empty() && IDEX1.res != 255u)
		EXout.unlock();
	/*thread4();
	thread3();
	thread2();
	thread1();*/
	if (EXMEM.com >= CommandType::_beq && EXMEM.com <= CommandType::_bltz) {
		if (IFID1.npc.ui - 1 != EXMEM.address.ui) {
			predictionFiled = true;
			pc = EXMEM.address;
		}
		changeBranch(IDEX.npc.ui - 1, EXMEM.address.ui != IDEX.npc.ui);
	}

	th2.join();
	if (!predictionFiled && (IDEX.com >= CommandType::_b && IDEX.com <= CommandType::_jalr ||
		IDEX.com >= CommandType::_beq && IDEX.com <= CommandType::_bltz && getBranch(IDEX.npc.ui - 1))) {
		pc = IDEX.imm;
		jump = true;
	}

	th1.join();

	if (IDEX.com == CommandType::_syscall10 || IDEX.com == CommandType::_syscall17) {
		IDEX.init();
		IFID.init();
		pc = 0;
		return;
	}
	if (predictionFiled) {
		if (IDEX.res.ui != 255u)
			regLock[IDEX.res.ui]--;
		IDEX.init();
	}
	if (predictionFiled || jump)
		IFID.init();
	else {
		if (IDEX.empty() && !IFID1.empty())
			IFID = IFID1;
		else {

				if (pc != 0)
					pc.ui++;
		}
	}
}
