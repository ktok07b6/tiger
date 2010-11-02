#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "Object.h"
#include "Temp.h"
#include "Label.h"
#include <string>
#include <vector>

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

	virtual bool isMOVE() {return false;}
	virtual bool isLABEL() {return false;}
	std::string format(TempMap *m);
	std::string toString() {
		return assem;
	}
 protected:
	std::string assem;
};

typedef std::vector<Instruction*> InstructionList;


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

	virtual bool isMOVE() {return true;}

	Temp *getSrc() { return src; }
	Temp *getDst() { return dst; }
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
	virtual bool isLABEL() {return true;}
	Label *label() {return lab;}
 private:
	Label *lab;
};

}//namespace assem

#endif //INSTRUCTION_H
