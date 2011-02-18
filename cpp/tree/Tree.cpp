//#define ENABLE_FUNCLOG
#include "TreeVisitor.h"
#include "Tree.h"
#include "TreePrinter.h"
#include "TreeMatcher.h"

namespace tree {

Tree::Tree(TypeID id) : id (id) {}
Tree::~Tree() {}

Exp::Exp(TypeID id) : Tree(id) {}
Stm::Stm(TypeID id) : Tree(id) {}

BINOP::BINOP(Oper op, Exp *l, Exp *r)
	: Exp(BINOP_T)
	, op(op)
	, l(l)
	, r(r)
{
}

ExpList 
BINOP::kids()
{
	FUNCLOG;
	ExpList li;
	li.push_back(l);
	li.push_back(r);
	return li;
}

Exp *
BINOP::build(ExpList kids)
{
	FUNCLOG;
	Exp *ll = kids.pop_front();
	Exp *rr = kids.pop_front();

	return _BINOP(op, ll, rr);
}


CALL::CALL(NAME *f, const ExpList &args)
	: Exp(CALL_T)
	, func(f)
	, args(args)
{
}

ExpList
CALL::kids()
{
	FUNCLOG;
	ExpList a = args;
	a.push_front(func);
	return a;
}

Exp *
CALL::build(ExpList kids)
{
	FUNCLOG;

	int s = kids.size();
	Exp *f = kids.pop_front();
	assert(kids.size() == s-1);
	assert(f->isNAME_T());
	return _CALL((NAME*)f, kids);
}


CONST::CONST(int v)
	: Exp(CONST_T)
	, value(v)
{
}

ExpList 
CONST::kids()
{
	FUNCLOG;
	return ExpList();
}

Exp *
CONST::build(ExpList kids)
{
	FUNCLOG;
	return this;
}

ESEQ::ESEQ(Stm *s, Exp *e)
	: Exp(ESEQ_T)
	, stm(s)
	, exp(e)
{
}

ExpList
ESEQ::kids()
{
	FUNCLOG;
	assert(0);
	return ExpList();
}

Exp *
ESEQ::build(ExpList kids)
{
	FUNCLOG;
	assert(0);
	return this;
}


MEM::MEM(Exp *e)
	: Exp(MEM_T)
	, exp(e)
{
}

ExpList
MEM::kids()
{
	FUNCLOG;
	ExpList li;
	li.push_back(exp);
	return li;
}

Exp *
MEM::build(ExpList kids)
{
	FUNCLOG;
	Exp *e = kids.pop_front();
	return _MEM(e);
}


NAME::NAME(Label *l)
	: Exp(NAME_T)
	, label(l)
{
}

ExpList
NAME::kids()
{
	FUNCLOG;
	return ExpList();
}

Exp *
NAME::build(ExpList kids)
{
	FUNCLOG;
	return this;
}


TEMP::TEMP(Temp *t)
	: Exp(TEMP_T)
	, temp(t)
{
}

ExpList
TEMP::kids()
{
	FUNCLOG;
	return ExpList();
}

Exp *
TEMP::build(ExpList kids)
{
	FUNCLOG;
	return this;
}


EXPR::EXPR(Exp *e)
	: Stm(EXPR_T)
	, exp(e)
{
}

ExpList
EXPR::kids()
{
	FUNCLOG;
	ExpList li;
	li.push_back(exp);
	return li;
}

Stm *
EXPR::build(ExpList kids)
{
	FUNCLOG;
	Exp *e = kids.pop_front();
	return _EXPR(e);
}


CJUMP::CJUMP(RelOp relop, Exp *l, Exp *r, Label *t, Label *f)
	: Stm(CJUMP_T)
	, relop(relop)
	, l(l)
	, r(r)
	, truelab(t)
	, falselab(f)
{
}

ExpList
CJUMP::kids()
{
	FUNCLOG;
	ExpList li;
	li.push_back(l);
	li.push_back(r);
	return li;
}

Stm *
CJUMP::build(ExpList kids)
{
	FUNCLOG;
	Exp *ll = kids.pop_front();
	Exp *rr = kids.pop_front();
	return _CJUMP(relop, ll, rr, truelab, falselab);
}

CJUMP::RelOp
CJUMP::notRel(RelOp op)
{
	switch (op) {
	case oEQ:
		return tree::CJUMP::oNE;
	case oNE:
		return tree::CJUMP::oEQ;
	case oLT:
		return tree::CJUMP::oGE;
	case oLE:
		return tree::CJUMP::oGT;
	case oGT:
		return tree::CJUMP::oLE;
	case oGE:
		return tree::CJUMP::oLT;
	default:
		assert(0);
	}
}

JUMP::JUMP(Exp *e, const LabelList &t)
	: Stm(JUMP_T)
	, exp(e)
	, targets(t)
{
}


JUMP::JUMP(Label *target)
	: Stm(JUMP_T)
	, exp(NULL)
	, targets()
{
	exp = _NAME(target);
	targets.push_back(target);
}

ExpList 
JUMP::kids()
{
	FUNCLOG;
	ExpList li;
	li.push_back(exp);
	return li;
}

Stm *
JUMP::build(ExpList kids)
{
	FUNCLOG;
	Exp *e = kids.pop_front();
	return _JUMP2(e, targets);
}


LABEL::LABEL(Label *l)
	: Stm(LABEL_T)
	, label(l)
{
}

ExpList
LABEL::kids()
{
	FUNCLOG;
	return ExpList();
}

Stm *
LABEL::build(ExpList kids)
{
	FUNCLOG;
	return this;
}


MOVE::MOVE(Exp *dst, Exp *src)
	: Stm(MOVE_T)
	, dst(dst)
	, src(src)
{
	assert(dst && src);
}

ExpList
MOVE::kids()
{
	FUNCLOG;
	ExpList li;
	MEM *mem;
	if (_M0(MEM_T, mem)==dst) {
		li.push_back(mem->exp);
		li.push_back(src);
	} else {
		li.push_back(src);
	}
	return li;
}

Stm *
MOVE::build(ExpList kids)
{
	FUNCLOG;
	MEM *mem;
	if (_M0(MEM_T, mem)==dst) {
		Exp *d = kids.pop_front();
		Exp *s = kids.pop_front();
		return _MOVE(_MEM(d), s);
	} else {
		Exp *s = kids.pop_front();
		return _MOVE(dst, s);
	}
}



SEQ::SEQ()
	: Stm(SEQ_T)
	, l()
	, r()
{
}

SEQ::SEQ(Stm *l, Stm *r)
	: Stm(SEQ_T)
	, l(l)
	, r(r)
{
}

ExpList 
SEQ::kids()
{
	FUNCLOG;
	assert(0);
	return ExpList();
}

Stm *
SEQ::build(ExpList kids)
{
	FUNCLOG;
	assert(0);
	return this;
}

void
SEQMaker::add(Stm *s)
{
	vec.push_back(s);
}

SEQ *
SEQMaker::make()
{
	if (vec.empty()) {
		return NULL;
	}
	SEQ *seq = _SEQ0();
	SEQ *top = seq;
	SEQ *next;

	std::list<Stm*>::iterator it = vec.begin();
	do {
		Stm *s = *it;
		seq->l = s;
		++it;
		if (it != vec.end()) { 
			next = _SEQ0(); 
			seq->r = next;
			seq = next;
		} else {
			seq->r = NULL;
			break;
		}
	}while (true);

	return top;
}

/*
SEQ *makeSEQ(Stm *s1, 
			 Stm *s2)
{
	SEQ *seq = gcnew(SEQ, ());
	SEQ *top = seq;
	SEQ *next;
	
	seq->l = s1;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s2;
	seq->r = NULL;

	return top;
}

SEQ *makeSEQ(Stm *s1, 
			 Stm *s2, 
			 Stm *s3)
{
	SEQ *seq = gcnew(SEQ, ());
	SEQ *top = seq;
	SEQ *next;

	seq->l = s1;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s2;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s3;
	seq->r = NULL;

	return top;
}

SEQ *makeSEQ(Stm *s1, 
			 Stm *s2, 
			 Stm *s3, 
			 Stm *s4)
{
	SEQ *seq = gcnew(SEQ, ());
	SEQ *top = seq;
	SEQ *next;

	seq->l = s1;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s2;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s3;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s4;
	seq->r = NULL;

	return top;
}

SEQ *makeSEQ(Stm *s1, 
			 Stm *s2, 
			 Stm *s3, 
			 Stm *s4, 
			 Stm *s5)
{
	SEQ *seq = gcnew(SEQ, ());
	SEQ *top = seq;
	SEQ *next;

	seq->l = s1;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s2;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s3;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s4;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s5;
	seq->r = NULL;

	return top;
}

SEQ *makeSEQ(Stm *s1, 
			 Stm *s2, 
			 Stm *s3, 
			 Stm *s4, 
			 Stm *s5, 
			 Stm *s6)
{
	SEQ *seq = gcnew(SEQ, ());
	SEQ *top = seq;
	SEQ *next;

	seq->l = s1;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s2;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s3;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s4;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s5;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s6;
	seq->r = NULL;

	return top;
}

SEQ *makeSEQ(Stm *s1, 
			 Stm *s2, 
			 Stm *s3, 
			 Stm *s4, 
			 Stm *s5, 
			 Stm *s6, 
			 Stm *s7)
{
	SEQ *seq = gcnew(SEQ, ());
	SEQ *top = seq;
	SEQ *next;

	seq->l = s1;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s2;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s3;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s4;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s5;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s6;
	next = gcnew(SEQ, ()); 
	seq->r = next;
	seq = next;
		
	seq->l = s7;
	seq->r = NULL;

	return top;
}
*/

}//namespace tree
