#pragma warning(disable:4996)
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "MispParser.h"
#include "MipsSimulator.h"
using namespace std;

int main() {
	freopen("code.in", "r", stdin);
	//freopen("code.out", "w", stdout);
	__initialization();
	ifstream sourceFile("code.s");
	cout << sourceFile.is_open() << endl;
	stringstream buffer;
	buffer << sourceFile.rdbuf();
	MipsParser *Parser = new MipsParser(buffer.str());
	Memory *mem = new Memory(memorySize);
	Parser->parser(*mem);

	return 0;
}