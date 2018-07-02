#ifndef __Formats
#define __Formats

#include <vector>
#include <string>
using namespace std;

typedef char Byte;

union Half{
	short i;
	unsigned short ui;
	struct { Byte b[2]; };
	Half(short _t) :i(_t) {}
};

union Word {
	int i;
	unsigned int ui;
	struct { Byte b[4]; };
	Word(int _t) :i(_t) {}
};

enum class DataType:char {
	_ascii, _assciz, _byte, _half, _word, _space, none
};

enum class CommandType:char  {
	_add, _addu, _addiu, _sub, _subu, _subiu,
	_mul, _mulu, _div, _divu, 
	_xor, xoru, _neg, _negu, _rem, _remu, 
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
	Word address, length;
public:
	Data(DataType d = DataType::none, Word ad = -1, Word l = 0)
		:_type(d), address(ad), length(l) {}
	~Data() = default;
};

class Command {
	friend class MipsParser;
	friend class MipsSimulator;
	CommandType _type;
	Byte res, rd, rt;
	Word cons, offset, address;
public:
	Command(CommandType c = CommandType::none, Byte r1 = 255, Byte r2 = 255, Byte r3 = 255, Word con = 0, Word ad = -1, Word of = 0)
		:_type(c), res(r1), rd(r2), rt(r3), cons(con), address(ad), offset(of){}
	~Command() = default;
};

class CommandBlock {
	friend class MipsParser;
	friend class MipsSimulator;
	Word address, length;
	std::string name;
public:
	CommandBlock(Word a = -1, Word l = 0, std::string na = "") :address(a), length(l), name(na) {}
	~CommandBlock() = default;
};

class DataBlock {
	friend class MipsParser;
	friend class MipsSimulator;
	Word address, length;
	std::string name;
public:
	DataBlock(Word a = -1, Word l = 0, std::string na = "") :address(a), length(l), name(na) {}
	~DataBlock() = default;
};

#endif // !__Formats

