#include "ARMFrame.h"
#include "HeapManager.h"


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
		sprintf(buf, "reg%d", i);
		r[i] = gcnew(Temp, (buf));
	}
	framePtr = gcnew(Temp, ("fp"));
}

ARMFrame::~ARMFrame()
{
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
	tree::NAME *name = gcnew(tree::NAME, (flabel));
	tree::CALL *call = gcnew(tree::CALL, (name, args));
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
	tree::LABEL *l = gcnew(tree::LABEL, (fname));
	tree::SEQMaker sm;
	sm.add(l);
	sm.add(body);
	return sm.make();
}

std::string 
ARMFrame::string(Label *label, const std::string &value)
{
	return "";
}

tree::Exp *
ARMFrame::staticChain(tree::Exp *fp)
{
	tree::MEM *mem = gcnew(tree::MEM,(fp));
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

InstrList *
ARMFrame::procEntryExit2(InstrList *body)
{
	return NULL;
}

Proc *
ARMFrame::procEntryExit3(InstrList *body)
{
	return NULL;
}

std::string 
ARMFrame::tempMap(Temp *temp)
{
	std::string s;
	char buf[8];
	for (int i = 0; i < MAX_REG; ++i){
		if (temp == r[i]) {
			sprintf(buf, "r%d", i);
			s = buf;
			goto found;
		} 
	}
 found:
	return s;
}

InstrList *
ARMFrame::codegen(tree::Stm *stm)
{
	return NULL;
}

TempList *
ARMFrame::registers()
{
	return NULL;
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
	tree::CONST *konst = gcnew(tree::CONST, (offset));
	tree::BINOP *bop = gcnew(tree::BINOP, (tree::BINOP::oPLUS, fp, konst));
	tree::MEM *mem = gcnew(tree::MEM, (bop));
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
	return gcnew(tree::TEMP, (temp));
}

