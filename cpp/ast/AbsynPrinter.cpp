#include "AbsynPrinter.h"
#include "Absyn.h"
#include "debug.h"
#include <cassert>

using namespace std;

void AbsynPrinter::push(const string &s)
{
	stack.push(s);
}

string AbsynPrinter::pop()
{
	assert(!stack.empty());
	string s = stack.top();
	stack.pop();
	return s;
}

void AbsynPrinter::visit(SimpleVar *var)
{
	string sresult;
	sresult = "SimpleVar(Symbol(\"" + var->sym->name + "\"))";
	push(sresult);
}

void AbsynPrinter::visit(FieldVar *var)
{
	string svar, sresult;
	var->var->accept(this);
	svar = pop();
	sresult = "FieldVar(" + svar + ", Symbol(\"" + var->sym->name + "\"))";
	push(sresult);
}

void AbsynPrinter::visit(SubscriptVar *var)
{
	string svar, sexp, sresult;
	var->var->accept(this);
	var->exp->accept(this);
	sexp = pop();
	svar = pop();
	sresult = "SubscriptVar(" + svar + "," +  sexp +")";
	push(sresult);
}

void AbsynPrinter::visit(VarExp *exp)
{
	string svar, sresult;
	exp->var->accept(this);
	svar = pop();
	sresult = "VarExp(" + svar + ")";
	push(sresult);
}

void AbsynPrinter::visit(NilExp *exp)
{
	push("NilExp()");
}

void AbsynPrinter::visit(IntExp *exp)
{
	char buf[32];
	sprintf(buf, "%d", exp->n);
	push("IntExp(" + string(buf) + ")");
}

void AbsynPrinter::visit(StringExp *exp)
{
	DBG("%s", exp->str.c_str());
	push("StringExp(\"" + exp->str + "\")");
}

void AbsynPrinter::visit(CallExp *exp)
{
	string sfunc, sexps, sresult;
	sfunc = exp->func->name;
	ExpList::iterator it = exp->explist->begin();
	if(it != exp->explist->end()) {
		do {
			Exp *e = *it;
			if (e) {
				e->accept(this);
				sexps += pop();
			}
			++it;
			if(it == exp->explist->end()) {
				break;
			} else {
				sexps += ", ";
			}
		} while (true);
	}
	sresult = "CallExp(\"" + sfunc + "\", " + sexps + ")";
	push(sresult);
}

string op2str(Oper op) {
	switch(op) {
	case PlusOp:   return "+";
	case MinusOp:  return "-";
	case TimesOp:  return "*";
	case DivideOp: return "/";
	case EqOp:     return "=";
	case NeOp:     return "<>";
	case GeOp:     return ">=";
	case LeOp:     return "<=";
	case GtOp:     return ">";
	case LtOp:     return "<";
	case AndOp:    return "&";
	case OrOp:     return "|";
	default :      return "";
	}
}

void AbsynPrinter::visit(OpExp *exp)
{
	string sleft, sright, sresult;
	exp->left->accept(this);
	exp->right->accept(this);
	sright = pop();
	sleft = pop();
	sresult = "OpExp(" + sleft + ", " + op2str(exp->op) + ", " + sright + ")";
	push(sresult);
}

void AbsynPrinter::visit(RecordField *field)
{
	string sname, sinit, sresult;
	sname = field->name->name;
	field->init->accept(this);
	sinit = pop();
	sresult = sname + " = " + sinit;
	push(sresult);
}

void AbsynPrinter::visit(RecordExp *exp)
{
	string stype, sfields, sresult;
	RecordFieldList::iterator it = exp->fields->begin();
	if(it != exp->fields->end()) {
		do {
			RecordField *f = *it;
			f->accept(this);
			sfields += pop();
			++it;
			if(it == exp->fields->end()) {
				break;
			} else {
				sfields += ", ";
			}
		} while (true);
	}
	stype = exp->type->name;
	sresult = "RecordExp(\"" + stype + "\", " + sfields + ")";
	push(sresult);
}

void AbsynPrinter::visit(SeqExp *exp)
{
	string sseq, sresult;
	Seq::iterator it = exp->seq->begin();
	if(it != exp->seq->end()) {
		do {
			Exp *e = *it;
			if (e) {
				e->accept(this);
				sseq += pop();
			}
			++it;
			if(it == exp->seq->end()) {
				break;
			} else {
				sseq += "; ";
			}
		} while (true);
	}
	sresult = "SeqExp(" + sseq + ")";
	push(sresult);
}

