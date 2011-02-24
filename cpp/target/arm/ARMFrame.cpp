#include "ARMFrame.h"
#include "HeapManager.h"
#include "Instruction.h"
#include "ARMCodeGen.h"
#include "tiger.h"
#include <iterator>
#include <boost/foreach.hpp>

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

std::string
Frame::getFileHeader()
{
	return ".text\n" "\t.align\t2\n" "\t.global\t__tigermain\n";
}

int
ARMFrame::frameCount;

ARMFrame::ARMFrame(Symbol *n, const std::vector<int> &f)
	: frameOffset(0)
	, maxExtraArgSize(0)
{
	Frame::name = n;
	std::string end = format("end_of_%s", name->name.c_str());
	Frame::endFuncLabel = gcnew(Label, (end));
	frameCount++;

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

	generator = new ARMCodeGen(this);

	regs.args.push_back(regs.all[0]);
	regs.args.push_back(regs.all[1]);
	regs.args.push_back(regs.all[2]);
	regs.args.push_back(regs.all[3]);

	//TODO:
	regs.callerSaves.push_back(regs.all[0]);
	regs.callerSaves.push_back(regs.all[1]);
	regs.callerSaves.push_back(regs.all[2]);
	regs.callerSaves.push_back(regs.all[3]);

	regs.calleeSaves.push_back(regs.all[4]);
	regs.calleeSaves.push_back(regs.all[5]);
	regs.calleeSaves.push_back(regs.all[6]);
	regs.calleeSaves.push_back(regs.all[7]);
	regs.calleeSaves.push_back(regs.all[8]);
	regs.calleeSaves.push_back(regs.all[9]);
	regs.calleeSaves.push_back(regs.all[10]);
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
		access = gcnew(InFrame, (-frameOffset));
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

	int i = 0;
	int extraArgOffset = 12;
	std::vector<Access*>::iterator it = formals.begin();
	while (it != formals.end()) {
		if (i < 4) {
			tree::Exp *r = _TEMP(regs.all[i]);
			tree::Exp *tmp = (*it)->exp(_TEMP(fp()));
			sm.add(_MOVE(tmp, r));
		} else {
			tree::CONST *offs = _CONST(extraArgOffset);
			tree::TEMP *base = _TEMP(fp());
			tree::MEM *m = _MEM(_(base) + _(offs));
			tree::Exp *tmp = (*it)->exp(_TEMP(fp()));
			sm.add(_MOVE(tmp, m));
			extraArgOffset += WORD_SIZE;
		}
		++it;
		++i;
	}
	sm.add(body);
	return sm.make();
}

std::string 
ARMFrame::string(Label *label, const std::string &value)
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
ARMFrame::staticChain(tree::Exp *fp)
{
	tree::MEM *mem = _MEM(fp);
	return mem;
}

