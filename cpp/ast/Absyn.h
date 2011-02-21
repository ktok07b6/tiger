
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

struct Absyn;
struct Symbol;
struct Var;
struct Exp;
struct Dec;
struct Ty;
struct Seq;
struct RecordField;
struct TypeField;
class ExpList;
class DecList;
class RecordFieldList;
class TypeFieldList;

#include "parser.hpp"

struct VarEntry;
struct FuncEntry;
struct Type;

#define ACCEPTOR virtual void accept(AbsynVisitor *v) { v->visit(this); }

typedef YYLTYPE TokenLocation;
#define LOC_PRINT(ast)								\
	do {											\
		if (ast->loc) {									\
			DBG("first_line", ast->tloc->first_line);	\
			DBG("first_column", ast->tloc->first_column);	\
			DBG("last_line", ast->tloc->last_line);			\
			DBG("last_column", ast->tloc->last_column);		\
		}													\
	} while (false)


struct Absyn : public Object
{
Absyn(TokenLocation *tloc) : tloc(tloc) {}
	virtual ~Absyn() {}
	virtual void accept(AbsynVisitor *v)=0;
	TokenLocation *tloc;
};

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
	//	Var(){}
 Var(TokenLocation *tloc) : Absyn(tloc), symInfo(NULL) {}

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
	//	Exp(){}
 Exp(TokenLocation *tloc) : Absyn(tloc) {}
};


struct Dec : Absyn
{
	//	Dec(){}
 Dec(TokenLocation *tloc) : Absyn(tloc) {}
	virtual bool isFundec() { return false; }
};


struct Ty : Absyn
{
	//	Ty(){}
 Ty(TokenLocation *tloc) : Absyn(tloc) {}
};


