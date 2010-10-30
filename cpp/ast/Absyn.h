
#ifndef ABSYN_H
#define ABSYN_H

#include "Object.h"
#include <string>
#include <list>
#include <algorithm>
#include <vector>
#include "Symbol.h"
#include "AbsynVisitor.h"
#include "debug.h"

struct VarEntry;
struct FuncEntry;

#define ACCEPTOR virtual void accept(AbsynVisitor *v) { v->visit(this); }

struct Absyn : public Object
{
Absyn() : pos(-1) {}
Absyn(int pos) : pos(pos) {}
	virtual ~Absyn() {}
	virtual void accept(AbsynVisitor *v)=0;
	int pos;
};

struct Exp;
struct Dec;
struct RecordField;
struct TypeField;

class ExpList : public Object, public std::vector<Exp*> {};
class Seq : public Object, public std::vector<Exp*> {};
class DecList : public Object, public std::vector<Dec*> {};
class RecordFieldList : public Object, public std::vector<RecordField*> {};
class TypeFieldList : public Object, public std::vector<TypeField*> {};
//typedef std::vector<Exp*> ExpList;
//typedef std::vector<Exp*> Seq;
//typedef std::vector<Dec*> DecList;
//typedef std::vector<RecordField*> RecordFieldList;
//typedef std::vector<TypeField*> TypeFieldList;


struct Var : Absyn
{
	Var(){}
 Var(int pos) : Absyn(pos), symInfo(NULL) {}

	enum VarID {
		SIMPLE,
		FIELD,
		SUBSCRIPT
	};
	VarID id;
	VarEntry *symInfo;
	virtual Symbol *getSymbol() = 0;
};

struct Exp : Absyn
{
	Exp(){}
 Exp(int pos) : Absyn(pos) {}
};


struct Dec : Absyn
{
	Dec(){}
 Dec(int pos) : Absyn(pos) {}
	virtual bool isFundec() { return false; }
};


struct Ty : Absyn
{
	Ty(){}
 Ty(int pos) : Absyn(pos) {}
};

enum Oper{
	PlusOp,
	MinusOp,
	TimesOp,
	DivideOp,
	EqOp,
	NeOp,
	LtOp,
	LeOp,
	GtOp,
	GeOp,
	AndOp,
	OrOp
};

typedef int Pos;

//Vars
struct SimpleVar : Var
{
 SimpleVar(Symbol *sym, int pos) 
	 : Var(pos), sym(sym) {
		id = Var::SIMPLE;
	}
	Symbol *sym;
	virtual Symbol *getSymbol() {
		return sym;
	}
	ACCEPTOR;
};

struct FieldVar : Var
{
 FieldVar(Var *var, Symbol *sym, int pos)
	 : Var(pos), var(var), sym(sym) {
		id = Var::FIELD;
	}
	Var *var;
	Symbol *sym;
	virtual Symbol *getSymbol() {
		return var->getSymbol();
	}
	ACCEPTOR;
};

struct SubscriptVar : Var
{
 SubscriptVar(Var *var, Exp *exp, int pos) 
	 : Var(pos), var(var), exp(exp) {
		id = Var::SUBSCRIPT;
	}
	Var *var;
	Exp *exp;
	virtual Symbol *getSymbol() {
		return var->getSymbol();
	}
	ACCEPTOR;
};

//Exps
struct VarExp : Exp
{
 VarExp(Var *var) : var(var) {}
	Var *var;
	ACCEPTOR;
};

struct NilExp : Exp
{
	NilExp() {}
	ACCEPTOR;
};

struct IntExp : Exp
{
 IntExp(int n) : n(n) {}
	const int n;
	ACCEPTOR;
};

struct StringExp : Exp
{
 StringExp(const std::string &str, int pos) 
	 : Exp(pos), str(str) {}
	const std::string str;
	ACCEPTOR;
};

struct CallExp : Exp
{
 CallExp(Symbol *func, ExpList *explist, int pos)
	 : Exp(pos), func(func), explist(explist) {}
	Symbol *func;
	FuncEntry *fnInfo;
	ExpList *explist;
	ACCEPTOR;
};

struct OpExp : Exp
{
 OpExp(Exp *left, Oper op, Exp *right, int pos)
	 :Exp(pos), left(left), right(right), op(op) {}
	Exp *left;
	Exp *right;
	Oper op;
	ACCEPTOR;
};