assem::InstructionList
ARMFrame::procEntryExit2(const assem::InstructionList &body)
{
	assem::InstructionList newbody;

	TempList alive_regs;
	//alive_regs.push_back(rv());
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
ARMFrame::findRegsInBody(const assem::InstructionList &body)
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
ARMFrame::procEntryExit3(const assem::InstructionList &body)
{
	/*
	  the stack frame of arm-tiger
	  
	  ex: frameOffset = 8
	      callee save = {r4, r5, fp, lr}
		  baseOffset = 16 (four callee saves * WORD_SIZE)
          maxExtraArgSize = 2

	       ...  lower address
	           |--------------|
	       -24 |    <empty>   |-+ <== new sp
	           |--------------| | extra parameters for function call
	       -20 |    <empty>   |-+ 
	           |--------------| 
	       -16 |      r5      | | callee saves 2
	           |--------------| |
	       -12 |      r4      |-+ 
	           |--------------| 
	        -8 |      ...     |-+ 
	           |--------------| | local variables & spilled regs (frameOffset)
	        -4 |      ...     |-+ 
	           |--------------| 
	         0 | static link  | <== new fp
	           |--------------|
	        +4 |   (old)lr    |-+
	           |--------------| | callee saves 1
	        +8 |   (old)fp    |-+
	           |--------------|
	       +12 |     arg5     | <== old sp 
	           |--------------|
	       +16 |     arg6     |
	           |--------------|
	       ...  higher address


		   Therefore ...
		   * access to static link    ... fp[0]
		   * access to 1st argument   ... r0
		   * access to 5th argument   ... fp[12]
		   * access to 6th argument   ... fp[16]
		   * access to 1st local var  ... fp[-4]
		   * access to 2nd local var  ... fp[-8]
		   * old fp is saved at       ... fp[4]
		   * store to 1st param of function call ... r0
		   * store to 5th param of function call ... sp[0]
	 */

	assem::InstructionList proc;
	std::string assem;
	std::string strCalleeSaveReg;
	int numCalleeSaveReg = 0;
	//usedRegs is passed by regalloc
	TempList::const_iterator it = usedRegs.begin();
	while (it != usedRegs.end()) {
		Temp *r = *it;
		if (isCalleeSaveReg(r)) {
			if (numCalleeSaveReg) {
				strCalleeSaveReg += ",";
			}
			strCalleeSaveReg += r->toString();
			numCalleeSaveReg++;
		}
		++it;
	}

	//prologue//////////
	assem::Instruction *funcLabel = body.front();
	assert(funcLabel->isLABEL());
	proc.push_back(funcLabel);

	assem::OPER *save_fp = _aOPER("stmfd", "sp!, {fp, lr}", NULL, NULL);
	proc.push_back(save_fp);

	assem::OPER *set_fp = _aOPER("sub", "fp, sp, #4", NULL, NULL);
	proc.push_back(set_fp);

	if (frameOffset) {
		assem = format("sp, sp, #%d", frameOffset);
		assem::OPER *expand_local_temp_space = _aOPER("sub", assem, NULL, NULL);
		proc.push_back(expand_local_temp_space);
	}
	
	if (numCalleeSaveReg) {
		assem = format("sp!, {%s}", strCalleeSaveReg.c_str());
		assem::OPER *store_callee_saves = _aOPER("stmfd", assem, NULL, NULL);
		proc.push_back(store_callee_saves);
	}

	if (maxExtraArgSize) {
		assem = format("sp, sp, #%d", maxExtraArgSize);
		assem::OPER *expand_extra_arg_space = _aOPER("sub", assem, NULL, NULL);
		proc.push_back(expand_extra_arg_space);
	}

	//body//////////////
	std::copy(body.begin()+1, body.end(), std::back_inserter(proc));

	//epilogue//////////
	if (maxExtraArgSize) {
		assem = format("sp, sp, #%d", maxExtraArgSize);
		assem::OPER *rewind_extra_arg_space = _aOPER("sub", assem, NULL, NULL);
		proc.push_back(rewind_extra_arg_space);
	}
	
	if (numCalleeSaveReg) {
		assem = format("sp!, {%s}", strCalleeSaveReg.c_str());
		assem::OPER *restore_callee_saves = _aOPER("ldmfd", assem, NULL, NULL);
		proc.push_back(restore_callee_saves);
	}

	assem::OPER *rewind_sp = _aOPER("add", "sp, fp, #4", NULL, NULL);
	proc.push_back(rewind_sp);

	assem::OPER *restore_and_ret = _aOPER("ldmfd", "sp!, {fp, pc}", NULL, NULL);
	proc.push_back(restore_and_ret);

	return proc;
}

Temp *
ARMFrame::tempMap(Temp *temp)
{
	return temp;
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

assem::InstructionList
ARMFrame::spillTemp(const assem::InstructionList &proc, Temp *spill)
{
	assem::InstructionList result;
	frameOffset += WORD_SIZE;
	BOOST_FOREACH(assem::Instruction *inst, proc) {
		TempList use = inst->use();
		if (std::find(use.begin(), use.end(), spill) != use.end()) {
			Temp *tmp = gcnew(Temp, ());
			std::string operand = format("'d0, [fp, #%d]", -frameOffset);
			assem::OPER *ldr = _aOPER("ldr", operand, tmp, NULL);
			result.push_back(ldr);
			inst->replaceUse(spill, tmp);
		}

		result.push_back(inst);

		TempList def = inst->def();
		if (std::find(def.begin(), def.end(), spill) != def.end()) {
			Temp *tmp = gcnew(Temp, ());
			std::string operand = format("'s0, [fp, #%d]", -frameOffset);
			assem::OPER *str = _aOPER("str", operand, NULL, tmp);
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
ARMFrame::setUsedRegs(const TempList &regs)
{
	usedRegs = regs;
}

void 
ARMFrame::extraArgSize(int size)
{
	maxExtraArgSize = (maxExtraArgSize < size) ? size : maxExtraArgSize;
}


bool 
ARMFrame::isCalleeSaveReg(Temp *r)
{
	TempList::iterator it = regs.calleeSaves.begin();
	while (it != regs.calleeSaves.end()) {
		if (r == *it) {
			return true;
		}
		++it;
	}
	return false;
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

