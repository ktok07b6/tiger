#ifndef SYMBOL_H
#define SYMBOL_H

#include "Object.h"
#include <string>

struct Symbol : public Object
{
Symbol(const std::string &s)
: name(s) {}
	std::string name;
	static Symbol *symbol(const std::string &n);
};

#endif //SYMBOL_H
