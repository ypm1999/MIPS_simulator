#ifndef __Formats
#define __Formats

//#define DEBUG

union Byte {
	char i;
	unsigned char ui;
	Byte(char _t = 0) :i(_t) {}
	operator char(){
		return i;
	}
};

union Half{
	short i;
	unsigned short ui;
	struct {char b0, b1; };
	Half(short _t = 0) :i(_t) {}
	operator short() {
		return i;
	}
};

union Word {
	int i;
	unsigned int ui;
	struct { char b0, b1, b2, b3; };
	Word(int _t = 0) :i(_t) {}
	operator int() {
		return i;
	}
};

enum class DataType:char {
	_ascii, _asciiz, _byte, _half, _word, _space, _align, none
};

enum class CommandType:char  {
	_add, _addu, _addiu, _sub, _subu,
	_mul, _mulu, _div, _divu,
	_xor, _xoru, _neg, _negu, _rem, _remu,
	_seq, _sge, _sgt, _sle, _slt, _sne,
	_b, _beq, _bne, _bge, _ble, _bgt, _blt,
	_beqz, _bnez, _bgez, _blez, _bgtz, _bltz,
	_j, _jr, _jal, _jalr,
	_li, _la, _lb, _lh, _lw,
	_sb, _sh, _sw,
	_move, _mfhi, _mflo,
	_nop, _syscall,
	none
};

class Data {
	friend class MipsParser;
	friend class MipsSimulator;
	DataType _type;
	Word address;
public:
	Data(DataType d = DataType::none, Word ad = -1)
		:_type(d), address(ad){}
	~Data() = default;
#ifdef DEBUG
#include <cstdio>
	void out() {
		printf("data: %d %d\n", _type, address.i);
	}
#endif
};

class Command {
	friend class MipsParser;
	friend class MipsSimulator;
	CommandType _type;
	Byte rs, rd, rt;
	Word cons, address, offset;
public:
	Command(CommandType c = CommandType::none, Byte r1 = 255, Byte r2 = 255, Byte r3 = 255, Word con = 0, Word ad = -1, Word of = 0)
		:_type(c), rs(r1), rd(r2), rt(r3), cons(con), address(ad), offset(of) {}
	~Command() = default;
#ifdef DEBUG
	void out() {
		printf("command: %d %d %d %d %d %d %d\n", _type, rs.i, rd.i, rt.i, cons.i, address.i, offset.i);
		//	printf("command: %d %d %d\n", _type, address.i, offset.i);
	}
#endif // DEBUG
};

#endif // !__Formats
