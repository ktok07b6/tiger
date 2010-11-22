#ifndef ARM_FRAME_H
#define ARM_FRAME_H

#include "Frame.h"
class ARMCodeGen;

class ARMFrame : public Frame
{
 public:
	ARMFrame(Symbol *name, const std::vector<int> &formals);
	~ARMFrame();
	virtual Access *allocLocal(bool escape);
	virtual Temp *fp();
	virtual int wordSize();
	virtual tree::Exp *externalCall(const std::string &func, const tree::ExpList &args);
	virtual Temp *rv();
	virtual tree::Stm *procEntryExit1(tree::Stm *body);
	virtual std::string string(Label *label, const std::string &value);
	virtual tree::Exp *staticChain(tree::Exp *fp);
	virtual assem::InstructionList procEntryExit2(const assem::InstructionList &body);
	virtual assem::InstructionList procEntryExit3(const assem::InstructionList &body);
	virtual std::string tempMap(Temp *temp);
	virtual assem::InstructionList *codegen(tree::Stm *stm);
	virtual const Registers &registers();

	enum {
		WORD_SIZE = 4,
		MAX_REG = 16
	};
 private:
	int frameOffset;
	ARMCodeGen *generator;
	assem::InstructionList instList;
};


class InFrame : public Frame::Access
{
 public:
 InFrame(int offset) : offset(offset) {}

	virtual std::string toString();
	virtual tree::Exp *exp(tree::Exp *fp);
 private:
	int offset;
};

class InReg : public Frame::Access
{
 public:
 InReg(Temp *t) : temp(t) {}

	virtual std::string toString();
	virtual tree::Exp *exp(tree::Exp *fp);
 private:
	Temp *temp;
};

#endif //ARM_FRAME_H
