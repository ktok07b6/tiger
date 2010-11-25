#include "ARMCodeGen.h"
#include "Instruction.h"
#include "TreeMatcher.h"
#include "tiger.h"
#include "Frame.h"

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

using namespace std;

assem::CodeGen *
assem::CodeGen::create()
{
	return new ARMCodeGen(NULL);
}


ARMCodeGen::ARMCodeGen(Frame *frame)
	: frame(frame)
{
}


void
ARMCodeGen::munchMOVE(tree::Exp *dst, tree::Exp *src)
{
	tree::MEM *mem;
	tree::BINOP *binop;
	tree::CONST *konst;
	tree::TEMP *temp;

	if (_M1(MEM_T, mem, _M0(BINOP_T, binop)) == dst) {
		if (binop->op == tree::BINOP::oPLUS) {
			if (_M0(CONST_T, konst) == binop->r) {
				//(MEM(BINOP(PLUS,e1,CONST(i))), e2)
				std::string assem;
				if (konst->value < 4096) {
					assem = format("$s1, [$s0, #+%d]", konst->value);
				} else {
					//TODO:
					assert(0);
				}
				TempList tl;
				tl.push_back(munchExp(binop->l));
				tl.push_back(munchExp(src));
				emit(gcnew(assem::OPER, ("str", assem, TempList(), tl)));
				return;
			} else if (_M0(CONST_T, konst) == binop->l) {
				//MEM(BINOP(PLUS,CONST(i),e1)),e2))
				std::string assem;
				if (konst->value < 4096) {
					assem = format("$s1, [$s0, #+%d]", konst->value);
				} else {
					//TODO:
					assert(0);
				}
				TempList tl;
				tl.push_back(munchExp(binop->r));
				tl.push_back(munchExp(src));
				emit(gcnew(assem::OPER, ("str", assem, TempList(), tl)));
				return;
			}
		} 
	}
	//(MEM(e1),e2)
	if (_M0(MEM_T, mem) == dst) {
		TempList tsrc;
		tsrc.push_back(munchExp(mem->exp));
		tsrc.push_back(munchExp(src));
		emit(gcnew(assem::OPER, ("str", "$s1, [$s0]", TempList(), tsrc)));
		return;
	}
	/*
	//(TEMP(i), BINOP(...))
	if (_M0(TEMP_T, temp) == dst && _M0(BINOP_T, binop) == src) {
		if (binop->op == tree::BINOP::oPLUS) {
			if (_M0(TEMP_T, temp2) == binop->l && _M0(CONST_T, konst) == binop->r) {
				if (temp == temp2) {
					string assem = format("add $d0, $s0, #%d", konst->value);
					TempList tdst, tsrc;
					tdst.push_back(temp->temp);
					tsrc.push_back(temp->temp);
					emit(gcnew(assem::OPER, (assem, tdst, tsrc)));
					return;
				}
			} else if (_M0(TEMP_T, temp2) == binop->r && _M0(CONST_T, konst) == binop->l) {
				if (temp == temp2) {
					string assem = format("add $d0, $s0, #%d", konst->value);
					TempList tdst, tsrc;
					tdst.push_back(temp->temp);
					tsrc.push_back(temp->temp);
					emit(gcnew(assem::OPER, (assem, tdst, tsrc)));
					return;
				}
			}

		} else if (binop->op == tree::BINOP::oMINUS) {
		} else if (binop->op == tree::BINOP::oMUL) {
		} else if (binop->op == tree::BINOP::oDIV) {
		}
	}
	*/
	//(TEMP(i), CONST(e2))
	if (_M0(TEMP_T, temp) == dst && _M0(CONST_T, konst) == src) {
		std::string assem = format("$d0, #%d", konst->value);
		emit(gcnew(assem::MOVE, ("mov", assem, temp->temp, NULL)));
		return;
	}

	//(TEMP(i), e2)
	if (_M0(TEMP_T, temp) == dst) {
		Temp *ts = munchExp(src);
		emit(gcnew(assem::MOVE, ("mov", "$d0, $s0", temp->temp, ts)));
		return;
	}
}

void
ARMCodeGen::munchLABEL(Label *lab)
{
	emit(gcnew(assem::LABEL, (lab->toString() + ":", lab)));
}

