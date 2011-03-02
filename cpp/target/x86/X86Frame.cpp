#include "X86Frame.h"
#include "HeapManager.h"
#include "Instruction.h"
#include "X86CodeGen.h"
#include "tiger.h"
#include <iterator>
#include <boost/foreach.hpp>
#include "TreeMatcher.h"
#include "TreeUtils.h"

Frame *
Frame::newFrame(Symbol *name, const std::vector<int> &formals)
{
	return gcnew(X86Frame, (name, formals));
}

std::string
Frame::getFileHeader()
{
	return ".text\n" "\t.globl\t__tigermain\n";
}

int
X86Frame::frameCount;

X86Frame::X86Frame(Symbol *n, const std::vector<int> &f)
	: frameOffset(0)
	, maxArgSize(0)
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

	std::vector<int>::const_iterator it;
	it = f.begin();
	int argOffset = 8;//0 ... ebp, 4 ... return addr
	while (it != f.end()) {
		Access *acc = gcnew(InFrame, (argOffset));
		formals.push_back(acc);
		argOffset += WORD_SIZE;
		++it;
	}

	generator = new X86CodeGen(this);

	regs.callerSaves.push_back(eax);
	regs.callerSaves.push_back(ecx);
	regs.callerSaves.push_back(edx);

	regs.calleeSaves.push_back(ebx);
	regs.calleeSaves.push_back(esi);
	regs.calleeSaves.push_back(edi);
	
	regs.specials.push_back(ebp);
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
		frameOffset += WORD_SIZE;
		DBG("frameOffset %d", frameOffset);
		access = gcnew(InFrame, (-frameOffset));
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
	if (!tree::isExistType(body, tree::Tree::CALL_T)) {
		//this is leaf function
		//TODO:
	}
	Label *fname = gcnew(Label, (name->name));
	tree::LABEL *l = _LABEL(fname);
	tree::SEQMaker sm;
	sm.add(l);
	/*
	int i = 0;
	int offset = 4;
	std::vector<Access*>::iterator it = formals.begin();
	while (it != formals.end()) {
		tree::CONST *offs = _CONST(offset);
		tree::TEMP *base = _TEMP(fp());
		tree::MEM *m = _MEM(_(base) + _(offs));
		tree::Exp *tmp = (*it)->exp(_TEMP(fp()));
		sm.add(_MOVE(tmp, m));
		offset += WORD_SIZE;
		
		++it;
		++i;
	}
	*/
	sm.add(body);
	return sm.make();
}

