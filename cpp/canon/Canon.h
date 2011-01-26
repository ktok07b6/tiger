#ifndef CANON_H
#define CANON_H

#include "Tree.h"
#include "TreeVisitor.h"
#include <stdio.h>


struct StmExpList
{
StmExpList(): stm(NULL), exps(){}
StmExpList(tree::Stm *s, const tree::ExpList &e) : stm(s), exps(e){}
StmExpList(const StmExpList &other)	: stm(other.stm), exps(other.exps){}
	tree::Stm *stm;
	tree::ExpList exps;
};

class Canon
{
	StmExpList nopNull;
	FILE *dumpfp;
	void dump(tree::Tree*, void*);
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
