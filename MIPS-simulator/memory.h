#ifndef __Memory
#define __Memory


#include <cstring>
#include "Format.h"
#include "Exceptions.h"
using namespace std;

class Memory {
	int used, size;
	Byte *data;
public:
	Word getSpace(const int &len, const Byte _data[]) {
		if (used + len > size)
			throw memory_used_up();
		memcpy(data + used, _data, len);
		used += len;
		return used - len;
	}
	Word getSpace(const int &len, const char _data[]) {
		if (used + len > size)
			throw memory_used_up();
		memcpy(data + used, _data, len);
		used += len;
		return used - len;
	}
	Word getSpace(const int &len) {
		if (used + len > size)
			throw memory_used_up();
		used += len;
		return used - len;
	}
	Word getSpace(const Byte &_data) {
		if (used + 1 > size)
			throw memory_used_up();
		data[used++] = _data;
		return used - 1;
	}
	Word getSpace(const Half &_data) {
		if (used + 2 > size)
			throw memory_used_up();
		data[used++] = _data.b0;
		data[used++] = _data.b1;
		return used - 2;
	}
	Word getSpace(const Word &_data) {
		if (used + 4 > size)
			throw memory_used_up();
		data[used++] = _data.b0;
		data[used++] = _data.b1;
		data[used++] = _data.b2;
		data[used++] = _data.b3;
		return used - 4;
	}
	void writeByte(const Word &address, const Byte &_data) {
		if (address.i + 1 > used)
			throw write_out_of_bound();
		data[address.i] = _data.i;
	}
	void writeHalf(const Word &address, const Half &_data) {
		if (address.i + 2 > used)
			throw write_out_of_bound();
		data[address.i] = _data.b0;
		data[address.i + 1] = _data.b1;
	}
	void writeWord(const Word &address, const Word &_data) {
		if (address.i + 4 > used)
			throw write_out_of_bound();
		data[address.i] = _data.b0;
		data[address.i + 1] = _data.b1;
		data[address.i + 2] = _data.b2;
		data[address.i + 3] = _data.b3;
	}
	void writeString(const Word &address, const int &len, const char _data[]) {
		if (address.i + len > used)
			throw write_out_of_bound();
		memcpy(data + address.i, _data, len);
	}
	void algin(const int &n) {
		if(((used >> n) << n) != used)
			used = ((used >> n) + 1) << n;
		if (used > size)
			throw memory_used_up();
	}
	Byte getByte(const Word &address) const {
		return data[address.ui];
	}
	Half getHalf(const Word &address) const {
		Half tmp;
		tmp.b0 = data[address.ui];
		tmp.b1 = data[address.ui + 1];
		return std::move(tmp);
	}
	Word getWord(const Word &address) const {
		Word tmp;
		tmp.b0 = data[address.ui];
		tmp.b1 = data[address.ui + 1];
		tmp.b2 = data[address.ui + 2];
		tmp.b3 = data[address.ui + 3];
		return std::move(tmp);
	}
	string getString(const Word &adsress) const {

	}
	Memory(int _size) :used(0), size(_size) {
		data = new Byte[_size];
	}
	~Memory() {
		delete[] data;
	}
};

#endif // !__Memory

