#include "TreeUtils.h"
#include "TreeMatcher.h"

namespace tree {

bool isExistType(Stm *stm, Tree::TypeID type)
{
	SEQ *seq;
	MOVE *mov;
	CJUMP *cjmp;
	EXPR *expr;
	if (!stm) return false;
	else if (stm->id == type) return true;

	else if (_M0(MOVE_T, mov) == stm) {
		return isExistType(mov->src, type);
	}
	else if (_M0(SEQ_T, seq) == stm) {
		return isExistType(seq->l, type) || isExistType(seq->r, type);
	}
	else if (_M0(CJUMP_T, cjmp) == stm) {
		return isExistType(cjmp->l, type) || isExistType(cjmp->r, type);
	}
	else if (_M0(EXPR_T, expr) == stm) {
		return isExistType(expr->exp, type);
	}
	else return false;

}

bool isExistType(Exp *exp, Tree::TypeID type)
{
	CALL *call;
	MEM *mem;
	BINOP *binop;
	ESEQ *eseq;
	if (!exp) return false;
	else if (exp->id == type) return true;

	else if (_M0(ESEQ_T, eseq) == exp) {
		return isExistType(eseq->stm, type) || isExistType(eseq->exp, type);
	}
	else if (_M0(BINOP_T, binop) == exp) {
		return isExistType(binop->l, type) || isExistType(binop->r, type);
	}
	else if (_M0(MEM_T, mem) == exp) {
		return isExistType(mem->exp, type);
	}
	else if (_M0(CALL_T, call) == exp) {
		ExpList::const_iterator it = call->args.begin();
		while (it != call->args.end()) {
			if (isExistType(*it, type)) {
				return true;
			}
			++it;
		}
	}
	else return false;
}

}//namespace tree