std::string 
X86Frame::string(Label *label, const std::string &value)
{
	std::string assem;
	const char *lab = label->toString().c_str();
	assem = format("%s:\t.long\t%s_body\n%s_body:\t.asciz\t\"%s\"", lab, lab, lab, value.c_str());
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
	tree::CONST *offset = _CONST(8);
	tree::BINOP *binop = _(fp) + _(offset);
	tree::MEM *mem = _MEM(binop);
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
	assem::OPER *sink1 = _aOPER5("", "", alive_regs, TempList(), "@def " + comment);
	assem::OPER *sink2 = _aOPER5("", "", TempList(), alive_regs, "@use " + comment);
	
	assem::Instruction *funcLabel = body.front();
	assert(funcLabel->isLABEL());
	newbody.push_back(funcLabel);
	
	newbody.push_back(sink1);
	std::copy(body.begin()+1, body.end(), std::back_inserter(newbody));
	
	//insert end label
	assert(endFuncLabel);
	std::string assem = format("%s:", endFuncLabel->toString().c_str());
	assem::LABEL *end_lab = _aLABEL(assem, endFuncLabel);
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
	  the stack frame of x86-tiger
	  
	  ex: frameOffset = 8
	      saveCalleeSave={ebx, edi}
          maxArgSize = 2

	       ...  lower address
	           |--------------|
	       -24 |    <empty>   |-+ <== esp
	           |--------------| |
	       -20 |    <empty>   |-+ parameters of function call (maxArgSize*WORD_SIZE)
	           |--------------| | 
	       -16 |      edi     | |
	           |--------------| |
	       -12 |      ebx     |-+ callee saves (saveCalleeSave.size()*WORD_SIZE)
	           |--------------| |
	        -8 |      ...     | |
	           |--------------| |
	        -4 |      ...     |-+ local variables & spilled regs (frameOffset)
	           |--------------| |
	         0 | previous ebp |-+ <== ebp (old esp top)
	           |--------------|
	        +4 | return addr  | // push by "call" instruction
	           |--------------| 
	        +8 | static link  | // arg 0 is static link. this is tiger's calling convention
	           |--------------|
	       +12 |     arg 1    | // pushed by caller
	           |--------------|
	       +16 |     arg 2    | // pushed by caller
	           |--------------|
	       ...  higher address


		   Therefore ...
		   * access to static link    ... 8(%ebp)
		   * access to 1st arggument  ... 12(%ebp)
		   * access to 1st local var  ... -4(%ebp)
		   * access to 2nd local var  ... -8(%ebp)
		   * access to 1st param of function call ... 0(%esp)
		   * access to 2nd param of function call ... 4(%esp)
	 */
	assem::InstructionList proc;
	std::string assem;

	//prologue//////////
	assem::Instruction *funcLabel = body.front();
	assert(funcLabel->isLABEL());
	proc.push_back(funcLabel);
	
	assem::OPER *save_bp = _aOPER("pushl", "%ebp", NULL, ebp);
	proc.push_back(save_bp);

	assem::MOVE *move_sp = _aMOVE("movl", "%esp, %ebp", ebp, esp);
	proc.push_back(move_sp);

	//usedRegs is passed by regalloc
	TempList savedCalleeSave;
	{
		TempList::const_iterator it = usedRegs.begin();
		while (it != usedRegs.end()) {
			Temp *r = *it;
			if (isCalleeSaveReg(r)) {
				savedCalleeSave.push_back(r);
			}
			++it;
		}
	}

	int stackFrameSize = frameOffset + savedCalleeSave.size()*WORD_SIZE + maxArgSize*WORD_SIZE;
	assem = format("$%d, %%esp", stackFrameSize);
	assem::OPER *expand_sp = _aOPER("subl", assem, esp, esp);
	proc.push_back(expand_sp);

	//CalleeSave registers is stored between the local variables and the parameters of the function call
	int offset = frameOffset;
	{
		TempList::const_iterator it = savedCalleeSave.begin();
		while (it != savedCalleeSave.end()) {
			offset += 4;
			assem = format("%s, %d(%%ebp)", (*it)->toString().c_str(), -offset);
			assem::OPER *store_callee_save = _aOPER("movl", 
													assem,
													NULL, 
													(*it));
			proc.push_back(store_callee_save);
			++it;
		}
	}

	//body//////////////
	std::copy(body.begin()+1, body.end(), std::back_inserter(proc));

	//epilogue//////////

	assem = format("$%d, %%esp", stackFrameSize);
	assem::OPER *unexpand_sp = _aOPER("addl", assem, esp, esp);
	proc.push_back(unexpand_sp);

	//restore callee saves
	{
		TempList::const_reverse_iterator rit = savedCalleeSave.rbegin();
		TempList::const_reverse_iterator rend = savedCalleeSave.rend();
		while (rit != rend) {
			assem = format("%d(%%ebp), %s", -offset, (*rit)->toString().c_str());
			assem::OPER *restore_callee_save = _aOPER("movl", 
													  assem,
													  (*rit), 
													  NULL
													  );
			proc.push_back(restore_callee_save);
			offset -= 4;
			++rit;
		}
	}


	//leave
	assem::MOVE *rewind_sp = _aMOVE("movl", "%ebp, %esp", esp, ebp);
	proc.push_back(rewind_sp);

	assem::OPER *load_bp = _aOPER("popl", "%ebp", ebp, NULL);
	proc.push_back(load_bp);
	
	assem::OPER *ret = _aOPER("ret", "", NULL, NULL);
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
			std::string operand = format("%d(%%ebp), 'd0\t# loaded spill ", -frameOffset);
			assem::OPER *ldr = _aOPER("movl", operand, tmp, NULL);
			result.push_back(ldr);
			inst->replaceUse(spill, tmp);
		}

		result.push_back(inst);

		TempList def = inst->def();
		if (std::find(def.begin(), def.end(), spill) != def.end()) {
			Temp *tmp = gcnew(Temp, ());
			std::string operand = format("'s0, %d(%%ebp)\t# stored spill", -frameOffset);
			assem::OPER *str = _aOPER("movl", operand, NULL, tmp);
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
X86Frame::argSize(int size)
{
	maxArgSize = (maxArgSize < size) ? size : maxArgSize;
}

bool 
X86Frame::isCalleeSaveReg(Temp *r)
{
	return (r == ebx || r == esi || r == edi);
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
	tree::BINOP *bop = _(fp) + _(konst);
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

