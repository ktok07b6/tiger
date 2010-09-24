#include "AST2DotTranslater.h"
#include <cassert>
#include "Absyn.h"
#include "debug.h"

using namespace std;

AST2DotTranslater::AST2DotTranslater()
	: id(0) 
{
}


string
AST2DotTranslater::getid()
{
	char buf[16];
	sprintf(buf, "_%d", id++);
	return buf;
}

void
AST2DotTranslater::push(const string &s)
{
	stack.push(s);
}

string AST2DotTranslater::pop()
{
	assert(!stack.empty());
	string s = stack.top();
	stack.pop();
	return s;
}

void
AST2DotTranslater::visit(SimpleVar *var)
{
	string sresult;
	string name = "SimpleVar" + getid(); 
	sresult = name + " -> Symbol" + getid() + "_" + var->sym->name + ";\n";
	push(sresult);
}

void
AST2DotTranslater::visit(FieldVar *var)
{
	string svar, sresult;
	var->var->accept(this);
	svar = pop();
	string name = "FieldVar" + getid();
	sresult = name + " -> " + svar;
	sresult += name + " -> Symbol" + getid() + "_" + var->sym->name + ";\n";

	push(sresult);
}

void
AST2DotTranslater::visit(SubscriptVar *var)
{
	string svar, sexp, sresult;
	var->var->accept(this);
	var->exp->accept(this);
	sexp = pop();
	svar = pop();
	string name = "SubscriptVar" + getid(); 
	sresult = name + " -> " + svar;
	sresult += name + " -> " + sexp;
	push(sresult);
}

void
AST2DotTranslater::visit(VarExp *exp)
{
	string svar, sresult;
	string name = "VarExp" + getid(); 
	exp->var->accept(this);
	svar = pop();
	sresult = name + " -> " + svar;
	push(sresult);
}

void
AST2DotTranslater::visit(NilExp *exp)
{
	push("NilExp" + getid() + ";\n");
}

void
AST2DotTranslater::visit(IntExp *exp)
{
	char buf[32];
	sprintf(buf, "%d", exp->n);
	push("IntExp" + getid() + "_" + string(buf) + ";\n");
}

void
AST2DotTranslater::visit(StringExp *exp)
{
	push("StringExp" + getid() + "_" + exp->str + ";\n");
}

void
AST2DotTranslater::visit(CallExp *exp)
{
	string sfunc, sexps, sresult;
	string name = "CallExp" + getid();
	sfunc = exp->func->name + getid() + ";\n";
	sresult = name + " -> " + sfunc;

	ExpList::iterator it = exp->explist->begin();
	while (it != exp->explist->end()) {
		Exp *e = *it;
		if (e) {
			e->accept(this);
			sexps = pop();
			sresult += name + " -> " + sexps;
		}
		++it;
	}
	push(sresult);
}

namespace {
string op2str(Oper op) {
	switch(op) {
	case PlusOp:   return "\"+\"";
	case MinusOp:  return "\"-\"";
	case TimesOp:  return "\"*\"";
	case DivideOp: return "\"/\"";
	case EqOp:     return "\"=\"";
	case NeOp:     return "\"<>\"";
	case GeOp:     return "\">=\"";
	case LeOp:     return "\"<=\"";
	case GtOp:     return "\">\"";
	case LtOp:     return "\"<\"";
	case AndOp:    return "\"&\"";
	case OrOp:     return "\"|\"";
	default :      return "";
	}
}
}

void
AST2DotTranslater::visit(OpExp *exp)
{
	string sleft, sright, sresult;
	string name = "OpExp" + getid();

	exp->left->accept(this);
	exp->right->accept(this);
	sright = pop();
	sleft = pop();
	
	sresult = name + " -> " + sleft;
	sresult += name + " -> " + op2str(exp->op) + getid() + ";\n";
	sresult += name + " -> " + sright;
	push(sresult);
}

void
AST2DotTranslater::visit(RecordField *field)
{
	string sname, sinit, sresult;
	sname = field->name->name + getid() + ";\n";
	field->init->accept(this);
	sinit = pop();
	sresult = sname + " -> " + sinit;
	push(sresult);
}

void
AST2DotTranslater::visit(RecordExp *exp)
{
	string stype, sfields, sresult;

	stype = exp->type->name + getid() + ";\n";
	string name = "RecordExp" + getid();
	sresult = name + " -> " + stype;

	RecordFieldList::iterator it = exp->fields->begin();

	while (it != exp->fields->end()) {
		RecordField *f = *it;
		f->accept(this);
		sfields = pop();
		sresult += name + " -> " + sfields;
		++it;
	}
	
	push(sresult);
}

void
AST2DotTranslater::visit(SeqExp *exp)
{
	string sseq, sresult;
	string name = "SeqExp" + getid();
	Seq::iterator it = exp->seq->begin();
	while (it != exp->seq->end()) {
		Exp *e = *it;
		if (e) {
			e->accept(this);
			sseq = pop();
			sresult += name + " -> " + sseq;
		}
		++it;
	}
	
	push(sresult);
}

void
AST2DotTranslater::visit(AssignExp *exp)
{
	DBG("exp->var = %p, exp->exp = %p", exp->var, exp->exp);
	string svar, sexp, sresult;
	assert(exp->var);
	assert(exp->exp);
	exp->var->accept(this);
	exp->exp->accept(this);
	sexp = pop();
	svar = pop();
	string name = "AssignExp" + getid();
	sresult = name + " -> " + svar;
	sresult += name + " -> " + sexp;
	push(sresult);
}

