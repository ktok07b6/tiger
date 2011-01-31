#include "Temp.h"

int
Temp::count;

void 
Temp::printTempList(const TempList &li)
{
	DBG("print temp list %d", li.size());
	std::string result;
	TempList::const_iterator it = li.begin();
	while (it != li.end()) {
		const Temp *t = *it;
		assert(t);
		result += t->toString();
		result += " ";
		++it;
	}
	DBG("%s", result.c_str());
}

