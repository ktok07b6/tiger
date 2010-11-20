#ifndef LABEL_H
#define LABEL_H

#include "Object.h"
#include <string>
#include <vector>

class Label : public Object
{
public:
	Label();
	Label(const std::string &name);
	~Label();
	const std::string &toString() const;

private:
	std::string name;
	static int count;
};

typedef std::vector<Label*> LabelList;

#endif //LABEL_H
