#include "ARMFrame.h"
#include "HeapManager.h"
#include "Instruction.h"
#include "ARMCodeGen.h"
#include "tiger.h"
#include <iterator>

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

	regs.all.push_back(gcnew(Temp, ("r0")));
	regs.all.push_back(gcnew(Temp, ("r1")));
	regs.all.push_back(gcnew(Temp, ("r2")));
	regs.all.push_back(gcnew(Temp, ("r3")));
	regs.all.push_back(gcnew(Temp, ("r4")));
	regs.all.push_back(gcnew(Temp, ("r5")));
	regs.all.push_back(gcnew(Temp, ("r6")));
	regs.all.push_back(gcnew(Temp, ("r7")));
	regs.all.push_back(gcnew(Temp, ("r8")));
	regs.all.push_back(gcnew(Temp, ("r9")));
	regs.all.push_back(gcnew(Temp, ("r10")));
	regs.all.push_back(gcnew(Temp, ("fp")));
	regs.all.push_back(gcnew(Temp, ("r12")));
	regs.all.push_back(gcnew(Temp, ("sp")));
	regs.all.push_back(gcnew(Temp, ("lr")));
	regs.all.push_back(gcnew(Temp, ("pc")));

	//TODO: if(このフレームがstatic linkを必要とするなら)
	//frameOffset += WORD_SIZE;//static linkのためにfp[0]を空ける

	int i = 0;
	std::vector<int>::const_iterator it;
	it = f.begin();
	while (it != f.end()) {
		int escape = (*it);
		Access *acc;
		if (escape) {
			acc = allocLocal(true);
		} else {
			acc = gcnew(InReg, (regs.all[i]));
		}
		formals.push_back(acc);
		++i;
		++it;
	}

	generator = new ARMCodeGen(this);

	regs.args.push_back(regs.all[0]);
	regs.args.push_back(regs.all[1]);
	regs.args.push_back(regs.all[2]);
	regs.args.push_back(regs.all[3]);

	//TODO:
	regs.calleeSaves.push_back(regs.all[4]);
	regs.calleeSaves.push_back(regs.all[5]);
	regs.calleeSaves.push_back(regs.all[6]);
	regs.calleeSaves.push_back(regs.all[7]);
	regs.calleeSaves.push_back(regs.all[11]);
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
	return regs.all[11];
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
	return regs.all[0];
}

tree::Stm *
ARMFrame::procEntryExit1(tree::Stm *body)
{
	Label *fname = gcnew(Label, (name->name));
	tree::LABEL *l = _LABEL(fname);
	tree::SEQMaker sm;
	sm.add(l);

	if (formals.size() < 4) {
		int i = 0;
		std::vector<Access*>::iterator it = formals.begin();
		while (it != formals.end()) {
			tree::Exp *r = _TEMP(regs.all[i]);
			tree::Exp *tmp = (*it)->exp(_TEMP(fp()));
			sm.add(_MOVE(tmp, r));
			++it;
			++i;
		}
	} else {
		//TODO:
		assert(0);
	}
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

assem::InstructionList
ARMFrame::procEntryExit2(const assem::InstructionList &body)
{
	TempList alive_regs;
	alive_regs.push_back(rv());
	std::copy(regs.calleeSaves.begin(), regs.calleeSaves.end(), std::back_inserter(alive_regs));
	assem::OPER *sink1 = gcnew(assem::OPER, ("", alive_regs, TempList()));
	assem::OPER *sink2 = gcnew(assem::OPER, ("", TempList(), alive_regs));

	assem::InstructionList newbody;

	newbody.push_back(sink1);
	std::copy(body.begin(), body.end(), std::back_inserter(newbody));

	//insert end label
	assert(endLabel);
	std::string assem = format("%s:", endLabel->toString().c_str());
	assem::LABEL *end_lab = gcnew(assem::LABEL, (assem, endLabel));
	newbody.push_back(end_lab);

	newbody.push_back(sink2);

	return newbody;
}

assem::InstructionList
ARMFrame::procEntryExit3(const assem::InstructionList &body)
{
	/*
	  stack frame example

	       ...  lower address
	  XXXXFFFC |--------------|
	  XXXX0000 |    <empty>   |
	  XXXX0004 |--------------|
	  XXXX0008 |      ...     |-+ <== sp
	  XXXX000C |--------------| |
	  XXXX0010 |      ...     | | local vars & args
	  XXXX0014 |--------------| |
	  XXXX0018 | static link  |-+ <== fp
	  XXXX001C |--------------|
	  XXXX0020 |      lr      |-+
	  XXXX0024 |--------------| | 
	  XXXX0028 |      fp      | | callee save regs
	  XXXX002C |--------------| |
	  XXXX0030 |      ...     |-+
	  XXXX0034 |--------------|
	  XXXX0038 |      ...     | <== old sp
	  XXXX003C |--------------|
	       ...  higher address
	 */

	assem::InstructionList proc;
	std::string assem;
	std::string calleeSaveStr;//TODO:
	TempList::const_iterator it = regs.calleeSaves.begin();
	while (it != regs.calleeSaves.end()) {
		Temp *r = *it;
		calleeSaveStr += r->toString();
		calleeSaveStr += ",";
		++it;
	}
	//calleeSaveStr += "fp,";

	//prologue//////////
	assem = format("stmfd sp!, {%s lr}", calleeSaveStr.c_str());
	assem::OPER *stmfd = gcnew(assem::OPER, (assem, TempList(), TempList()));
	proc.push_back(stmfd);
	assem = "sub fp, sp, #4";
	assem::OPER *set_fp = gcnew(assem::OPER, (assem, TempList(), TempList()));
	proc.push_back(set_fp);

	assem = format("sub sp, sp, #%d", frameOffset);
	assem::OPER *set_sp = gcnew(assem::OPER, (assem, TempList(), TempList()));
	proc.push_back(set_sp);

	//body//////////////
	std::copy(body.begin(), body.end(), std::back_inserter(proc));

	//epilogue//////////
	assem = "add sp, fp, #4";
	assem::OPER *rewind_sp = gcnew(assem::OPER, (assem, TempList(), TempList()));
	proc.push_back(rewind_sp);
	
	assem = format("ldmfd sp!, {%s pc}", calleeSaveStr.c_str());
	assem::OPER *ldmfd = gcnew(assem::OPER, (assem, TempList(), TempList()));
	proc.push_back(ldmfd);

	return proc;
}

std::string 
ARMFrame::tempMap(Temp *temp)
{
	for (int i = 0; i < MAX_REG; ++i){
		if (temp == regs.all[i]) {
			return regs.all[i]->toString();
		} 
	}
	return temp->toString();
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
	return format("m[%d]", offset);
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
	return temp->toString();
}

tree::Exp *
InReg::exp(tree::Exp *)
{
	return _TEMP(temp);
}

