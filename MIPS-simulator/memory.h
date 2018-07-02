#ifndef __Memory
#define __Memory

#include "Format.h"
using namespace std;

const int memorySize = 1024 * 1024 * 16;

class Memory {
	int used;
	Byte data[memorySize];
public:
	bool write(const int &len, Byte _data[]) {
	}
	bool algin(const int &n) {
	}
	Byte getByte(const Word &address) const {
	}
	Half getHalf(const Word &address) const {
	}
	Word getWord(const Word &address) const {
	}
	Memory() :used(0) {}
}memory;

#endif // !__Memory

