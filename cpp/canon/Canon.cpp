//#define CANON_DEBUG
//#define ENABLE_FUNCLOG
#include "Canon.h"
#include "TreePrinter.h"
#include "TreeMatcher.h"

class MoveCall : public tree::Stm
{
	tree::TEMP *dst;
	tree::CALL *src;

 public:
	MoveCall(tree::TEMP *d, tree::CALL *s);
	virtual tree::ExpList kids();
	virtual tree::Stm *build(tree::ExpList kids);
    virtual void accept(tree::TreeVisitor *v);
};

class MoveBinop : public tree::Stm
{
	tree::TEMP *dst;
	tree::BINOP *src;

 public:
	MoveBinop(tree::TEMP *d, tree::BINOP *s);
	virtual tree::ExpList kids();
	virtual tree::Stm *build(tree::ExpList kids);
    virtual void accept(tree::TreeVisitor *v);
};

class ExpCall : public tree::Stm
{
	tree::CALL *call;
 public:
	ExpCall(tree::CALL *c);
	virtual tree::ExpList kids();
	virtual tree::Stm *build(tree::ExpList kids);
    virtual void accept(tree::TreeVisitor *v);
};

MoveCall::MoveCall(tree::TEMP *d, tree::CALL *s)
	: Stm(MOVE_T)
	, dst(d)
	, src(s)
{
}
 
tree::ExpList
MoveCall::kids()
{
	return src->kids();
}

tree::Stm *
MoveCall::build(tree::ExpList kids)
{
	return _MOVE(dst, src->build(kids));
}

void 
MoveCall::accept(tree::TreeVisitor *v)
{
	tree::MOVE *m = _MOVE(dst, src);
	v->visit(m);
}

MoveBinop::MoveBinop(tree::TEMP *d, tree::BINOP *s)
	: Stm(MOVE_T)
	, dst(d)
	, src(s)
{
}
 
tree::ExpList
MoveBinop::kids()
{
	return src->kids();
}

tree::Stm *
MoveBinop::build(tree::ExpList kids)
{
	return _MOVE(dst, src->build(kids));
}

void 
MoveBinop::accept(tree::TreeVisitor *v)
{
	tree::MOVE *m = _MOVE(dst, src);
	v->visit(m);
}


ExpCall::ExpCall(tree::CALL *c)
	: Stm(EXPR_T)
	, call(c)
{
}

tree::ExpList
ExpCall::kids()
{
	return call->kids();
}

tree::Stm *
ExpCall::build(tree::ExpList kids)
{
	return _EXPR(call->build(kids));
}

void 
ExpCall::accept(tree::TreeVisitor *v)
{
	tree::EXPR *expr = _EXPR(call);
	v->visit(expr);
}




//--------------------------------------------------

Canon::Canon()
{
	nopNull.stm = _EXPR(_CONST(0));

#ifdef CANON_DEBUG
	dumpfp = fopen("canon.dump", "w");
#endif
}

Canon::~Canon()
{
#ifdef CANON_DEBUG
	fclose(dumpfp);
#endif
}

void
Canon::dump(tree::Tree *s, void *tag)
{
#ifdef CANON_DEBUG
	fprintf(dumpfp, "========================== %d %p\n", s->id, tag);
	tree::TreePrinter printer;
	s->accept(&printer);
	fprintf(dumpfp, "%s", printer.getResult().c_str());
#endif
}

bool
Canon::isNop(tree::Stm *s) 
{
	//it has no side effects
	if (!s) return true;
	return (s->isEXPR_T() && ((tree::EXPR*)s)->exp && ((tree::EXPR*)s)->exp->isCONST_T());
}

tree::Stm *
Canon::seq(tree::Stm *s1, tree::Stm *s2)
{
	FUNCLOG;
	//remove a no side effect statement
	if (isNop(s1)) return s2;
	if (isNop(s2)) return s1;
	return _SEQ(s1, s2);
}

bool
Canon::commute(tree::Stm *s, tree::Exp *e)
{
	return isNop(s) || e->isNAME_T() || e->isCONST_T();
}

tree::Stm *
Canon::do_stm(tree::SEQ *s)
{
	FUNCLOG;
	return seq(do_stm(s->l), do_stm(s->r));
}

tree::Stm *
Canon::do_stm(tree::MOVE *move)
{
	FUNCLOG;
	tree::TEMP *temp;
	tree::CALL *call;
	tree::ESEQ *eseq;
	if (_M0(TEMP_T, temp) == move->dst && _M0(CALL_T, call) == move->src) {
		return reorder_stm(gcnew(MoveCall, (temp, call)));
	}
	/*
	else if (move->dst->isTEMP_T() && move->src->isBINOP_T()) {
		return reorder_stm(gcnew(MoveBinop, ((tree::TEMP*)move->dst, (tree::BINOP*)move->src)));
	}
	*/
	else if(_M0(ESEQ_T, eseq) == move->dst) {
		//remove ESEQ
		tree::MOVE *move2 = _MOVE(eseq->exp, move->src);
		tree::SEQ *seq = _SEQ(eseq->stm, move2);
		return do_stm(seq);
	} else {
		return reorder_stm(move);
	}
}


