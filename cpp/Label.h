#ifndef LABEL_H
#define LABEL_H

#include "Object.h"
#include <string>
#include <vector>
#include "debug.h"

class Label : public Object
{
 public:
	Label()
		:name(){
		char buf[16];
		sprintf(buf, "L%d", count++);
		name = buf;
	}
 Label(const std::string &name)
		: name(name) {
	}

	~Label() {FUNCLOG;}
	const std::string &toString() const;

 private:
	std::string name;
	static int count;
};

inline const std::string & 
Label::toString() const
{
	return name;
}

typedef std::vector<Label*> LabelList;

#endif //LABEL_H