//Vars
struct SimpleVar : Var
{
 SimpleVar(Symbol *sym, TokenLocation *tloc) 
	 : Var(tloc), sym(sym) {
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
 FieldVar(Var *var, Symbol *sym, TokenLocation *tloc)
	 : Var(tloc), var(var), sym(sym) {
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
 SubscriptVar(Var *var, Exp *exp, TokenLocation *tloc) 
	 : Var(tloc), var(var), exp(exp) {
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
 VarExp(Var *var, TokenLocation *tloc) : Exp(tloc), var(var) {}
	Var *var;
	ACCEPTOR;
};

struct NilExp : Exp
{
 NilExp() : Exp(NULL){}
	ACCEPTOR;
};

struct IntExp : Exp
{
 IntExp(int n, TokenLocation *tloc) : Exp(tloc), n(n) {}
	const int n;
	ACCEPTOR;
};

struct StringExp : Exp
{
 StringExp(const std::string &str, TokenLocation *tloc) 
	 : Exp(tloc), str(str.substr(1, str.size()-2)) {
	}
	std::string str;
	ACCEPTOR;
};

struct CallExp : Exp
{
 CallExp(Symbol *func, ExpList *explist, TokenLocation *tloc)
	 : Exp(tloc), func(func), explist(explist) {}
	Symbol *func;
	FuncEntry *fnInfo;
	ExpList *explist;
	ACCEPTOR;
};

struct OpExp : Exp
{
 OpExp(Exp *left, Oper op, Exp *right, TokenLocation *tloc)
	 :Exp(tloc), left(left), right(right), op(op) {}
	Exp *left;
	Exp *right;
	Oper op;
	Type *l_t;
	Type *r_t;
	ACCEPTOR;
};


struct RecordField : Absyn
{
 RecordField(Symbol *name, Exp *init, TokenLocation *tloc)
	 : Absyn(tloc), name(name), init(init) {}
	Symbol *name;
	Exp *init;
	ACCEPTOR;
};


struct RecordExp : Exp
{
 RecordExp(RecordFieldList *fields, Symbol *type, TokenLocation *tloc)
	 : Exp(tloc), fields(fields), type(type) {}
	RecordFieldList *fields;
	Symbol *type;
	ACCEPTOR;
};


struct SeqExp : Exp
{
 SeqExp(Seq *seq)
	 : Exp(NULL), seq(seq) {}
	Seq *seq;
	ACCEPTOR;
};

struct AssignExp : Exp
{
 AssignExp(Var *var, Exp *exp, TokenLocation *tloc)
	 : Exp(tloc) , var(var), exp(exp) {}
	Var *var;
	Exp *exp;
	ACCEPTOR;
};

struct IfExp : Exp
{
 IfExp(Exp *test, Exp *thenexp, TokenLocation *tloc)
	 : Exp(tloc), test(test), thenexp(thenexp), elseexp(0) {}

 IfExp(Exp *test, Exp *thenexp, Exp *elseexp, TokenLocation *tloc)
	 : Exp(tloc), test(test), thenexp(thenexp), elseexp(elseexp) {}
	Exp *test;
	Exp *thenexp;
	Exp *elseexp;
	ACCEPTOR;
};

struct WhileExp : Exp
{
 WhileExp(Exp *test, Exp *body, TokenLocation *tloc)
	 : Exp(tloc), test(test), body(body) {}
	Exp *test;
	Exp *body;
	ACCEPTOR;
};

struct ForExp : Exp
{
 ForExp(Symbol *var, bool escape, Exp *lo, Exp *hi, Exp *body, TokenLocation *tloc)
	 : Exp(tloc), var(var), escape(escape), lo(lo), hi(hi), body(body) {}
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
 BreakExp(TokenLocation *tloc)
	 : Exp(tloc) {}
	ACCEPTOR;
};

struct LetExp : Exp
{
 LetExp(DecList *decs, SeqExp *body, TokenLocation *tloc)
	 : Exp(tloc), decs(decs), body(body) {}
	DecList *decs;
	SeqExp *body;
	ACCEPTOR;
};

struct ArrayExp : Exp
{
 ArrayExp(Symbol *type, Exp *size, Exp *init, TokenLocation *tloc)
	 : Exp(tloc), type(type), size(size), init(init) {}
	Symbol *type;
	Exp *size;
	Exp *init;
	ACCEPTOR;
};

struct TypeField : Absyn
{
 TypeField(Symbol *name, bool escape, Symbol *type, TokenLocation *tloc)
	 : Absyn(tloc), name(name), escape(escape), type(type) 
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
 FunDec(Symbol *name, TypeFieldList *params, Symbol *result, Exp *body, TokenLocation *tloc)
	 : Dec(tloc), name(name), params(params), result(result), body(body) {}

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
 VarDec(Symbol *name, bool escape, Symbol *type, Exp *init, TokenLocation *tloc)
	 : Dec(tloc), name(name), escape(escape), type(type), init(init) {}
	Symbol *name;
	VarEntry *symInfo;
	bool escape;
	Symbol *type;
	Exp *init;
	ACCEPTOR;
};

struct TypeDec : Dec
{
 TypeDec(Symbol *name, Ty *ty, TokenLocation *tloc)
	 :Dec(tloc), name(name), ty(ty) {}
	Symbol *name;
	Ty *ty;
	ACCEPTOR;
};

//Tys
struct NameTy : Ty
{
 NameTy(Symbol *name, TokenLocation *tloc)
	 : Ty(tloc), name(name) {}
	Symbol *name;
	ACCEPTOR;
};

struct RecordTy : Ty
{
 RecordTy(TypeFieldList *fields, TokenLocation *tloc)
	 : Ty(tloc), fields(fields) {}
	
	TypeFieldList *fields;
	ACCEPTOR;
};

struct ArrayTy : Ty
{
 ArrayTy(Symbol *name, TokenLocation *tloc)
	 : Ty(tloc), name(name) {}
	Symbol *name;
	ACCEPTOR;
};


#endif //ABSYN_H
