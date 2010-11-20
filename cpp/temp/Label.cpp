#include "Label.h"
#include "tiger.h"
#include "debug.h"

Label::Label()
	: name(format("L%d", count++))
{
}

Label::Label(const std::string &name)
	: name(name) 
{
}

Label::~Label()
{
}

const std::string & 
Label::toString() const
{
	return name;
}

int
Label::count;
