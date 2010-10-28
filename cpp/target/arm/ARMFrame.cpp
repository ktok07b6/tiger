#include "ARMFrame.h"
#include "HeapManager.h"
#include "Instruction.h"
#include "ARMCodeGen.h"

/*
  ::instruction sufixes::
  al    unconditional execution
  eq    equal
  ne    not equal
  mi    negative value
  pl    positive value or zero
  vs    overflow
  vc    not overflow
  cs/hs >= (unsigned)
  cc/lo <  (unsigned)
  hi    >  (unsigned)
  ls    <= (unsigned)
  ge    >= (signed)
  lt    <  (signed)
  gt    >  (signed)
  le    <= (signed)
 */
/*
  ::registers::
  r0  a1
  r1  a2
  r2  a3
  r3  a4
  r4  v1
  r5  v2
  r6  v3
  r7  v4 wr
  r8  v5
  r9  v6 sb
  r10 v7 sl
  r11 v8 fp
  r12 ip
  r13 sp
  r14 lr
  r15 pc
 */

Frame *
Frame::newFrame(Symbol *name, const std::vector<int> &formals)
{
	return gcnew(ARMFrame, (name, formals));
}

ARMFrame::ARMFrame(Symbol *n, const std::vector<int> &f)
	: frameOffset(0)
{
	this->name = n;
	//Access *access;

	//size_t num = f.size();

	std::vector<int>::const_iterator it;
	it = f.begin();
	while (it != f.end()) {
		formals.push_back(allocLocal((*it)!=0));
		++it;
	}

	char buf[64];
	for (int i = 0; i < MAX_REG; ++i) {
		sprintf(buf, "r%d", i);
		r[i] = gcnew(Temp, (buf));
	}
	framePtr = gcnew(Temp, ("fp"));

	generator = new ARMCodeGen(this);
}

ARMFrame::~ARMFrame()
{
	delete generator;
}

Frame::Access *
ARMFrame::allocLocal(bool escape)
{
	Frame::Access *access = NULL;
	if (escape) {
		DBG("frameOffset %d", frameOffset);
		access = gcnew(InFrame, (frameOffset));
		frameOffset += WORD_SIZE;
	} else {
		Temp *t = gcnew(Temp, ());
		access = gcnew(InReg, (t));
	}
	return access;
}

Temp *
ARMFrame::fp()
{
	return framePtr;
}

int 
ARMFrame::wordSize()
{
	return WORD_SIZE;
}

tree::Exp *
ARMFrame::externalCall(const std::string &func, const tree::ExpList &args)
{
	Label *flabel = gcnew(Label, (func));
	tree::NAME *name = _NAME(flabel);
	tree::CALL *call = _CALL(name, args);
	return call;
}

Temp *
ARMFrame::rv()
{
	return r[0];
}

tree::Stm *
ARMFrame::procEntryExit1(tree::Stm *body)
{
	//TODO:
	//prolog
	//epilog
	Label *fname = gcnew(Label, (name->name));
	tree::LABEL *l = _LABEL(fname);
	tree::SEQMaker sm;
	sm.add(l);
	sm.add(body);
	return sm.make();
}

std::string 
ARMFrame::string(Label *label, const std::string &value)
{
	/* example
    str1:    .ascii      "mov\tr1, r2\n"
             .align 2
    str2:    .asciz      "New Line\n"
             .align 2
	*/ 
	return "";
}

tree::Exp *
ARMFrame::staticChain(tree::Exp *fp)
{
	tree::MEM *mem = _MEM(fp);
	return mem;
}

Label *
ARMFrame::badPtr()
{
	return NULL;
}

Label *
ARMFrame::badSub()
{
	return NULL;
}

assem::InstructionList *
ARMFrame::procEntryExit2(assem::InstructionList *body)
{
	return NULL;
}

Proc *
ARMFrame::procEntryExit3(assem::InstructionList *body)
{
	return NULL;
}

std::string 
ARMFrame::tempMap(Temp *temp)
{
	for (int i = 0; i < MAX_REG; ++i){
		if (temp == r[i]) {
			return r[i]->toString();
		} 
	}
	return "";
}

assem::InstructionList *
ARMFrame::codegen(tree::Stm *stm)
{
	generator->generate(stm, instList);
	return &instList;
}

const Frame::Registers &
ARMFrame::registers()
{
	return regs;
}

//------------------------------------------------------
std::string 
InFrame::toString()
{
	return "";
}

tree::Exp *
InFrame::exp(tree::Exp *fp)
{
	tree::CONST *konst = _CONST(offset);
	tree::BINOP *bop = _BINOP(tree::BINOP::oPLUS, fp, konst);
	tree::MEM *mem = _MEM(bop);
	return mem;
}


//------------------------------------------------------
std::string 
InReg::toString()
{
	return "";
}

tree::Exp *
InReg::exp(tree::Exp *)
{
	return _TEMP(temp);
}

