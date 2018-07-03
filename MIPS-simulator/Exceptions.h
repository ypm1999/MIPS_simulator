#ifndef SJTU_EXCEPTIONS_HPP
#define SJTU_EXCEPTIONS_HPP

#include <cstddef>
#include <cstring>
#include <string>

class empty_label {
public:
	empty_label(string name = "Uknown") {
		cerr << "lebal " << name << " is empty" << endl;
	}
};

class command_not_found {
public:
	command_not_found(string name = "Uknown") {
		cerr << "command " << name << " is not found" << endl;
	}
};


#endif
