#ifndef TREE2DOT_TRANSLATER_H
#define TREE2DOT_TRANSLATER_H

#include "TreeVisitor.h"

namespace tree {

class Tree2DotTranslater : public TreeVisitor
{
 public:
	virtual void visit(BINOP *binop);
	virtual void visit(CALL *call);
	virtual void visit(CONST *konst);
	virtual void visit(ESEQ *eseq);
	virtual void visit(MEM *mem);
	virtual void visit(NAME *name);
	virtual void visit(TEMP *temp);
	virtual void visit(EXPR *expr);
	virtual void visit(CJUMP *cjump);
	virtual void visit(JUMP *jump);
	virtual void visit(LABEL *label);
	virtual void visit(MOVE *move);
	virtual void visit(SEQ *seq);
};

}//namespace tree

#endif //TREE2DOT_TRANSLATER_H

