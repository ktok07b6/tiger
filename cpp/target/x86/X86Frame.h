#ifndef X86_FRAME_H
#define X86_FRAME_H

#include "Frame.h"
class X86CodeGen;

class X86Frame : public Frame
{
 public:
	X86Frame(Symbol *name, const std::vector<int> &formals);
	~X86Frame();
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
	virtual Temp *tempMap(Temp *temp);
	virtual assem::InstructionList *codegen(tree::Stm *stm);
	virtual const Registers &registers();
	virtual assem::InstructionList spillTemp(const assem::InstructionList &body, Temp *spill);
	virtual void setUsedRegs(const TempList &regs);

	void argSize(int size);
	enum {
		WORD_SIZE = 4,

		EAX = 0,
		ECX,
		EDX,
		EBX,
		ESI,
		EDI,
		EBP,
		ESP,
		EIP,
		MAX_REG
	};

 private:
	TempList findRegsInBody(const assem::InstructionList &body);

	int frameOffset;
	int maxArgSize;
	X86CodeGen *generator;
	assem::InstructionList instList;
	TempList usedRegs;
	Temp *eax;
	Temp *ecx;
	Temp *edx;
	Temp *ebx;
	Temp *esi;
	Temp *edi;
	Temp *ebp;
	Temp *esp;
	Temp *eip;
	static int frameCount;
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

#endif //X86_FRAME_H
