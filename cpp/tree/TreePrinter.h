#ifndef TREE_PRINTER_H
#define TREE_PRINTER_H

#include "TreeVisitor.h"
#include <string>
#include "Tree.h"

namespace tree {

class TreePrinter : public TreeVisitor
{
 public:
	virtual void visit(BINOP *binop);
	virtual void visit(CALL *call);
	virtual void visit(CONST *konst);
	virtual void visit(ESEQ *eseq);
	virtual void visit(MEM *mem);
	virtual void visit(NAME *name);
	virtual void visit(TEMP *temp);
	virtual void visit(EXPR *EXPR);
	virtual void visit(CJUMP *cjump);
	virtual void visit(JUMP *jump);
	virtual void visit(LABEL *label);
	virtual void visit(MOVE *move);
	virtual void visit(SEQ *seq);

	static void printTree(Tree *t);
	static void printExpList(const ExpList &elist);
	static void printStmList(const StmList &slist);

 private:
	std::string result;
	static void printTree(Tree *t);
	static void printExpList(const ExpList &elist);
	static void printStmList(const StmList &slist);

 private:
	std::string tab;
};

}//namespace tree

#endif //TREE_PRINTER_H
