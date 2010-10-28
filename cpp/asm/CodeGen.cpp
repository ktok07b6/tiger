#include "CodeGen.h"
#include "Instruction.h"
#include "TreeMatcher.h"

using namespace std;

namespace assem {

CodeGen::CodeGen()
{
}

void 
CodeGen::generate(tree::Stm *s, InstructionList &instList)
{
	munchStm(s);
	instList = ilist;
	ilist.clear();
}

void 
CodeGen::munchStm(tree::Stm *s)
{
	if (s->isSEQ_T()) {
		tree::SEQ *seq = (tree::SEQ*)s;
		munchSEQ(seq->l, seq->r);
	} else if (s->isMOVE_T()) {
		tree::MOVE *mv = (tree::MOVE*)s;
		munchMOVE(mv->dst, mv->src);
	} else if (s->isLABEL_T()) {
		tree::LABEL *l = (tree::LABEL*)s;
		munchLABEL(l->label);
	} else if (s->isJUMP_T()) {
		tree::JUMP *j = (tree::JUMP*)s;
		assert(j->targets.size() == 1);
		munchJUMP(j->targets.front());
	} else if (s->isCJUMP_T()) {
		tree::CJUMP *cj = (tree::CJUMP*)s;
		munchCJUMP(cj);
	} else if (s->isEXPR_T()) {
		tree::EXPR *expr = (tree::EXPR*)s;
		munchEXPR(expr->exp);
	}
}

void
CodeGen::munchSEQ(tree::Stm *a, tree::Stm *b)
{
	munchStm(a); 
	munchStm(b);
}

Temp *
CodeGen::munchExp(tree::Exp *e)
{
	if (e->isMEM_T()) {
		return munchMEM((tree::MEM*)e);
	} else if (e->isBINOP_T()) {
		return munchBINOP((tree::BINOP*)e);
	} else if (e->isCONST_T()) {
		return munchCONST((tree::CONST*)e);
	} else if (e->isTEMP_T()) {
		return munchTEMP((tree::TEMP*)e);
	} else if (e->isCALL_T()) {
		return munchCALL((tree::CALL*)e);
	}
	assert(0);
	return NULL;
}

void 
CodeGen::emit(Instruction *inst)
{
	ilist.push_back(inst);
}

} //namespace assem
