#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "Object.h"
#include "Temp.h"
#include "Label.h"
#include <string>
#include <vector>
#include "Vector.h"

class TempMap;

namespace assem {

class Instruction : public Object
{
 public:
	Instruction(const std::string &opcode, 
				const std::string &operands = "",
				const std::string &comment = "",
				int sourceLine = -1);
	virtual ~Instruction() {}

	virtual TempList use() = 0;
	virtual TempList def() = 0;
	virtual LabelList jumps() = 0;

	virtual bool isMOVE() {return false;}
	virtual bool isLABEL() {return false;}
	std::string format(TempMap *m);
	std::string toString() {
		std::string result = opcode + "\t" + operands;
		if (!comment.empty()) {
			result += "\t" + comment;
		}
		return result;
	}
	const std::string &getOpcode() const {
		return opcode;
	}
 protected:
	std::string opcode;
	std::string operands;
	std::string comment;
	int sourceLine;
};

typedef std::vector<Instruction*> InstructionList;


class OPER : public Instruction
{
 public:
	OPER(const std::string &opcode, 
		 const std::string &operands,
		 const TempList &dst, 
		 const TempList &src,
		 const std::string &comment = "",
		 int sourceLine = -1);

	OPER(const std::string &opcode, 
		 const std::string &operands,
		 Temp *dst, 
		 Temp *src,
		 const std::string &comment = "",
		 int sourceLine = -1);

	virtual TempList use();
	virtual TempList def();
	virtual LabelList jumps();
	void setJumpTargets(const LabelList &jmps);
private:
	TempList dst;
	TempList src;
	LabelList targets;
};

class MOVE : public Instruction
{
 public:
	MOVE(const std::string &opcode, 
		 const std::string &operands,
		 Temp *dst, 
		 Temp *src,
		 const std::string &comment = "",
		 int sourceLine = -1);

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
