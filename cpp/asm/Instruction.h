#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "Object.h"
#include "Temp.h"
#include "Label.h"
#include <string>
#include <list>

class TempMap;

namespace assem {

class Targets
{
 public:
 Targets(const LabelList &l) : li(l) 
	{
	}
	LabelList li;
};

 class Instruction : public Object
{
 public:
	Instruction();
	virtual ~Instruction() {}

	virtual TempList use() = 0;
	virtual TempList def() = 0;
	virtual Targets jumps() = 0;
	std::string format(TempMap *m);
 protected:
	std::string assem;
};

typedef std::list<Instruction*> InstructionList;


class OPER : public Instruction
{
 public:
	OPER(const std::string &assem, const TempList &dst, const TempList &src);
	OPER(const std::string &assem, const TempList &dst, const TempList &src, const LabelList &jmps);

	virtual TempList use();
	virtual TempList def();
	virtual Targets jumps();
};

class MOVE : public Instruction
{
 public:
	MOVE(const std::string &assem, Temp *dst, Temp *src);

	virtual TempList use();
	virtual TempList def();
	virtual Targets jumps();
};

class LABEL : public Instruction
{
 public:
	LABEL(const std::string &assem, Label *l);

	virtual TempList use();
	virtual TempList def();
	virtual Targets jumps();
};

}//namespace assem

#endif //INSTRUCTION_H
