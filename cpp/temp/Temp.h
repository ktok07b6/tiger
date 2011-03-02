#ifndef TEMP_H
#define TEMP_H

#include "Object.h"
#include <string>
#include <string.h>
#include <vector>
#include <assert.h>

#include "debug.h"

class Temp;
typedef std::vector<Temp*> TempList;

class Temp : public Object
{
 public:
 Temp() 
	 : index(-1), num(count++), specialName() {}
 Temp(const std::string &specialName) 
	 : index(-1), num(count++), specialName(specialName)
	{
	}
 Temp(const Temp &other) 
	 : index(-1), num(other.num), specialName(other.specialName) 
		{
		}
	~Temp() {FUNCLOG;}

	operator const char *() {
		return this->toString().c_str();
	}
	std::string toString() const {
		std::string s;
		if (specialName.empty()) {
			char buf[32];
			sprintf(buf, "t%d", num);
			s = buf;
		} else {
			s = specialName;
		}
		return s;
	}

	static void printTempList(const TempList &tlist);
	int index;//for regalloc
private:
	int num;
	const std::string specialName;
	static int count;
	friend struct LessTemp;
};

struct LessTemp {
    bool operator()(Temp *lhs, Temp *rhs) const {
        return lhs->num < rhs->num;
    }
};
#endif //TEMP_H