tree::Stm *
Canon::do_stm(tree::EXPR *expr)
{
	FUNCLOG;
	tree::CALL *call;
	if (_M0(CALL_T, call) == expr->exp) {
		return reorder_stm(gcnew(ExpCall, (call)));
	} else {
		return reorder_stm(expr);
	}
}


tree::Stm *
Canon::do_stm(tree::Stm *stm)
{
	FUNCLOG;
	if (!stm) {
		VDBG("stm is nil");
		return NULL;
	}

	tree::SEQ *seq;
	tree::MOVE *mov;
	tree::EXPR *expr;
	if (_M0(SEQ_T, seq) == stm) {
		return do_stm(seq);
	}
	if (_M0(MOVE_T, mov) == stm) {
		return do_stm(mov);
	}
	if (_M0(EXPR_T, expr) == stm) {
		return do_stm(expr);
	}
	return reorder_stm(stm);

}

tree::Stm *
Canon::reorder_stm(tree::Stm *stm)
{
	FUNCLOG;
	if (!stm) {
		VDBG("stm is nil");
		return NULL;
	}
#ifdef CANON_DEBUG
	int tag;
	dump(stm, &tag);
#endif
	StmExpList x = reorder(stm->kids());
	tree::Stm *newstm = seq(x.stm, stm->build(x.exps));
#ifdef CANON_DEBUG
	dump(newstm, &tag);
#endif
	return newstm;
}


tree::ESEQ *
Canon::do_exp(tree::ESEQ *eseq)
{
	FUNCLOG;
	tree::Stm *stm = do_stm(eseq->stm);
	tree::ESEQ *b = do_exp(eseq->exp);
	if (b) {
	  return _ESEQ(seq(stm, b->stm), b->exp);
	} else {
	  return _ESEQ(stm, NULL);
	}
}

tree::ESEQ *
Canon::do_exp(tree::Exp *exp)
{
	FUNCLOG;
	if (!exp) {
		VDBG("exp is nil");
		return NULL;
	}

	tree::ESEQ *eseq;
	if (_M0(ESEQ_T, eseq) == exp) {
		return do_exp(eseq);
	} else {
		return reorder_exp(exp);
	}
}

tree::ESEQ *
Canon::reorder_exp(tree::Exp *exp)
{
	FUNCLOG;
#ifdef CANON_DEBUG
	int tag;
	dump(exp, &tag);
#endif
	StmExpList x = reorder(exp->kids());
	tree::ESEQ *eseq = _ESEQ(x.stm, exp->build(x.exps));
#ifdef CANON_DEBUG
	dump(eseq, &tag);
#endif
	return eseq;
}

StmExpList
Canon::reorder(tree::ExpList kids)
{
	FUNCLOG;
	if (kids.empty()) {
		VDBG("kids is expty");
		return nopNull;
	}

	//DBG("reorder kids ------------------ begin");
	//tree::TreePrinter::printExpList(kids);
	//DBG("reorder kids ------------------ end");

	tree::Exp *a = kids.pop_front();
	if (a->isCALL_T()/* || a->isBINOP_T()*/) {
		//replace CALL
		Temp *t = gcnew(Temp, ());
		tree::TEMP *tmp = _TEMP(t);
		tree::MOVE *mv = _MOVE(tmp, a);
		tree::Exp *e = _ESEQ(mv, tmp);
		//kids.push_front(e);
		//return reorder(kids);
		a = e;
	}

	{
		tree::ESEQ *aa = do_exp(a);
		StmExpList bb = reorder(kids);
		if (commute(bb.stm, aa->exp)) {
			tree::ExpList exps;
			exps.push_back(aa->exp);
			exps.push_all(bb.exps);
			return StmExpList(seq(aa->stm, bb.stm) ,exps);
		} else {
			Temp *t = gcnew(Temp, ());
			tree::TEMP *tmp = _TEMP(t);

			tree::ExpList exps;
			exps.push_back(tmp);
			exps.push_all(bb.exps);
	
			tree::MOVE *mv = _MOVE(tmp, aa->exp);
			return StmExpList(seq(aa->stm, seq(mv, bb.stm)), exps);
		}
	}

}


//SEQの要素をlistに格納
tree::StmList
Canon::linear(tree::Stm *s, tree::StmList stms)
{
	FUNCLOG;
	if (!s) {//skip nil
		return stms;
	}

	tree::SEQ *seq;
	if (_M0(SEQ_T, seq) == s) {
		return linear(seq->l, linear(seq->r, stms));
	} else {
		stms.push_front(s);
		return stms;
	}
}

tree::StmList
Canon::linearize(tree::Stm *s)
{
	FUNCLOG;
	tree::StmList stms;
	tree::Stm *stm = do_stm(s);

	//tree::TreePrinter::printTree(stm);

	return linear(stm, stms);
}




