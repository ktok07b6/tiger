#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "Object.h"
#include "Temp.h"
#include "Label.h"
#include <string>
#include <list>

class TempMap;

namespace assem {

class Instruction : public Object
{
 public:
	Instruction(const std::string &assem);
	virtual ~Instruction() {}

	virtual TempList use() = 0;
	virtual TempList def() = 0;
	virtual LabelList jumps() = 0;

	virtual bool isMove() {return false;}
	std::string format(TempMap *m);
	std::string toString() {
		return assem;
	}
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
	virtual LabelList jumps();
private:
	TempList dst;
	TempList src;
	LabelList targets;
};

class MOVE : public Instruction
{
 public:
	MOVE(const std::string &assem, Temp *dst, Temp *src);

	virtual TempList use();
	virtual TempList def();
	virtual LabelList jumps();

	virtual bool isMove() {return true;}
 private:
	Temp *dst;
	Temp *src;
};

class LABEL : public Instruction
{
 public:
	LABEL(const std::string &assem, Label *l);

	virtual TempList use();
	virtual TempList def();
	virtual LabelList jumps();
};

}//namespace assem

#endif //INSTRUCTION_H
