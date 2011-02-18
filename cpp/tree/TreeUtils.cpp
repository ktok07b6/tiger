#include "TreeUtils.h"
#include "TreeMatcher.h"

namespace tree {
bool isThereCALL(Stm *stm)
{
	SEQ *seq;
	MOVE *mov;
	CJUMP *cjmp;
	EXPR *expr;
	if (!stm) return false;
	if (_M0(SEQ_T, seq) == stm) {
		return isThereCALL(seq->l) || isThereCALL(seq->r);
	}
	if (_M0(MOVE_T, mov) == stm) {
		return isThereCALL(mov->src);
	}
	if (_M0(CJUMP_T, cjmp) == stm) {
		return isThereCALL(cjmp->l) || isThereCALL(cjmp->r);
	}
	if (_M0(EXPR_T, expr) == stm) {
		return isThereCALL(expr->exp);
	}
	return false;

}

bool isThereCALL(Exp *exp)
{
	CALL *call;
	MEM *mem;
	BINOP *binop;
	ESEQ *eseq;
	if (!exp) return false;
	if (_M0(CALL_T, call) == exp) {
		return true;
	}
	if (_M0(MEM_T, mem) == exp) {
		return isThereCALL(mem->exp);
	}
	if (_M0(BINOP_T, binop) == exp) {
		return isThereCALL(binop->l) || isThereCALL(binop->r);
	}
	if (_M0(ESEQ_T, eseq) == exp) {
		return isThereCALL(eseq->stm) || isThereCALL(eseq->exp);
	}

	return false;
}

}//namespace tree