void AbsynPrinter::visit(AssignExp *exp)
{
	DBG("exp->var = %p, exp->exp = %p", exp->var, exp->exp);
	string svar, sexp, sresult;
	assert(exp->var);
	assert(exp->exp);
	exp->var->accept(this);
	exp->exp->accept(this);
	sexp = pop();
	svar = pop();
	sresult = "AssignExp(" + svar + " := " + sexp + ")";
	push(sresult);
}

void AbsynPrinter::visit(IfExp *exp)
{
	string stest, sthen, selse, sresult;
	exp->test->accept(this);
	stest = pop();
	exp->thenexp->accept(this);
	sthen = pop();
	if(exp->elseexp) {
		exp->elseexp->accept(this);
		selse = pop();
		sresult = "IfExp(" + stest + ", " + sthen + ", " + selse + ")";
	} else {
		sresult = "IfExp(" + stest + ", " + sthen + ")";
	}
	push(sresult);
}

void AbsynPrinter::visit(WhileExp *exp)
{
	string stest, sbody, sresult;
	exp->test->accept(this);
	stest = pop();
	exp->body->accept(this);
	sbody = pop();
	sresult = "WhileExp(" + stest + ", " + sbody + ")";
	push(sresult);
}

void AbsynPrinter::visit(ForExp *exp)
{
	string svar, sescape, slo, shi, sbody, sresult;
	svar = exp->var->name;
	exp->lo->accept(this);
	slo = pop();
	exp->hi->accept(this);
	shi = pop();
	exp->body->accept(this);
	sbody = pop();
	sresult = "ForExp(" + svar + ", " + slo + ", " + shi + ", " + sbody + ")";
	push(sresult);
}

void AbsynPrinter::visit(BreakExp *exp)
{
	push("BreakExp()");
}

void AbsynPrinter::visit(LetExp *exp)
{
	string sdecs, sbody, sresult;
	DecList::iterator it = exp->decs->begin();
	if(it != exp->decs->end()) {
		do {
			Dec *d = *it;
			d->accept(this);
			sdecs += pop();
			++it;
			if(it == exp->decs->end()) {
				break;
			} else {
				sdecs += ", ";
			}
		} while (true);
	}
	exp->body->accept(this);
	sbody = pop();
	sresult = "LetExp(" + sdecs + ": " + sbody + ")";
	push(sresult);
}

void AbsynPrinter::visit(ArrayExp *exp)
{
	string stype, ssize, sinit, sresult;
	stype = exp->type->name;
	exp->size->accept(this);
	ssize = pop();
	exp->init->accept(this);
	sinit = pop();
	sresult = "ArrayExp(" + stype + ", " + ssize + ", " + sinit + ")";
	push(sresult);
}

void AbsynPrinter::visit(TypeField *field)
{
	push("TypeField(" + field->name->name + " : " + field->type->name + ")");
}

void AbsynPrinter::visit(FunDec *dec)
{
	string sname, sparams, srettype, sbody, sresult;
	sname = dec->name->name;
	TypeFieldList::iterator it = dec->params->begin();
	if(it != dec->params->end()) {
		do {
			TypeField *f = *it;
			f->accept(this);
			sparams += pop();
			++it;
			if(it == dec->params->end()) {
				break;
			} else {
				sparams += ", ";
			}
		} while (true);
	}
	srettype = dec->result->name;
	dec->body->accept(this);
	sbody = pop();
	sresult = "FunDec(" + sname + ", " + sparams + ", " + srettype + ", " + sbody + ")";
	push(sresult);
}

void AbsynPrinter::visit(VarDec *dec)
{
	string sname, stype, sinit, sresult;
	sname = dec->name->name;
	stype = dec->type->name;
	dec->init->accept(this);
	sinit = pop();
	sresult = "VarDec(" + sname + ", " + stype + ", " + sinit + ")";
	push(sresult);
}

void AbsynPrinter::visit(TypeDec *dec)
{
	string sname, sty, sresult;
	sname = dec->name->name;
	dec->ty->accept(this);
	sty = pop();
	sresult = "TypeDec(" + sname + ", " + sty + ")";
	push(sresult);
}

void AbsynPrinter::visit(NameTy *ty)
{
	push("NameTy(\"" + ty->name->name + "\")");
}

void AbsynPrinter::visit(RecordTy *ty)
{
	string sfields, sresult;
	TypeFieldList::iterator it = ty->fields->begin();
	if(it != ty->fields->end()) {
		do {
			TypeField *f = *it;
			f->accept(this);
			sfields += pop();
			++it;
			if(it == ty->fields->end()) {
				break;
			} else {
				sfields += ", ";
			}
		} while (true);
	}
	sresult = "RecordTy(" + sfields + ")";
	push(sresult);
}

void AbsynPrinter::visit(ArrayTy *ty)
{
	push("ArrayTy(\"" + ty->name->name + "\")");
}