void
AST2DotTranslater::visit(IfExp *exp)
{
	string stest, sthen, selse, sresult;
	exp->test->accept(this);
	stest = pop();
	exp->thenexp->accept(this);
	sthen = pop();
	string name = "IfExp" + getid();
	if(exp->elseexp) {
		exp->elseexp->accept(this);
		selse = pop();
		sresult = name + " -> " + stest;
		sresult += name + " -> " + sthen;
		sresult += name + " -> " + selse;
	} else {
		sresult = name + " -> " + stest;
		sresult += name + " -> " + sthen;
	}
	push(sresult);
}

void
AST2DotTranslater::visit(WhileExp *exp)
{
	string stest, sbody, sresult;
	exp->test->accept(this);
	stest = pop();
	exp->body->accept(this);
	sbody = pop();
	string name = "WhileExp" + getid();
	sresult = name + " -> " + stest;
	sresult += name + " -> " + sbody;
	push(sresult);
}

void
AST2DotTranslater::visit(ForExp *exp)
{
	string svar, sescape, slo, shi, sbody, sresult;
	svar = exp->var->name + getid() + ";\n";
	exp->lo->accept(this);
	slo = pop();
	exp->hi->accept(this);
	shi = pop();
	exp->body->accept(this);
	sbody = pop();
	string name = "ForExp" + getid();
	sresult = name + " -> " + svar;
	sresult += name + " -> " + slo;
	sresult += name + " -> " + shi;
	sresult += name + " -> " + sbody;
	push(sresult);
}

void
AST2DotTranslater::visit(BreakExp *exp)
{
	push("BreakExp" + getid() + ";\n");
}

void
AST2DotTranslater::visit(LetExp *exp)
{
	string sdecs, sbody, sresult;

	string name = "LetExp" + getid();
	DecList::iterator it = exp->decs->begin();
	while (it != exp->decs->end()) {
		Dec *d = *it;
		d->accept(this);
		sdecs = pop();
		sresult += name + " -> " + sdecs; 
		++it;
	}
	exp->body->accept(this);
	sbody = pop();
	sresult += name + " -> " + sbody;

	push(sresult);
}

void
AST2DotTranslater::visit(ArrayExp *exp)
{
	string stype, ssize, sinit, sresult;
	stype = exp->type->name + getid() + ";\n";
	exp->size->accept(this);
	ssize = pop();
	exp->init->accept(this);
	sinit = pop();
	string name = "ArrayExp" + getid();
	sresult = name + " -> " + stype;
	sresult += name + " -> " + ssize;
	sresult += name + " -> " + sinit;
	push(sresult);
}

void
AST2DotTranslater::visit(TypeField *field)
{
	string name = "TypeField" + getid();
	string sresult;
	sresult = name + " -> " + field->name->name + getid() + ";\n";
	sresult += name + " -> " + field->type->name + getid() + ";\n";
	push(sresult);
}

void
AST2DotTranslater::visit(FunDec *dec)
{
	string sname, sparams, srettype, sbody, sresult;
	string name = "FunDec" + getid();
	sname = dec->name->name + getid() + ";\n";
	sresult = name + " -> " + sname;
	TypeFieldList::iterator it = dec->params->begin();
	while (it != dec->params->end()) {
		TypeField *f = *it;
		f->accept(this);
		sparams = pop();
		sresult += name + " -> " + sparams;
		++it;
	}
	srettype = dec->result->name + getid() + ";\n";
	sresult += name + " -> " + srettype;

	dec->body->accept(this);
	sbody = pop();
	sresult += name + " -> " + sbody;

	push(sresult);
}

void
AST2DotTranslater::visit(VarDec *dec)
{
	string sname, stype, sinit, sresult;
	string name = "VarDec" + getid();
	sname = dec->name->name + getid() + ";\n";
	stype = dec->type->name + getid() + ";\n";
	dec->init->accept(this);
	sinit = pop();
	
	sresult = name + " -> " + sname;
	sresult += name + " -> " + stype;
	sresult += name + " -> " + sinit;

	push(sresult);
}

void
AST2DotTranslater::visit(TypeDec *dec)
{
	string sname, sty, sresult;
	string name = "TypeDec" + getid();
	
	sname = dec->name->name + getid() + ";\n";
	dec->ty->accept(this);
	sty = pop();
	
	sresult = name + " -> " + sname;
	sresult += name + " -> " + sty;

	push(sresult);
}

void
AST2DotTranslater::visit(NameTy *ty)
{
	string name = "NameTy" + getid();
	push(name + " -> " + ty->name->name + getid() + ";\n");
}

void
AST2DotTranslater::visit(RecordTy *ty)
{
	string sfields, sresult;
	string name = "RecordTy" + getid();

	TypeFieldList::iterator it = ty->fields->begin();
	while (it != ty->fields->end()) {
		TypeField *f = *it;
		f->accept(this);
		sfields = pop();
		sresult += name + " -> " + sfields;
		++it;
	}
	push(sresult);
}

void
AST2DotTranslater::visit(ArrayTy *ty)
{
	string name = "ArrayTy" + getid();
	push(name + " -> " + ty->name->name + getid() + ";\n");
}