void 
ARMCodeGen::munchJUMP(Label *lab)
{
	LabelList targets;
	targets.push_back(lab);
	assem::OPER *op = gcnew(assem::OPER, ("b", "$j0", NULL, NULL));
	if (lab->toString() == END_FUNCTION_LABEL_NAME) {
		
	}
	op->setJumpTargets(targets);
	emit(op);
}

void 
ARMCodeGen::munchCJUMP(tree::CJUMP *cj)
{
	tree::CONST *konst;
	std::string assem;
	TempList tsrc;

	if (_M0(CONST_T, konst) == cj->l) {
		assem = format("$s0, #%d", konst->value);
		tsrc.push_back(munchExp(cj->r));
	} else if (_M0(CONST_T, konst) == cj->r) {
		assem = format("$s0, #%d", konst->value);
		tsrc.push_back(munchExp(cj->l));
	} else {
		assem = "$s0, $s1";
		tsrc.push_back(munchExp(cj->l));
		tsrc.push_back(munchExp(cj->r));
	}
	emit(gcnew(assem::OPER, ("cmp", assem, TempList(), tsrc)));

	const char *cond[] = {"eq", "ne", 
						  "lt", "gt", 
						  "le", "ge",
						  "lo", "hi",
						  "ls", "hs"};
	LabelList targets;
	targets.push_back(cj->truelab);
	targets.push_back(cj->falselab);
	assem = format("b%s", cond[cj->relop]);
	assem::OPER *op = gcnew(assem::OPER, (assem, "$j0", NULL, NULL));
	op->setJumpTargets(targets);
	emit(op);
}

void
ARMCodeGen::munchEXPR(tree::Exp *exp)
{
	tree::CALL *call;
	if (_M0(CALL_T, call) == exp) {
		TempList tsrc;
		munchArgs(call->args, &tsrc);

		LabelList targets;
		targets.push_back(call->func->label);
		assem::OPER *op = gcnew(assem::OPER, ("bl", "$j0", TempList(), tsrc));
		op->setJumpTargets(targets);
		emit(op);
	}
}

void
ARMCodeGen::munchArgs(const tree::ExpList &exps, TempList *tsrc)
{
	const Frame::Registers &regs = frame->registers();
	tree::ExpList::const_iterator exp;
	tree::CONST *konst;
	if (regs.args.size() < exps.size()) {
		exp = exps.begin();
		std::advance(exp, exps.size() - regs.args.size());
		int offset = 0;
		while (exp != exps.end()) {
			tree::Exp *e = *exp;
			Temp *tmp = gcnew(Temp, ());
			if (_M0(CONST_T, konst) == e) {
				std::string assem = format("$d1, #%d", konst->value);
				emit(gcnew(assem::MOVE, ("mov", assem, tmp, NULL)));
				//FIXME:
				assem = format("$s1, [sp, #%d]", offset);
				emit(gcnew(assem::OPER, ("str", assem, regs.all[13], tmp)));
			} else {
				Temp *src = munchExp(e);
				emit(gcnew(assem::MOVE, ("mov", "$d1, $s1", tmp, src)));
				std::string assem = format("$s1, [sp, #%d]", offset);
				emit(gcnew(assem::OPER, ("str", assem, regs.all[13], tmp)));
			}
			++exp;
			offset += frame->wordSize();
		}
	}

	TempList::const_iterator arg_reg;
	exp = exps.begin();
	arg_reg = regs.args.begin();
	while (arg_reg != regs.args.end()) {
		tree::Exp *e = *exp;
		Temp *dst = *arg_reg;
		if (_M0(CONST_T, konst) == e) {
			std::string arg_reg_str = frame->tempMap(dst);
			std::string assem = format("%s, #%d", arg_reg_str.c_str(), konst->value);
			emit(gcnew(assem::MOVE, ("mov", assem, dst, NULL)));
		} else {
			Temp *src = munchExp(e);
			std::string arg_reg_str = frame->tempMap(dst);
			std::string assem = format("%s, $s0", arg_reg_str.c_str());
			emit(gcnew(assem::MOVE, ("mov", assem, dst, src)));
		}
		tsrc->push_back(dst);
		++exp;
		++arg_reg;
	}
}


