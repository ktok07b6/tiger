#ifndef TEMP_H
#define TEMP_H

#include "Object.h"
#include <string>
#include <string.h>
#include <vector>
#include <assert.h>

#include "debug.h"

class Temp : public Object
{
 public:
 Temp() : num(count++), specialName() {}
 Temp(const std::string &specialName) : num(count++), specialName(specialName) 
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
 private:
	int num;
	const std::string specialName;
	static int count;
};

typedef std::vector<Temp*> TempList;

#endif //TEMP_H
