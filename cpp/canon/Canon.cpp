//#define CANON_DEBUG
//#define ENABLE_FUNCLOG
#include "Canon.h"
#include "TreePrinter.h"

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
	return gcnew(tree::MOVE, (dst, src->build(kids)));
}

void 
MoveCall::accept(tree::TreeVisitor *v)
{
	tree::MOVE *m = gcnew(tree::MOVE, (dst, src));
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
	return gcnew(tree::MOVE, (dst, src->build(kids)));
}

void 
MoveBinop::accept(tree::TreeVisitor *v)
{
	tree::MOVE *m = gcnew(tree::MOVE, (dst, src));
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
	return gcnew(tree::EXPR, (call->build(kids)));
}

void 
ExpCall::accept(tree::TreeVisitor *v)
{
	tree::EXPR *expr = gcnew(tree::EXPR, (call));
	v->visit(expr);
}




//--------------------------------------------------

Canon::Canon()
{
	tree::CONST *const0 = gcnew(tree::CONST, (0));
	tree::EXPR *expr = gcnew(tree::EXPR, (const0));
	nopNull.stm = expr;

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
	return gcnew(tree::SEQ, (s1, s2));
}

bool
Canon::commute(tree::Stm *s, tree::Exp *e)
{
	return isNop(s) || e->isNAME_T() || e->isCONST_T() || e->isTEMP_T() || e->isMEM_T();
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
	if (move->dst->isTEMP_T() && move->src->isCALL_T()) {
		return reorder_stm(gcnew(MoveCall, ((tree::TEMP*)move->dst, (tree::CALL*)move->src)));
	}
	else if (move->dst->isTEMP_T() && move->src->isBINOP_T()) {
		return reorder_stm(gcnew(MoveBinop, ((tree::TEMP*)move->dst, (tree::BINOP*)move->src)));
	}
	else if(move->dst->isESEQ_T()) {
		//remove ESEQ
		tree::ESEQ *eseq = (tree::ESEQ*)move->dst;
		tree::MOVE *move2 = gcnew(tree::MOVE, (eseq->exp, move->src));
		tree::SEQ *seq = gcnew(tree::SEQ, (eseq->stm, move2));
		return do_stm(seq);
	} else {
		return reorder_stm(move);
	}
}


tree::Stm *
Canon::do_stm(tree::EXPR *expr)
{
	FUNCLOG;
	if (expr->exp->isCALL_T()) {
		return reorder_stm(gcnew(ExpCall, ((tree::CALL*)expr->exp)));
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

	if (stm->isSEQ_T()) {
		return do_stm((tree::SEQ*)stm);
	}
	if (stm->isMOVE_T()) {
		return do_stm((tree::MOVE*)stm);
	}
	if (stm->isEXPR_T()) {
		return do_stm((tree::EXPR*)stm);
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
	  return gcnew(tree::ESEQ, (seq(stm, b->stm), b->exp));
	} else {
	  return gcnew(tree::ESEQ, (stm, NULL));
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

	if (exp->isESEQ_T()) {
		return do_exp((tree::ESEQ*)exp);
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
	tree::ESEQ *eseq = gcnew(tree::ESEQ, (x.stm, exp->build(x.exps)));
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
	if (a->isCALL_T() || a->isBINOP_T()) {
		//replace CALL
		Temp *t = gcnew(Temp, ());
		tree::TEMP *tmp = gcnew(tree::TEMP, (t));
		tree::MOVE *mv = gcnew(tree::MOVE, (tmp, a));
		tree::Exp *e = gcnew(tree::ESEQ, (mv, tmp));
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
			tree::TEMP *tmp = gcnew(tree::TEMP, (t));

			tree::ExpList exps;
			exps.push_back(tmp);
			exps.push_all(bb.exps);
	
			tree::MOVE *mv = gcnew(tree::MOVE, (tmp, aa->exp));
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

	if (s->isSEQ_T()) {
		tree::SEQ *seq = (tree::SEQ*)s;
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




