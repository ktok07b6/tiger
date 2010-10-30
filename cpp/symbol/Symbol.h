#ifndef SYMBOL_H
#define SYMBOL_H

#include "Object.h"
#include <string>

struct Symbol : public Object
{
Symbol(const std::string &s)
: name(s) {}
	static Symbol *symbol(const std::string &n);
	operator const char *()
	{
		return name.c_str();
	}
	std::string name;
};

#endif //SYMBOL_H
