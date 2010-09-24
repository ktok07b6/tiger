#ifndef CANON_H
#define CANON_H

#include "Tree.h"
#include "TreeVisitor.h"

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

class ExpCall : public tree::Stm
{
	tree::CALL *call;
 public:
	ExpCall(tree::CALL *c);
	virtual tree::ExpList kids();
	virtual tree::Stm *build(tree::ExpList kids);
    virtual void accept(tree::TreeVisitor *v);
};

class StmExpList
{
 public:
	StmExpList();
	StmExpList(tree::Stm *s, const tree::ExpList &e);

	tree::Stm *stm;
	tree::ExpList exps;
};

class Canon
{
	StmExpList nopNull;
 public:
	Canon();
	~Canon();
	bool isNop(tree::Stm *s);
	tree::Stm * seq(tree::Stm *s1, tree::Stm *s2);
	bool commute(tree::Stm *s, tree::Exp *e);
	tree::Stm *do_stm(tree::SEQ *seq);
	tree::Stm *do_stm(tree::MOVE *move);
	tree::Stm *do_stm(tree::EXPR *expr);
	tree::Stm *do_stm(tree::Stm *stm);
	tree::Stm *reorder_stm(tree::Stm *stm);
	tree::ESEQ *do_exp(tree::ESEQ *eseq);
	tree::ESEQ *do_exp(tree::Exp *exp);
	tree::ESEQ *reorder_exp(tree::Exp *exp);
	StmExpList reorder(tree::ExpList exps);
	tree::StmList linear(tree::Stm *s, tree::StmList l);
	tree::StmList linearize(tree::Stm *s);
};

#endif //CANON_H
