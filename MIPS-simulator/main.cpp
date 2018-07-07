#pragma warning(disable:4996)
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "MipsParser.h"
#include "MipsSimulator.h"
using std::cerr;
using std::ifstream;
using std::stringstream;
using std::unique_ptr;

int main(int argc, char *argv[]) {
	freopen("code.in", "r", stdin);
#ifdef DEBUG
	
	freopen("code.out", "w", stdout);
#endif
	//string source(argv[1]);
	string source("code.s");
	ifstream sourceFile(source);
	if (!sourceFile.is_open()) {
		cerr << "Source code file do not exit!" << std::endl;
		return 0;
	}
	stringstream buffer;
	buffer << sourceFile.rdbuf();
	unique_ptr<Memory> mem(new Memory);
	unique_ptr<MipsParser> Parser(new MipsParser(buffer.str(), mem.get()));
	unique_ptr<MipsSimulator> program(new MipsSimulator(Parser.get(), mem.get()));
	Parser->parser();
#ifdef DEBUG
	Parser->out();
	mem->out(8);
	freopen("CON", "w", stdout);
#endif // DEBUG
	if (!program->run())
		cerr << "Run time error!" << std::endl;
	else
		if (program->with_returnValue())
			cerr << source << " return Value is " << program->get_returnValue() << std::endl;
		else
			cerr << source << " return without returnValue" << std::endl;
	return 0;
}
