#include "X86Frame.h"
#include "HeapManager.h"
#include "Instruction.h"
#include "X86CodeGen.h"
#include "tiger.h"
#include <iterator>
#include <boost/foreach.hpp>

Frame *
Frame::newFrame(Symbol *name, const std::vector<int> &formals)
{
	return gcnew(X86Frame, (name, formals));
}

int
X86Frame::frameCount;

X86Frame::X86Frame(Symbol *n, const std::vector<int> &f)
	: frameOffset(0)
	, maxExtraArgSize(0)
{
	Frame::name = n;
	std::string end = format("end_of_%s", name->name.c_str());
	Frame::endFuncLabel = gcnew(Label, (end));
	frameCount++;

	eax = gcnew(Temp, ("%eax"));
	ecx = gcnew(Temp, ("%ecx"));
	edx = gcnew(Temp, ("%edx"));
	ebx = gcnew(Temp, ("%ebx"));
	esi = gcnew(Temp, ("%esi"));
	edi = gcnew(Temp, ("%edi"));
	ebp = gcnew(Temp, ("%ebp"));
	esp = gcnew(Temp, ("%esp"));
	eip = gcnew(Temp, ("%eip"));
	regs.all.push_back(eax);
	regs.all.push_back(ecx);
	regs.all.push_back(edx);
	regs.all.push_back(ebx);
	regs.all.push_back(esi);
	regs.all.push_back(edi);
	regs.all.push_back(ebp);
	regs.all.push_back(esp);
	regs.all.push_back(eip);

	int i = 0;
	std::vector<int>::const_iterator it;
	it = f.begin();
	while (it != f.end()) {
		int escape = (*it);
		Access *acc;
		acc = allocLocal(escape);
		formals.push_back(acc);
		++i;
		++it;
	}

	generator = new X86CodeGen(this);

	regs.callerSaves.push_back(eax);
	regs.callerSaves.push_back(ecx);
	regs.callerSaves.push_back(edx);

	regs.calleeSaves.push_back(ebx);
	regs.calleeSaves.push_back(esi);
	regs.calleeSaves.push_back(edi);
	regs.calleeSaves.push_back(ebp);
	
	regs.specials.push_back(esp);
	regs.specials.push_back(eip);
}

X86Frame::~X86Frame()
{
	delete generator;
}

Frame::Access *
X86Frame::allocLocal(bool escape)
{
	Frame::Access *access = NULL;
	if (escape) {
		DBG("frameOffset %d", frameOffset);
		access = gcnew(InFrame, (-frameOffset));
		frameOffset += WORD_SIZE;
	} else {
		Temp *t = gcnew(Temp, ());
		access = gcnew(InReg, (t));
	}
	return access;
}

Temp *
X86Frame::fp()
{
	return ebp;
}

int 
X86Frame::wordSize()
{
	return WORD_SIZE;
}

tree::Exp *
X86Frame::externalCall(const std::string &func, const tree::ExpList &args)
{
	Label *flabel = gcnew(Label, (func));
	tree::NAME *name = _NAME(flabel);
	tree::CALL *call = _CALL(name, args);
	return call;
}

Temp *
X86Frame::rv()
{
	return eax;
}

tree::Stm *
X86Frame::procEntryExit1(tree::Stm *body)
{
	Label *fname = gcnew(Label, (name->name));
	tree::LABEL *l = _LABEL(fname);
	tree::SEQMaker sm;
	sm.add(l);

	int i = 0;
	int offset = 4;
	std::vector<Access*>::iterator it = formals.begin();
	while (it != formals.end()) {
		if (i < 4) {
			tree::Exp *r = _TEMP(regs.all[i]);
			tree::Exp *tmp = (*it)->exp(_TEMP(fp()));
			sm.add(_MOVE(tmp, r));
		} else {
			tree::CONST *offs = _CONST(offset);
			tree::TEMP *base = _TEMP(fp());
			tree::MEM *m = _MEM(_(base) + _(offs));
			tree::Exp *tmp = (*it)->exp(_TEMP(fp()));
			sm.add(_MOVE(tmp, m));
			offset += WORD_SIZE;
		}
		++it;
		++i;
	}
	sm.add(body);
	return sm.make();
}

std::string 
X86Frame::string(Label *label, const std::string &value)
{
	std::string assem;
	const char *lab = label->toString().c_str();
	assem = format("%s:\t.long\t%s_body\n%s_body:\t.asciz\t\"%s\"", lab, lab, lab, value.c_str());
	assem += "\n\t.align 2";
	return assem;
	/* example
    str1:    .ascii      "mov\tr1, r2\n"
             .align 2
    str2:    .asciz      "New Line\n"
             .align 2
	*/ 
}

tree::Exp *
X86Frame::staticChain(tree::Exp *fp)
{
	tree::MEM *mem = _MEM(fp);
	return mem;
}

