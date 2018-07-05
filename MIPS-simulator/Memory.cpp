#include "Memory.h"


Word Memory::getSpace(const int &len, const Byte _data[]) {
	if (used + len > size)
		throw memory_used_up();
	memcpy(data + used, _data, len);
	used += len;
	return used - len;
}

Word Memory::getSpace(const int &len, const char _data[]) {
	if (used + len > size)
		throw memory_used_up();
	memcpy(data + used, _data, len);
	used += len;
	return used - len;
}

Word Memory::getSpace(const int &len) {
	if (used + len > size)
		throw memory_used_up();
	used += len;
	return used - len;
}

Word Memory::getSpace(const Byte &_data) {
	if (used + 1 > size)
		throw memory_used_up();
	data[used++] = _data;
	return used - 1;
}

Word Memory::getSpace(const Half &_data) {
	if (used + 2 > size)
		throw memory_used_up();
	data[used++] = _data.b0;
	data[used++] = _data.b1;
	return used - 2;
}

Word Memory::getSpace(const Word &_data) {
	if (used + 4 > size)
		throw memory_used_up();
	data[used++] = _data.b0;
	data[used++] = _data.b1;
	data[used++] = _data.b2;
	data[used++] = _data.b3;
	return used - 4;
}

void Memory::writeByte(const Word &address, const Byte &_data) {
	data[address.i] = _data.i;
}

void Memory::writeHalf(const Word &address, const Half &_data) {
	data[address.i] = _data.b0;
	data[address.i + 1] = _data.b1;
}

void Memory::writeWord(const Word &address, const Word &_data) {
	data[address.i] = _data.b0;
	data[address.i + 1] = _data.b1;
	data[address.i + 2] = _data.b2;
	data[address.i + 3] = _data.b3;
}

void Memory::writeString(const Word &address, const int &len, const string &str) {
	if (len < (int)str.length() + 1)
		throw write_out_of_bound();
	memcpy(data + address.i, str.c_str(), str.length() + 1);
}

void Memory::algin(const int &n) {
	if (((used >> n) << n) != used)
		used = ((used >> n) + 1) << n;
	if (used > size)
		throw memory_used_up();
}
Byte Memory::getByte(const Word &address) const {
	return data[address.ui];
}

Half Memory::getHalf(const Word &address) const {
	Half tmp;
	tmp.b0 = data[address.ui];
	tmp.b1 = data[address.ui + 1];
	return std::move(tmp);
}

Word Memory::getWord(const Word &address) const {
	Word tmp;
	tmp.b0 = data[address.ui];
	tmp.b1 = data[address.ui + 1];
	tmp.b2 = data[address.ui + 2];
	tmp.b3 = data[address.ui + 3];
	return std::move(tmp);
}

string Memory::getString(const Word &address) const {
	string s = "";
	int pos = address.ui;
	while (data[pos].i != 0) {
		s += data[pos++].i;
	}
	return s;
}

int Memory::top() const {
	return size - 1;
}

void Memory::out() const {
	for (int i = 0; i < used; i++) {
		std::cout << (unsigned int)data[i] << " \n"[i % 10 == 0];
	}
}

Memory::Memory(int _size) :used(0), size(_size) {
	data = new Byte[_size];
	memset(data, 0, size);
}

Memory::~Memory() {
	delete[] data;
}
