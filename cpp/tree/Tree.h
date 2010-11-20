#ifndef TREE_H
#define TREE_H

#include "Object.h"
#include "TreeVisitor.h"
#include "Label.h"
#include "Temp.h"
#include "HeapManager.h"
#include "debug.h"
#include "P2V.h"
#include "List.h"

namespace tree {

#define TREEACCEPTOR virtual void accept(TreeVisitor *v) { v->visit(this); }


class Tree : public Object
{
 public:
	enum TypeID{
		BINOP_T,
		CALL_T,
		CONST_T,
		ESEQ_T,
		MEM_T,
		NAME_T,
		TEMP_T,
		EXPR_T,
		CJUMP_T,
		JUMP_T,
		LABEL_T,
		MOVE_T,
		SEQ_T,
	};

	Tree(TypeID id);
	virtual ~Tree();
    virtual void accept(TreeVisitor *v) = 0;

	const TypeID id;
	bool isBINOP_T();
	bool isCALL_T();
	bool isCONST_T();
	bool isESEQ_T();
	bool isMEM_T();
	bool isNAME_T();
	bool isTEMP_T();
	bool isEXPR_T();
	bool isCJUMP_T();
	bool isJUMP_T();
	bool isLABEL_T();
	bool isMOVE_T();
	bool isSEQ_T();
};

inline bool Tree::isBINOP_T() 
{
	return id == BINOP_T;
}
inline bool Tree::isCALL_T()
{
	return id == CALL_T;
}
inline bool Tree::isCONST_T()
{
	return id == CONST_T;
}
inline bool Tree::isESEQ_T()
{
	return id == ESEQ_T;
}	
inline bool Tree::isMEM_T()
{
	return id == MEM_T;
}
inline bool Tree::isNAME_T()
{
	return id == NAME_T;
}
inline bool Tree::isTEMP_T()
{
	return id == TEMP_T;
}
inline bool Tree::isEXPR_T()
{
	return id == EXPR_T;
}
inline bool Tree::isCJUMP_T()
{
	return id == CJUMP_T;
}
inline bool Tree::isJUMP_T()
{
	return id == JUMP_T;
}
inline bool Tree::isLABEL_T()
{
	return id == LABEL_T;
}
inline bool Tree::isMOVE_T()
{
	return id == MOVE_T;
}
inline bool Tree::isSEQ_T()
{
	return id == SEQ_T;
}

class Exp;
class Stm;
typedef List<Exp> ExpList;
typedef List<Stm> StmList;

class Exp : public Tree 
{
 public:
	Exp(TypeID id);
	virtual ExpList kids() = 0;
	virtual Exp *build(ExpList kids) = 0;
};

class Stm : public Tree 
{
 public:
	Stm(TypeID id);
	virtual ExpList kids() = 0;
	virtual Stm *build(ExpList kids) = 0;
};


class BINOP : public Exp
{
 public:
	enum Oper{
		oPLUS, oMINUS, oMUL, oDIV,
		oAND, oOR, oXOR,
		oLSHIFT, oRSHIFT, oARSHIFT
	};

	BINOP(Oper op, Exp *l, Exp *r); 
	~BINOP() {FUNCLOG;}
	virtual ExpList kids();
	virtual Exp *build(ExpList kids);
	TREEACCEPTOR;

	Oper op;
	Exp *l;
	Exp *r;
};


class CALL : public Exp
{
 public:
	CALL(NAME *f, const ExpList &args);
	~CALL() {FUNCLOG;}
	virtual ExpList kids();
	virtual Exp *build(ExpList kids);
	TREEACCEPTOR;

	NAME *func;
	ExpList args;
};


class CONST : public Exp
{
 public:
	CONST(int value);
	~CONST() {FUNCLOG;}
	virtual ExpList kids();
	virtual Exp *build(ExpList kids);
	TREEACCEPTOR;

	int value;
};


class ESEQ : public Exp
{
 public:
	ESEQ(Stm *s, Exp *e);
	~ESEQ() {FUNCLOG;}
	virtual ExpList kids();
	virtual Exp *build(ExpList kids);
	TREEACCEPTOR;

	Stm *stm;
	Exp *exp;
};


class MEM : public Exp
{
 public:
	MEM(Exp *e);
	~MEM() {FUNCLOG;}
	virtual ExpList kids();
	virtual Exp *build(ExpList kids);
	TREEACCEPTOR;

	Exp *exp;
};


class NAME : public Exp
{
 public:
	NAME(Label *l);
	~NAME() {FUNCLOG;}
	virtual ExpList kids();
	virtual Exp *build(ExpList kids);
	TREEACCEPTOR;