assem::InstructionList
X86Frame::procEntryExit2(const assem::InstructionList &body)
{
	assem::InstructionList newbody;

	TempList alive_regs;
	TempList used = findRegsInBody(body);
	std::copy(used.begin(), used.end(), std::back_inserter(alive_regs));
	std::sort(alive_regs.begin(), alive_regs.end(), LessTemp());
	alive_regs.erase(std::unique(alive_regs.begin(), alive_regs.end()), alive_regs.end());

	std::string comment;
#if 1
	BOOST_FOREACH(Temp *r, alive_regs) {
		comment += r->toString();
		comment += " ";
	}
#endif		
	assem::OPER *sink1 = gcnew(assem::OPER, ("", "", alive_regs, TempList(), "@def " + comment));
	assem::OPER *sink2 = gcnew(assem::OPER, ("", "", TempList(), alive_regs, "@use " + comment));
	
	assem::Instruction *funcLabel = body.front();
	assert(funcLabel->isLABEL());
	newbody.push_back(funcLabel);
	
	newbody.push_back(sink1);
	std::copy(body.begin()+1, body.end(), std::back_inserter(newbody));
	
	//insert end label
	assert(endFuncLabel);
	std::string assem = format("%s:", endFuncLabel->toString().c_str());
	assem::LABEL *end_lab = gcnew(assem::LABEL, (assem, endFuncLabel));
	newbody.push_back(end_lab);

	newbody.push_back(sink2);

	return newbody;
}

TempList
X86Frame::findRegsInBody(const assem::InstructionList &body)
{
	TempList used;
	BOOST_FOREACH(assem::Instruction *inst, body) {
		TempList def = inst->def();
		BOOST_FOREACH(Temp *t, def) {
			for (int i = 0; i < MAX_REG; ++i) {
				if (t == regs.all[i]) {
					used.push_back(t);
				}
			}
		}
		TempList use = inst->use();
		BOOST_FOREACH(Temp *t, use) {
			for (int i = 0; i < MAX_REG; ++i) {
				if (t == regs.all[i]) {
					used.push_back(t);
				}
			}
		}
	}
	std::sort(used.begin(), used.end(), LessTemp());
	used.erase(std::unique(used.begin(), used.end()), used.end());
	return used;
}

assem::InstructionList
X86Frame::procEntryExit3(const assem::InstructionList &body)
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

	std::string saveRegStr;
	//usedRegs is passed by regalloc
	bool fpExist = false;
	TempList::const_iterator it = usedRegs.begin();
	while (it != usedRegs.end()) {
		Temp *r = *it;
		if (r == rv()) {
			++it;
			continue;
		} else if (r == fp()) {
			fpExist = true;
		}
		saveRegStr += r->toString();
		saveRegStr += ",";
		++it;
	}
	//fp is always used by frame
	if (!fpExist) {
		saveRegStr += "ebp,";
	}

	//prologue//////////
	assem::Instruction *funcLabel = body.front();
	assert(funcLabel->isLABEL());
	proc.push_back(funcLabel);
	
	assem = format("%ebp");
	assem::OPER *save_bp = gcnew(assem::OPER, ("pushl", "%ebp", NULL, ebp));
	proc.push_back(save_bp);

	assem::MOVE *move_sp = gcnew(assem::MOVE, ("movl", "%esp, %ebp", ebp, esp));
	proc.push_back(move_sp);

	assem = format("$%d, %%esp", frameOffset + maxExtraArgSize);
	assem::OPER *expand_sp = gcnew(assem::OPER, ("subl", assem, esp, esp));
	proc.push_back(expand_sp);

	//body//////////////
	std::copy(body.begin()+1, body.end(), std::back_inserter(proc));

	//epilogue//////////
	//leave
	assem::MOVE *rewind_sp = gcnew(assem::MOVE, ("movl", "%ebp, %esp", esp, ebp));
	proc.push_back(rewind_sp);

	assem::OPER *load_bp = gcnew(assem::OPER, ("popl", "%ebp", ebp, NULL));
	proc.push_back(load_bp);
	
	assem::OPER *ret = gcnew(assem::OPER, ("ret", "", NULL, NULL));
	proc.push_back(ret);

	return proc;
}

Temp *
X86Frame::tempMap(Temp *temp)
{
	return temp;
}

assem::InstructionList *
X86Frame::codegen(tree::Stm *stm)
{
	generator->generate(stm, instList);
	return &instList;
}

const Frame::Registers &
X86Frame::registers()
{
	return regs;
}

assem::InstructionList
X86Frame::spillTemp(const assem::InstructionList &proc, Temp *spill)
{
	assem::InstructionList result;
	frameOffset += WORD_SIZE;
	BOOST_FOREACH(assem::Instruction *inst, proc) {
		TempList use = inst->use();
		if (std::find(use.begin(), use.end(), spill) != use.end()) {
			Temp *tmp = gcnew(Temp, ());
			std::string operand = format("$d0, %d(%ebp)", -frameOffset);
			assem::OPER *ldr = gcnew(assem::OPER, ("movl", operand, tmp, NULL));
			result.push_back(ldr);
			inst->replaceUse(spill, tmp);
		}

		result.push_back(inst);

		TempList def = inst->def();
		if (std::find(def.begin(), def.end(), spill) != def.end()) {
			Temp *tmp = gcnew(Temp, ());
			std::string operand = format("%d(%ebp), $s0", -frameOffset);
			assem::OPER *str = gcnew(assem::OPER, ("movl", operand, NULL, tmp));
			result.push_back(str);
			inst->replaceDef(spill, tmp);
		}
	}
#if 0
	BOOST_FOREACH(assem::Instruction *inst, result) {
		DBG("%s", inst->format(this).c_str());
	}
#endif
	return result;
}

void 
X86Frame::setUsedRegs(const TempList &regs)
{
	usedRegs = regs;
}

void 
X86Frame::extraArgSize(int size)
{
	maxExtraArgSize = (maxExtraArgSize < size) ? size : maxExtraArgSize;
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

