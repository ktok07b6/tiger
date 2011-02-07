#include "X86CodeGen.h"
#include "Instruction.h"
#include "TreeMatcher.h"
#include "tiger.h"
#include "X86Frame.h"
#include "Optimizer.h"

using namespace std;


assem::CodeGen *
assem::CodeGen::create()
{
	return new X86CodeGen(NULL);
}


X86CodeGen::X86CodeGen(X86Frame *frame)
	: frame(frame)
{
}


void
X86CodeGen::munchMOVE(tree::Exp *dst, tree::Exp *src)
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
			assem = format("'s1, ['s0, #%d]", konst->value);

		} else if (_M0(CONST_T, konst) == binop->l &&
				   _M0(CONST_T, konst2) != binop->r) {
			//MEM(BINOP(PLUS,CONST(i),e1)),e2))
			//TODO:
		} 
	}
	
	//(MEM(e1),e2)
	if (_M0(MEM_T, mem) == dst) {
		TempList tsrc;
		tsrc.push_back(munchExp(mem->exp));
		tsrc.push_back(munchExp(src));
		//TODO:
		return;
	}

	//(TEMP(i), CONST(e2))
	if (_M0(TEMP_T, temp) == dst && _M0(CONST_T, konst) == src) {
		std::string assem = format("$%d, 'd0", konst->value);
		emit(gcnew(assem::MOVE, ("movl", assem, temp->temp, NULL)));
		return;
	}

	//(TEMP(i), e2)
	if (_M0(TEMP_T, temp) == dst) {
		Temp *ts = munchExp(src);
		emit(gcnew(assem::MOVE, ("movl", "'s0, 'd0", temp->temp, ts)));
		return;
	}
}

void
X86CodeGen::munchLABEL(Label *lab)
{
	emit(gcnew(assem::LABEL, (lab->toString() + ":", lab)));
}

void 
X86CodeGen::munchJUMP(Label *lab)
{
	LabelList targets;
	targets.push_back(lab);
	//TODO:
	assem::OPER *op = gcnew(assem::OPER, ("b", "$j0", NULL, NULL));
	op->setJumpTargets(targets);
	emit(op);
}

void 
X86CodeGen::munchCJUMP(tree::CJUMP *cj)
{
	tree::CONST *konst;
	std::string assem;
	TempList tsrc;

	//TODO:
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
	if (_M0(CONST_T, konst) == cj->l) {
		assem = format("'s0, #%d", konst->value);
		tsrc.push_back(munchExp(cj->r));
		reverse = true;
	} else if (_M0(CONST_T, konst) == cj->r) {
		assem = format("'s0, #%d", konst->value);
		tsrc.push_back(munchExp(cj->l));
	} else {
		assem = "'s0, 's1";
		tsrc.push_back(munchExp(cj->l));
		tsrc.push_back(munchExp(cj->r));
	}
	emit(gcnew(assem::OPER, ("cmp", assem, TempList(), tsrc)));
	LabelList targets;
	targets.push_back(cj->truelab);
	targets.push_back(cj->falselab);
	assem = format("j%s", reverse ? cond_r[cj->relop] : cond[cj->relop]);
	assem::OPER *op = gcnew(assem::OPER, (assem, "$j0", NULL, NULL));
	op->setJumpTargets(targets);
	emit(op);
}

void
X86CodeGen::munchEXPR(tree::Exp *exp)
{
	tree::CALL *call;
	if (_M0(CALL_T, call) == exp) {
		TempList tsrc;
		munchArgs(call->args, &tsrc);

		LabelList targets;
		targets.push_back(call->func->label);
		//TODO: eax, ecx, edx will be destoryed in c function
		assem::OPER *op = gcnew(assem::OPER, ("call", "$j0", frame->registers().callerSaves, tsrc));
		op->setJumpTargets(targets);
		emit(op);
	}
}

void
X86CodeGen::munchArgs(const tree::ExpList &exps, TempList *tsrc)
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
				std::string assem = format("$%d, 'd0", konst->value);
				emit(gcnew(assem::MOVE, ("movl", assem, tmp, NULL)));
				//FIXME:
				assem = format("'s0, [sp, #%d]", offset);
				emit(gcnew(assem::OPER, ("str", assem, regs.all[13], tmp)));
			} else {
				Temp *src = munchExp(e);
				emit(gcnew(assem::MOVE, ("mov", "'d0, 's0", tmp, src)));
				std::string assem = format("'s0, [sp, #%d]", offset);
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
			std::string assem = format("'d0, =%d", konst->value);
			emit(gcnew(assem::MOVE, ("ldr", assem, dst, NULL)));
		} else {
			Temp *src = munchExp(e);
			emit(gcnew(assem::MOVE, ("movl", "'s0, 'd0", dst, src)));
		}
		tsrc->push_back(dst);
		++exp;
		++arg_reg;
	}
}


Temp *
X86CodeGen::munchMEM(tree::MEM *mem)
{
	tree::BINOP *binop;
	tree::CONST *konst, *konst2;

	//(e1)
	Temp *r = gcnew(Temp, ());
	emit(gcnew(assem::MOVE, ("movl", 
							 "('s0), 'd0", 
							 r, 
							 munchExp(mem->exp))));
	return r;
}

Temp *
X86CodeGen::munchBINOP(tree::BINOP *binop)
{
	//TODO:
	switch (binop->op) {
	case tree::BINOP::oPLUS:
	case tree::BINOP::oMINUS:
	case tree::BINOP::oMUL:
	case tree::BINOP::oDIV:
	default:
		assert(0);
		return NULL;
	}
}

Temp *
X86CodeGen::munchCALL(tree::CALL *c)
{
	TempList tsrc;
	munchArgs(c->args, &tsrc);

	assert(frame);

	LabelList targets;
	targets.push_back(c->func->label);
	//eax, ecx, edx will be destoryed in c function
	assem::OPER *op = gcnew(assem::OPER, ("call", "$j0", frame->registers().callerSaves, tsrc));
	op->setJumpTargets(targets);
	emit(op);
	return frame->rv();
}

Temp *
X86CodeGen::munchCONST(tree::CONST *c)
{
	//(i)
	std::string assem = format("$%d, 'd0", c->value);
	Temp *r = gcnew(Temp, ());
	emit(gcnew(assem::OPER, ("movl", assem, r, NULL)));
	return r;
}

Temp *
X86CodeGen::munchTEMP(tree::TEMP *t)
{
	return t->temp;
}

Temp *
X86CodeGen::munchNAME(tree::NAME *n)
{
	std::string assem = format("%s, 'd0", n->label->toString().c_str());
	Temp *r = gcnew(Temp, ());
	emit(gcnew(assem::OPER, ("movl", assem, r, NULL)));
	return r;
}

bool 
X86CodeGen::isMove(assem::Instruction *inst)
{
	return inst->isMOVE();
	/*
	std::string assem = inst->toString();
	return strncmp("mov", assem.c_str(), 3) == 0;
	*/
}

bool 
X86CodeGen::isJump(assem::Instruction *inst)
{
	return inst->getOpcode() == "jmp";
}

