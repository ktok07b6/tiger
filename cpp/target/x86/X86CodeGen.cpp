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

bool 
X86CodeGen::isInFrameAccess(tree::Exp *e, tree::CONST **offset)
{
	tree::MEM *mem;
	tree::BINOP *binop;
	tree::CONST *konst;
	if (_M1(MEM_T, mem, _M0(BINOP_T, binop)) == e) {
		if (binop->op == tree::BINOP::oPLUS) {
			if (_M0(CONST_T, konst) == binop->r && 
				munchExp(binop->l) == frame->registers().all[X86Frame::EBP]) {
				*offset = konst;
				return true;
			}
		}
	}
	return false;
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
			_M0(CONST_T, konst) != binop->l) {

			//(MEM(BINOP(PLUS,e1,CONST(i))), e2)
			std::string assem;
			TempList tsrc;
			tsrc.push_back(munchExp(binop->l));
			tsrc.push_back(munchExp(src));
			assem = format("'s1, %d('s0)", konst->value);
			emit(gcnew(assem::OPER, ("movl", assem, TempList(), tsrc)));
			return;
		} else if (_M0(CONST_T, konst) == binop->l &&
				   _M0(CONST_T, konst2) != binop->r) {
			//MEM(BINOP(PLUS,CONST(i),e1)),e2))
			std::string assem;
			TempList tsrc;
			tsrc.push_back(munchExp(binop->r));
			tsrc.push_back(munchExp(src));
			assem = format("'s1, %d('s0)", konst->value);
			emit(gcnew(assem::OPER, ("movl", assem, TempList(), tsrc)));
			return;
		} 
	}
	
	//(MEM(e1),e2)
	if (_M0(MEM_T, mem) == dst) {
		TempList tsrc;
		tsrc.push_back(munchExp(mem->exp));
		tsrc.push_back(munchExp(src));
		emit(gcnew(assem::OPER, ("movl", "'s1, ('s0)", TempList(), tsrc)));
		return;
	}

	//(TEMP(i), CONST(e2))
	if (_M0(TEMP_T, temp) == dst && _M0(CONST_T, konst) == src) {
		std::string assem = format("$%d, 'd0", konst->value);
		emit(gcnew(assem::MOVE, ("movl", assem, temp->temp, NULL)));
		return;
	}

	//(TEMP(i), MEM(bp+offset))
	if (_M0(TEMP_T, temp) == dst && isInFrameAccess(src, &konst)) {
		std::string assem = format("%d(%%ebp), 'd0", konst->value);
		emit(gcnew(assem::OPER, ("movl", assem, 
								 temp->temp, 
								 frame->registers().all[X86Frame::EBP])));
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
	assem::OPER *op = gcnew(assem::OPER, ("jmp", "'j0", NULL, NULL));
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
	const char *cond[] = {"e", "ne", 
						  "l", "g", 
						  "le", "ge"
	};
	//reverse condition for less or greater
	const char *cond_r[] = {"e", "ne", 
							"ge", "le", 
							"g", "l"
	};
	bool reverse = false;
	if (_M0(CONST_T, konst) == cj->l) {
		assem = format("$%d, 's0", konst->value);
		tsrc.push_back(munchExp(cj->r));
		reverse = true;
	} else if (_M0(CONST_T, konst) == cj->r) {
		assem = format("$%d, 's0", konst->value);
		tsrc.push_back(munchExp(cj->l));
	} else {
		assem = "'s1, 's0";
		tsrc.push_back(munchExp(cj->l));
		tsrc.push_back(munchExp(cj->r));
	}
	emit(gcnew(assem::OPER, ("cmpl", assem, TempList(), tsrc)));
	LabelList targets;
	targets.push_back(cj->truelab);
	targets.push_back(cj->falselab);
	assem = format("j%s", reverse ? cond_r[cj->relop] : cond[cj->relop]);
	assem::OPER *op = gcnew(assem::OPER, (assem, "'j0", NULL, NULL));
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
		assem::OPER *op = gcnew(assem::OPER, ("call", "'j0", frame->registers().callerSaves, tsrc));
		op->setJumpTargets(targets);
		emit(op);
	}
}

