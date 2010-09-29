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
 Temp() : num(count++), specialName(NULL) {}
 Temp(const char *specialName) : num(count++), specialName(specialName) {
		assert(strlen(specialName) < 64);
	}

	~Temp() {FUNCLOG;}
	std::string toString() const {
		char buf[64];
		if (specialName) {
			sprintf(buf, "%s", specialName);
		} else {
			sprintf(buf, "t%d", num);
		}
		return buf;
	}
 private:
	int num;
	const char *specialName;
	static int count;
};

typedef std::vector<Temp*> TempList;

#endif //TEMP_H
