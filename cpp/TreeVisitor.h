#ifndef TREE_VISITOR_H
#define TREE_VISITOR_H

namespace tree {
class Tree;
class BINOP;
class CALL;
class CONST;
class ESEQ;
class MEM;
class NAME;
class TEMP;
class EXPR;
class CJUMP;
class JUMP;
class LABEL;
class MOVE;
class SEQ;

class TreeVisitor
{
 public:
	virtual ~TreeVisitor() {}
	virtual void visit(BINOP *binop) = 0;
	virtual void visit(CALL *call) = 0;
	virtual void visit(CONST *konst) = 0;
	virtual void visit(ESEQ *eseq) = 0;
	virtual void visit(MEM *mem) = 0;
	virtual void visit(NAME *name) = 0;
	virtual void visit(TEMP *temp) = 0;
	virtual void visit(EXPR *expr) = 0;
	virtual void visit(CJUMP *cjump) = 0;
	virtual void visit(JUMP *jump) = 0;
	virtual void visit(LABEL *label) = 0;
	virtual void visit(MOVE *move) = 0;
	virtual void visit(SEQ *seq) = 0;
};

}//namespace tree

#endif //TREE_VISITOR_H
