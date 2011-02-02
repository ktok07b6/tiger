#include "ARMCodeGen.h"
#include "Instruction.h"
#include "TreeMatcher.h"
#include "tiger.h"
#include "ARMFrame.h"
#include "Optimizer.h"

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

#define IMMEDIATE_MAX (255)

assem::CodeGen *
assem::CodeGen::create()
{
	return new ARMCodeGen(NULL);
}


ARMCodeGen::ARMCodeGen(ARMFrame *frame)
	: frame(frame)
{
}


void
ARMCodeGen::munchMOVE(tree::Exp *dst, tree::Exp *src)
{
	tree::MEM *mem;
	tree::BINOP *binop;
	tree::CONST *konst, *konst2;
	tree::TEMP *temp;

	if (_M1(MEM_T, mem, _M0(BINOP_T, binop)) == dst) {
		//the offset of the memory is always calculated by add process
		assert(binop->op == tree::BINOP::oPLUS);

		if (_M0(CONST_T, konst) == binop->r && 
			_M0(CONST_T, konst2) != binop->l) {
			//(MEM(BINOP(PLUS,e1,CONST(i))), e2)
			std::string assem;
			if (konst->value <= IMMEDIATE_MAX) {
				assem = format("$s1, [$s0, #%d]", konst->value);
				TempList tl;
				tl.push_back(munchExp(binop->l));
				tl.push_back(munchExp(src));
				emit(gcnew(assem::OPER, ("str", assem, TempList(), tl)));
					return;
			}
		} else if (_M0(CONST_T, konst) == binop->l &&
				   _M0(CONST_T, konst2) != binop->r) {
			//MEM(BINOP(PLUS,CONST(i),e1)),e2))
			std::string assem;
			if (konst->value <= IMMEDIATE_MAX) {
				assem = format("$s1, [$s0, #%d]", konst->value);
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
		std::string assem = format("$d0, =%d", konst->value);
		emit(gcnew(assem::MOVE, ("ldr", assem, temp->temp, NULL)));
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
	op->setJumpTargets(targets);
	emit(op);
}

void 
ARMCodeGen::munchCJUMP(tree::CJUMP *cj)
{
	tree::CONST *konst;
	std::string assem;
	TempList tsrc;

	const char *cond[] = {"eq", "ne", 
						  "lt", "gt", 
						  "le", "ge",
						  "lo", "hi",
						  "ls", "hs"};
	//reverse condition for less or greater
	const char *cond_r[] = {"eq", "ne", 
							"ge", "le", 
							"gt", "lt",
							"hs", "ls",
							"hi", "lo"};
	bool reverse = false;
	if (_M0(CONST_T, konst) == cj->l && konst->value <= IMMEDIATE_MAX) {
		assem = format("$s0, #%d", konst->value);
		tsrc.push_back(munchExp(cj->r));
		reverse = true;
	} else if (_M0(CONST_T, konst) == cj->r && konst->value <= IMMEDIATE_MAX) {
		assem = format("$s0, #%d", konst->value);
		tsrc.push_back(munchExp(cj->l));
	} else {
		assem = "$s0, $s1";
		tsrc.push_back(munchExp(cj->l));
		tsrc.push_back(munchExp(cj->r));
	}
	emit(gcnew(assem::OPER, ("cmp", assem, TempList(), tsrc)));
	LabelList targets;
	targets.push_back(cj->truelab);
	targets.push_back(cj->falselab);
	assem = format("b%s", reverse ? cond_r[cj->relop] : cond[cj->relop]);
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
		//TODO: r0-r3 will be destoryed in c function
		assem::OPER *op = gcnew(assem::OPER, ("bl", "$j0", frame->registers().args, tsrc));
		op->setJumpTargets(targets);
		emit(op);
	}
}

void
ARMCodeGen::munchArgs(const tree::ExpList &exps, TempList *tsrc)
{
	const Frame::Registers &regs = frame->registers();
	tree::ExpList::const_iterator exp;
	tree::ExpList::const_iterator exp_end = exps.end();
	tree::CONST *konst;
	if (regs.args.size() < exps.size()) {
		exp = exps.begin();
		std::advance(exp, regs.args.size());
		exp_end = exp;
		int offset = 0;
		while (exp != exps.end()) {
			tree::Exp *e = *exp;
			Temp *tmp = gcnew(Temp, ());
			if (_M0(CONST_T, konst) == e) {
				std::string assem = format("$d0, =%d", konst->value);
				emit(gcnew(assem::MOVE, ("ldr", assem, tmp, NULL)));
				//FIXME:
				assem = format("$s0, [sp, #%d]", offset);
				emit(gcnew(assem::OPER, ("str", assem, regs.all[13], tmp)));
			} else {
				Temp *src = munchExp(e);
				emit(gcnew(assem::MOVE, ("mov", "$d0, $s0", tmp, src)));
				std::string assem = format("$s0, [sp, #%d]", offset);
				emit(gcnew(assem::OPER, ("str", assem, regs.all[13], tmp)));
			}
			++exp;
			offset += frame->wordSize();
		}
		frame->extraArgSize(offset);
	}

	TempList::const_iterator arg_reg;
	exp = exps.begin();
	arg_reg = regs.args.begin();
	
	while (exp != exp_end) {
		tree::Exp *e = *exp;
		Temp *dst = *arg_reg;
		if (_M0(CONST_T, konst) == e) {
			std::string assem = format("$d0, =%d", konst->value);
			emit(gcnew(assem::MOVE, ("ldr", assem, dst, NULL)));
		} else {
			Temp *src = munchExp(e);
			emit(gcnew(assem::MOVE, ("mov", "$d0, $s0", dst, src)));
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
	tree::CONST *konst, *konst2;

	if (_M0(BINOP_T, binop) == mem->exp) {
		//the offset of the memory is always calculated by add process
		assert(binop->op == tree::BINOP::oPLUS);

		if (_M0(CONST_T, konst) == binop->r &&
			_M0(CONST_T, konst2) != binop->l) {
			//(BINOP(PLUS,e1,CONST(i)))
			std::string assem;
			if (konst->value <= IMMEDIATE_MAX) {
				assem = format("$d0, [$s0, #%d]", konst->value);
				Temp *r = gcnew(Temp, ());
				emit(gcnew(assem::OPER, ("ldr", 
										 assem, 
										 r, 
										 munchExp(binop->l))));
				return r;
			}
		}
		if (_M0(CONST_T, konst) == binop->l &&
			_M0(CONST_T, konst2) != binop->r) {
			//(BINOP(PLUS,CONST(i),e1))
			std::string assem;
			if (konst->value <= IMMEDIATE_MAX) {
				assem = format("$d0, [$s0, #%d]", konst->value);
				Temp *r = gcnew(Temp, ());
				emit(gcnew(assem::OPER, ("ldr", 
										 assem, 
										 r, 
										 munchExp(binop->r))));
				return r;
			}
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
	switch (binop->op) {
	case tree::BINOP::oPLUS:  return munchBINOP_PLUS(binop);
	case tree::BINOP::oMINUS: return munchBINOP_MINUS(binop);
	case tree::BINOP::oMUL:   return munchBINOP_MUL(binop);
	case tree::BINOP::oDIV:   return munchBINOP_DIV(binop);
	default:
		assert(0);
		return NULL;
	}
}

Temp *
ARMCodeGen::munchBINOP_PLUS(tree::BINOP *binop)
{
	tree::CONST *konst, *konst2;

	//(op,CONST(i1),CONST(i2))
	if (_M0(CONST_T, konst) == binop->l &&
		_M0(CONST_T, konst2) == binop->r &&
		opt::getOptimizationOption(opt::CONSTANT_FOLDING) == opt::OPT_ENABLE ) {
		int value;
		value = konst->value + konst2->value;

		Temp *r = gcnew(Temp, ());
		std::string assem = format("$d0, =%d", value);
		emit(gcnew(assem::MOVE, ("ldr", assem, r, NULL)));
		return r;
	}
	//(op,e1,CONST(i))
	else if (_M0(CONST_T, konst) == binop->r) {
		if (konst->value == 0) {
			//"e1 + 0"
			return munchExp(binop->l);
		}

		if (konst->value <= IMMEDIATE_MAX) {
			std::string assem = format("$d0, $s0, #%d", konst->value);
			Temp *r = gcnew(Temp, ());
			emit(gcnew(assem::OPER, ("add", assem, r, munchExp(binop->l))));
			return r;
		} 
	}
	//(op,CONST(i),e1)
	else if (_M0(CONST_T, konst) == binop->l) {
		//add process has commutative law
		if (konst->value == 0) {
			//"0 + e1"
			return munchExp(binop->r);
		}
		else if (konst->value <= IMMEDIATE_MAX) {
			std::string assem = format("$d0, $s0, #%d", konst->value);

			Temp *r = gcnew(Temp, ());
			emit(gcnew(assem::OPER, ("add", assem, r, munchExp(binop->r))));
			return r;
		}
	}
		
	{
		//(op,e1,e2)
		Temp *r = gcnew(Temp, ());
		TempList tdst, tsrc;
		tdst.push_back(r);
		tsrc.push_back(munchExp(binop->l));
		tsrc.push_back(munchExp(binop->r));
		emit(gcnew(assem::OPER, ("add", "$d0, $s0, $s1", tdst, tsrc)));
		return r;
	}
}

Temp *
ARMCodeGen::munchBINOP_MINUS(tree::BINOP *binop)
{
	tree::CONST *konst, *konst2;

	//(op,CONST(i1),CONST(i2))
	if (_M0(CONST_T, konst) == binop->l &&
		_M0(CONST_T, konst2) == binop->r &&
		opt::getOptimizationOption(opt::CONSTANT_FOLDING) == opt::OPT_ENABLE ) {
		int value;
		value = konst->value - konst2->value;

		Temp *r = gcnew(Temp, ());
		std::string assem = format("$d0, =%d", value);
		emit(gcnew(assem::MOVE, ("ldr", assem, r, NULL)));
		return r;
	}
	//(op,e1,CONST(i))
	else if (_M0(CONST_T, konst) == binop->r) {
		if (konst->value == 0) {
			//"e1 - 0"
			return munchExp(binop->l);
		}

		if (konst->value <= IMMEDIATE_MAX) {
			std::string assem = format("$d0, $s0, #%d", konst->value);
			Temp *r = gcnew(Temp, ());
			emit(gcnew(assem::OPER, ("sub", assem, r, munchExp(binop->l))));
			return r;
		} 
	}
	//(op,CONST(i),e1)
	else if (_M0(CONST_T, konst) == binop->l) {
		//can nothing
	}
		
	{
		//(op,e1,e2)
		Temp *r = gcnew(Temp, ());
		TempList tdst, tsrc;
		tdst.push_back(r);
		tsrc.push_back(munchExp(binop->l));
		tsrc.push_back(munchExp(binop->r));
		emit(gcnew(assem::OPER, ("sub", "$d0, $s0, $s1", tdst, tsrc)));
		return r;
	}
}

Temp *
ARMCodeGen::munchBINOP_MUL(tree::BINOP *binop)
{
	tree::CONST *konst, *konst2;
	//(op,CONST(i1),CONST(i2))
	if (_M0(CONST_T, konst) == binop->l &&
		_M0(CONST_T, konst2) == binop->r &&
		opt::getOptimizationOption(opt::CONSTANT_FOLDING) == opt::OPT_ENABLE ) {
		int value;
		value = konst->value * konst2->value;

		Temp *r = gcnew(Temp, ());
		std::string assem = format("$d0, =%d", value);
		emit(gcnew(assem::MOVE, ("ldr", assem, r, NULL)));
		return r;
	}
	//(op,e1,CONST(i))
	else if (_M0(CONST_T, konst) == binop->r) {
		if (konst->value == 1) {
			//"e1 * 1"
			return munchExp(binop->l);
		}

		Temp *rs = gcnew(Temp, ());
		std::string assem = format("$d0, =%d", konst->value);
		emit(gcnew(assem::MOVE, ("ldr", assem, rs, NULL)));
			
		Temp *rm = munchExp(binop->l);
		TempList tdst, tsrc;
		tdst.push_back(rs);//Rd
		tsrc.push_back(rm);
		tsrc.push_back(rs);
		emit(gcnew(assem::OPER, ("mul", "$d0, $s0, $s1", tdst, tsrc)));
		return rs;
	}
	//(op,CONST(i),e1)
	else if (_M0(CONST_T, konst) == binop->l) {
		if (konst->value == 1) {
			//"1 * e1"
			return munchExp(binop->r);
		}

		Temp *rs = gcnew(Temp, ());
		std::string assem = format("$d0, =%d", konst->value);
		emit(gcnew(assem::MOVE, ("ldr", assem, rs, NULL)));
		
		Temp *rm = munchExp(binop->r);
		TempList tdst, tsrc;
		tdst.push_back(rs);//Rd
		tsrc.push_back(rm);
		tsrc.push_back(rs);
		emit(gcnew(assem::OPER, ("mul", "$d0, $s0, $s1", tdst, tsrc)));
		return rs;
	}
		
	{
		//(op,e1,e2)
		Temp *r = gcnew(Temp, ());
		TempList tdst, tsrc;
		tdst.push_back(r);
		tsrc.push_back(munchExp(binop->l));
		tsrc.push_back(munchExp(binop->r));
		emit(gcnew(assem::OPER, ("mul", "$d0, $s0, $s1", tdst, tsrc)));
		return r;
	}

}

Temp *
ARMCodeGen::munchBINOP_DIV(tree::BINOP *binop)
{
	tree::CONST *konst, *konst2;

	//(op,CONST(i1),CONST(i2))
	if (_M0(CONST_T, konst) == binop->l &&
		_M0(CONST_T, konst2) == binop->r &&
		opt::getOptimizationOption(opt::CONSTANT_FOLDING) == opt::OPT_ENABLE ) {
		int value;
		value = konst->value / konst2->value;

		Temp *r = gcnew(Temp, ());
		std::string assem = format("$d0, =%d", value);
		emit(gcnew(assem::MOVE, ("ldr", assem, r, NULL)));
		return r;
	}
	//(op,e1,CONST(i))
	else if (_M0(CONST_T, konst) == binop->r) {
		if (konst->value == 1) {
			//"e1 / 1"
			return munchExp(binop->l);
		}
	}
	//(op,CONST(i),e1)
	else if (_M0(CONST_T, konst) == binop->l) {
		//can nothing
	}

	{
		//use builtin function of gcc (__aeabi_idiv)
		TempList tsrc;
		tree::ExpList args;
		args.push_back(binop->l);
		args.push_back(binop->r);
		munchArgs(args, &tsrc);
		emit(gcnew(assem::OPER, ("bl", "__aeabi_idiv", frame->registers().args, tsrc)));
		return frame->rv();
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
	//tdst.push_back(rv);

	LabelList targets;
	targets.push_back(c->func->label);
	//TODO: r0-r3 will be destoryed in c function
	assem::OPER *op = gcnew(assem::OPER, ("bl", "$j0", frame->registers().args, tsrc));
	op->setJumpTargets(targets);
	emit(op);
	return rv;
}

Temp *
ARMCodeGen::munchCONST(tree::CONST *c)
{
	//(i)
	std::string assem = format("$d0, =%d", c->value);

	Temp *r = gcnew(Temp, ());
	emit(gcnew(assem::OPER, ("ldr", assem, r, NULL)));
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
	std::string assem = format("$d0, %s", n->label->toString().c_str());
	Temp *r = gcnew(Temp, ());
	emit(gcnew(assem::OPER, ("ldr", assem, r, NULL)));
	return r;
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