struct RecordField : Absyn
{
 RecordField(Symbol *name, Exp *init, int pos)
	 : Absyn(pos), name(name), init(init) {}
	Symbol *name;
	VarEntry *symInfo;
	Exp *init;
	ACCEPTOR;
};


struct RecordExp : Exp
{
 RecordExp(RecordFieldList *fields, Symbol *type, int pos)
	 : Exp(pos), fields(fields), type(type) {}
	RecordFieldList *fields;
	Symbol *type;
	ACCEPTOR;
};


struct SeqExp : Exp
{
 SeqExp(Seq *seq)
	 : seq(seq) {}
	Seq *seq;
	ACCEPTOR;
};

struct AssignExp : Exp
{
 AssignExp(Var *var, Exp *exp, int pos)
	 : Exp(pos) , var(var), exp(exp) {}
	Var *var;
	Exp *exp;
	ACCEPTOR;
};

struct IfExp : Exp
{
 IfExp(Exp *test, Exp *thenexp, int pos)
	 : Exp(pos), test(test), thenexp(thenexp), elseexp(0) {}

 IfExp(Exp *test, Exp *thenexp, Exp *elseexp, int pos)
	 : Exp(pos), test(test), thenexp(thenexp), elseexp(elseexp) {}
	Exp *test;
	Exp *thenexp;
	Exp *elseexp;
	ACCEPTOR;
};

struct WhileExp : Exp
{
 WhileExp(Exp *test, Exp *body, int pos)
	 : Exp(pos), test(test), body(body) {}
	Exp *test;
	Exp *body;
	ACCEPTOR;
};

struct ForExp : Exp
{
 ForExp(Symbol *var, bool escape, Exp *lo, Exp *hi, Exp *body, int pos)
	 : Exp(pos), var(var), escape(escape), lo(lo), hi(hi), body(body) {}
	Symbol *var;
	VarEntry *symInfo;
	bool escape;
	Exp *lo;
	Exp *hi;
	Exp *body;
	ACCEPTOR;
};
struct BreakExp : Exp
{
 BreakExp(int pos)
	 : Exp(pos) {}
	ACCEPTOR;
};

struct LetExp : Exp
{
 LetExp(DecList *decs, SeqExp *body, int pos)
	 : Exp(pos), decs(decs), body(body) {}
	DecList *decs;
	SeqExp *body;
	ACCEPTOR;
};

struct ArrayExp : Exp
{
 ArrayExp(Symbol *type, Exp *size, Exp *init, int pos)
	 : Exp(pos), type(type), size(size), init(init) {}
	Symbol *type;
	Exp *size;
	Exp *init;
	ACCEPTOR;
};

struct TypeField : Absyn
{
 TypeField(Symbol *name, bool escape, Symbol *type, int pos)
	 : Absyn(pos), name(name), escape(escape), type(type) 
	{}
	Symbol *name;
	bool escape;
	Symbol *type;
	VarEntry *symInfo;
	ACCEPTOR;
};

//Decs
struct FunDec : Dec
{
 FunDec(Symbol *name, TypeFieldList *params, Symbol *result, Exp *body, int pos)
	 : Dec(pos), name(name), params(params), result(result), body(body) {}

	virtual bool isFundec() { return true; }
	Symbol *name;
	FuncEntry *fnInfo;
	TypeFieldList *params;
	Symbol *result;
	Exp *body;
	ACCEPTOR;
};

struct VarDec : Dec
{
 VarDec(Symbol *name, bool escape, Symbol *type, Exp *init, int pos)
	 : Dec(pos), name(name), escape(escape), type(type), init(init) {}
	Symbol *name;
	VarEntry *symInfo;
	bool escape;
	Symbol *type;
	Exp *init;
	ACCEPTOR;
};

struct TypeDec : Dec
{
 TypeDec(Symbol *name, Ty *ty, int pos)
	 :Dec(pos), name(name), ty(ty) {}
	Symbol *name;
	Ty *ty;
	ACCEPTOR;
};

//Tys
struct NameTy : Ty
{
 NameTy(Symbol *name, int pos)
	 : Ty(pos), name(name) {}
	Symbol *name;
	ACCEPTOR;
};

struct RecordTy : Ty
{
 RecordTy(TypeFieldList *fields)
	 : fields(fields) {}
	
	TypeFieldList *fields;
	ACCEPTOR;
};

struct ArrayTy : Ty
{
 ArrayTy(Symbol *name, int pos)
	 : Ty(pos), name(name) {}
	Symbol *name;
	ACCEPTOR;
};


#endif //ABSYN_H