void
X86CodeGen::munchArgs(const tree::ExpList &exps, TempList *tsrc)
{
	frame->argSize(exps.size());
	tree::ExpList::const_reverse_iterator exp = exps.rbegin();
	tree::CONST *konst;
	int offset = exps.size() * frame->wordSize();
	while (exp != exps.rend()) {
		offset -= frame->wordSize();
		tree::Exp *e = *exp;
		if (_M0(CONST_T, konst) == e) {
			std::string assem = format("$%d, %d(%%esp)", konst->value, offset);
			emit(gcnew(assem::OPER, ("movl", assem, NULL, NULL)));
		} else {
			Temp *src = munchExp(e);
			std::string assem = format("'s0, %d(%%esp)", offset);
			emit(gcnew(assem::OPER, ("movl", assem, NULL, src)));
		}
		++exp;
	}
}


Temp *
X86CodeGen::munchMEM(tree::MEM *mem)
{
	tree::CONST *konst;
	if (isInFrameAccess(mem, &konst)) {
		Temp *r = gcnew(Temp, ());
		Temp *ebp = frame->registers().all[X86Frame::EBP];
		string assem = format("%d(%%ebp), 'd0", konst->value); 
		emit(gcnew(assem::OPER, ("movl", assem, r, ebp)));
		return r;
	} else {
		Temp *r = gcnew(Temp, ());
		emit(gcnew(assem::OPER, ("movl", 
								 "('s0), 'd0", 
								 r, 
								 munchExp(mem->exp))));
		return r;
	}
}

Temp *
X86CodeGen::munchBINOP(tree::BINOP *binop)
{
	TempList tsrc;
	TempList tdst;
	Temp *l = NULL;
	Temp *tmp = NULL;

	switch (binop->op) {
	case tree::BINOP::oPLUS:
		l = munchExp(binop->l);
		tmp = gcnew(Temp, ());
		emit(gcnew(assem::OPER, ("movl", "'s0, 'd0", tmp, l)));
		
		tsrc.push_back(munchExp(binop->r));
		tsrc.push_back(tmp);
		tdst.push_back(tmp);
		emit(gcnew(assem::OPER, ("addl", "'s0, 'd0", tdst, tsrc)));
		return tmp;
	case tree::BINOP::oMINUS:
		l = munchExp(binop->l);
		tmp = gcnew(Temp, ());
		emit(gcnew(assem::OPER, ("movl", "'s0, 'd0", tmp, l)));

		tsrc.push_back(munchExp(binop->r));
		tsrc.push_back(tmp);
		tdst.push_back(tmp);
		emit(gcnew(assem::OPER, ("subl", "'s0, 'd0", tdst, tsrc)));
		return tmp;
	case tree::BINOP::oMUL:
		l = munchExp(binop->l);
		tmp = gcnew(Temp, ());
		emit(gcnew(assem::OPER, ("movl", "'s0, 'd0", tmp, l)));

		tsrc.push_back(munchExp(binop->r));
		tsrc.push_back(tmp);
		tdst.push_back(tmp);

		emit(gcnew(assem::OPER, ("imull", "'s0, 'd0", tdst, tsrc)));
		return tmp;
	case tree::BINOP::oDIV: {
		Temp *eax = frame->registers().all[X86Frame::EAX];
		Temp *edx = frame->registers().all[X86Frame::EDX];
		l = munchExp(binop->l);
		emit(gcnew(assem::OPER, ("movl", "'s0, %eax", eax, l)));
		//emit(gcnew(assem::OPER, ("sarl", "$31, %edx", edx, NULL)));//clear edx
		emit(gcnew(assem::OPER, ("cltd", "", edx, NULL)));//eax -> edx:eax
		tsrc.push_back(munchExp(binop->r));
		tsrc.push_back(eax);
		tsrc.push_back(edx);
		tdst.push_back(eax);//Quotient
		tdst.push_back(edx);//Remainder

		emit(gcnew(assem::OPER, ("idivl", "'s0", tdst, tsrc)));
		return eax;
	}
		break;
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
	assem::OPER *op = gcnew(assem::OPER, ("call", "'j0", frame->registers().callerSaves, tsrc));
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