Temp *
ARMCodeGen::munchMEM(tree::MEM *mem)
{
	tree::BINOP *binop;
	tree::CONST *konst;

	if (_M0(BINOP_T, binop) == mem->exp) {
		if (_M0(CONST_T, konst) == binop->r) {
			//(BINOP(PLUS,e1,CONST(i)))
			std::string assem;
			if (konst->value < 4096) {
				assem = format("$d0, [$s0, #+%d]", konst->value);
			} else {
				//TODO:
				assert(0);
			}
			Temp *r = gcnew(Temp, ());
			emit(gcnew(assem::OPER, ("ldr", 
									 assem, 
									 r, 
									 munchExp(binop->l))));
			return r;
		}
		if (_M0(CONST_T, konst) == binop->l) {
			//(BINOP(PLUS,CONST(i),e1))
			std::string assem;
			if (konst->value < 4096) {
				assem = format("$d0, [$s0, #+%d]", konst->value);
			} else {
				//TODO:
				assert(0);
			}
			Temp *r = gcnew(Temp, ());
			emit(gcnew(assem::OPER, ("ldr", 
									 assem, 
									 r, 
									 munchExp(binop->r))));
			return r;
		}
	}

	{
		//(e1)
		Temp *r = gcnew(Temp, ());
		emit(gcnew(assem::OPER, ("ldr", 
								 "$d0, [$s0]", 
								 r, 
								 munchExp(mem->exp))));
		return r;
	}
}

Temp *
ARMCodeGen::munchBINOP(tree::BINOP *binop)
{
	tree::CONST *konst;
	const char *opcode;
	switch (binop->op) {
	case tree::BINOP::oPLUS:
		opcode = "add";
		break;	
	case tree::BINOP::oMINUS:
		opcode = "sub";
		break;
	case tree::BINOP::oMUL:
		opcode = "mul";
		break;
	case tree::BINOP::oDIV:
		//TODO:
		opcode = "div???";
		break;
	default:
		//TODO:
		opcode = "???";
		break;
	}
	if (_M0(CONST_T, konst) == binop->r) {
		//(op,e1,CONST(i))
		std::string assem = format("$d0, $s0, #%d", konst->value);

		Temp *r = gcnew(Temp, ());
		emit(gcnew(assem::OPER, (opcode, assem, r, munchExp(binop->l))));
		return r;
	}
	if (_M0(CONST_T, konst) == binop->l) {
		//(op,CONST(i),e1)
		std::string assem = format("$d0, $s0, #%d", konst->value);

		Temp *r = gcnew(Temp, ());
		emit(gcnew(assem::OPER, (opcode, assem, r, munchExp(binop->r))));
		return r;
	}
		
	{
		//(op,e1,e2)
		Temp *r = gcnew(Temp, ());
		TempList tdst, tsrc;
		tdst.push_back(r);
		tsrc.push_back(munchExp(binop->l));
		tsrc.push_back(munchExp(binop->r));
		emit(gcnew(assem::OPER, (opcode, "$d0, $s0, $s1", tdst, tsrc)));
		return r;
	}
}

Temp *
ARMCodeGen::munchCALL(tree::CALL *c)
{
	TempList tsrc;
	TempList tdst;
	munchArgs(c->args, &tsrc);

	assert(frame);
	Temp *rv = frame->rv();
	tdst.push_back(rv);

	LabelList targets;
	targets.push_back(c->func->label);
	assem::OPER *op = gcnew(assem::OPER, ("bl", "$j0", tdst, tsrc));
	op->setJumpTargets(targets);
	emit(op);
	return rv;
}

Temp *
ARMCodeGen::munchCONST(tree::CONST *c)
{
	//(i)
	std::string assem = format("$d0, #%d", c->value);

	Temp *r = gcnew(Temp, ());
	emit(gcnew(assem::OPER, ("mov", assem, r, NULL)));
	return r;
}

Temp *
ARMCodeGen::munchTEMP(tree::TEMP *t)
{
	return t->temp;
}

Temp *
ARMCodeGen::munchNAME(tree::NAME *n)
{
	//TODO
	return gcnew(Temp, ());
}

bool 
ARMCodeGen::isMove(assem::Instruction *inst)
{
	return inst->isMOVE();
	/*
	std::string assem = inst->toString();
	return strncmp("mov", assem.c_str(), 3) == 0;
	*/
}

bool 
ARMCodeGen::isJump(assem::Instruction *inst)
{
	return inst->getOpcode() == "b";
}