	Label *label;
};


class TEMP : public Exp
{
 public:
	TEMP(Temp *t);
	~TEMP() {FUNCLOG;}
	virtual ExpList kids();
	virtual Exp *build(ExpList kids);
	TREEACCEPTOR;

	Temp *temp;
};

//-----------------------------------------------------------
class EXPR : public Stm
{
 public:
	EXPR(Exp *e);
	~EXPR() {FUNCLOG;}
	virtual ExpList kids();
	virtual Stm *build(ExpList kids);
	TREEACCEPTOR;

	Exp *exp;
};

class CJUMP : public Stm
{
 public:
	enum RelOp {
		oEQ,	oNE,	
		oLT,	oGT,	
		oLE,	oGE,	
		oULT,oUGT,
		oULE,oUGE
	};
	CJUMP(RelOp relop, Exp *l, Exp *r, Label *t, Label *f);
	~CJUMP() {FUNCLOG;}
	virtual ExpList kids();
	virtual Stm *build(ExpList kids);
	TREEACCEPTOR;

	static RelOp notRel(RelOp op);
	RelOp relop;
	Exp *l;
	Exp *r;
	Label *truelab;
	Label *falselab;
};

class JUMP : public Stm
{
 public:
	JUMP(Exp *e, const LabelList &t);
	JUMP(Label *target);
	~JUMP() {FUNCLOG;}
	virtual ExpList kids();
	virtual Stm *build(ExpList kids);
	TREEACCEPTOR;

	Exp *exp;
	LabelList targets;
};

class LABEL : public Stm
{
 public:
	LABEL(Label *l);
	~LABEL() {FUNCLOG;}
	virtual ExpList kids();
	virtual Stm *build(ExpList kids);
	TREEACCEPTOR;

	Label *label;
};


class MOVE : public Stm
{
 public:
	MOVE(Exp *dst, Exp *src);
	~MOVE() {FUNCLOG;}
	virtual ExpList kids();
	virtual Stm *build(ExpList kids);
	TREEACCEPTOR;

	Exp *dst;
	Exp *src;
};


class SEQ : public Stm
{
 public:
	SEQ();
	SEQ(Stm *l, Stm *r);
	~SEQ() {FUNCLOG;}
	virtual ExpList kids();
	virtual Stm *build(ExpList kids);
	TREEACCEPTOR;

	Stm *l;
	Stm *r;
};

class SEQMaker
{
 public:
	void add(Stm *s);
	SEQ *make();
 private:
	std::list<Stm*> vec;
};
/*
SEQ *makeSEQ(Stm *s1, Stm *s2);
SEQ *makeSEQ(Stm *s1, Stm *s2, Stm *s3);
SEQ *makeSEQ(Stm *s1, Stm *s2, Stm *s3, Stm *s4);
SEQ *makeSEQ(Stm *s1, Stm *s2, Stm *s3, Stm *s4, Stm *s5);
SEQ *makeSEQ(Stm *s1, Stm *s2, Stm *s3, Stm *s4, Stm *s5, Stm *s6);
SEQ *makeSEQ(Stm *s1, Stm *s2, Stm *s3, Stm *s4, Stm *s5, Stm *s6, Stm *s7);
*/

inline
BINOP *operator +(const P2V<Exp> &l, const P2V<Exp> &r)
{
	return gcnew(BINOP, (BINOP::oPLUS, l.ptr, r.ptr));
}

inline
BINOP *operator *(const P2V<Exp> &l, const P2V<Exp> &r)
{
	return gcnew(BINOP, (BINOP::oMUL, l.ptr, r.ptr));
}


#define _BINOP(op, l, r) gcnew(tree::BINOP, (op, l, r))
#define _CALL(fn, args) gcnew(tree::CALL, (fn, args))
#define _CONST(n) gcnew(tree::CONST, (n))
#define _ESEQ(s, e) gcnew(tree::ESEQ, (s, e))
#define _MEM(e) gcnew(tree::MEM, (e))
#define _NAME(l) gcnew(tree::NAME, (l))
#define _TEMP(t) gcnew(tree::TEMP, (t))

#define _EXPR(e) gcnew(tree::EXPR, (e))
#define _CJUMP(relop, l, r, t, f) gcnew(tree::CJUMP, (relop, l, r, t, f))
#define _JUMP(l) gcnew(tree::JUMP, (l))
#define _JUMP2(e, t) gcnew(tree::JUMP, (e, t))
#define _LABEL(l) gcnew(tree::LABEL, (l))
#define _MOVE(d, s) gcnew(tree::MOVE, (d, s))
#define _SEQ(l, r) gcnew(tree::SEQ, (l, r))
#define _SEQ0() gcnew(tree::SEQ, ())

} //namespace tree

#endif //TREE_H
