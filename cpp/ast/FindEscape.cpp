#include "FindEscape.h"
#include "debug.h"
#include "Symbol.h"
#include "Absyn.h"

FindEscape::FindEscape()
{
	beginStackFrame();
}
FindEscape::~FindEscape()
{
	endStackFrame();
}

void
FindEscape::visit(SimpleVar *var)
{
	checkEscape(var->sym);
}

void
FindEscape::visit(FieldVar *var)
{
	var->var->accept(this);
}

void
FindEscape::visit(SubscriptVar *var)
{
	var->var->accept(this);
	var->exp->accept(this);
}

void
FindEscape::visit(VarExp *exp)
{
	exp->var->accept(this);
}

void
FindEscape::visit(NilExp *exp)
{
}

void
FindEscape::visit(IntExp *exp)
{
}

void
FindEscape::visit(StringExp *exp)
{
}

void
FindEscape::visit(CallExp *exp)
{
	ExpList::iterator it;
	it = exp->explist->begin();
	while (it != exp->explist->end()) {
		Exp *e = *it;
		e->accept(this);
		++it;
	}
}

void
FindEscape::visit(OpExp *exp)
{
	exp->left->accept(this);
	exp->right->accept(this);
}

void
FindEscape::visit(RecordField *field)
{
	field->init->accept(this);
}

void
FindEscape::visit(RecordExp *exp)
{
	RecordFieldList::iterator it;
	it = exp->fields->begin();
	while (it != exp->fields->end()) {
		RecordField *f = *it;
		f->accept(this);
		++it;
	}
}

void
FindEscape::visit(SeqExp *exp)
{
	Seq::iterator it;
	it = exp->seq->begin();
	while (it != exp->seq->end()) {
		Exp *e = *it;
		e->accept(this);
		++it;
	}
}

void
FindEscape::visit(AssignExp *exp)
{
	exp->var->accept(this);
	exp->exp->accept(this);
}

void
FindEscape::visit(IfExp *exp)
{
	exp->test->accept(this);
	exp->thenexp->accept(this);
	exp->elseexp->accept(this);
}

void
FindEscape::visit(WhileExp *exp)
{
	exp->test->accept(this);
	exp->body->accept(this);
}

void
FindEscape::visit(ForExp *exp)
{
	addSymbolToScope(exp->var, &exp->escape);
	exp->lo->accept(this);
	exp->hi->accept(this);
	exp->body->accept(this);
}

void
FindEscape::visit(BreakExp *exp)
{
}

void
FindEscape::visit(LetExp *exp)
{
	DecList::iterator it;
	it = exp->decs->begin();
	while (it != exp->decs->end()) {
		Dec *d = *it;
		d->accept(this);
		++it;
	}

	exp->body->accept(this);
}

void
FindEscape::visit(ArrayExp *exp)
{
	exp->size->accept(this);
	exp->init->accept(this);
}

void
FindEscape::visit(TypeField *field)
{
	addSymbolToScope(field->name, &field->escape);
}

void
FindEscape::visit(FunDec *dec)
{
	beginStackFrame();

	TypeFieldList::iterator it;
	it = dec->params->begin();
	while (it != dec->params->end()) {
		TypeField *f = *it;
		f->accept(this);
		++it;
	}
	dec->body->accept(this);
	
	endStackFrame();
}

void
FindEscape::visit(VarDec *dec)
{
	addSymbolToScope(dec->name, &dec->escape);
	dec->init->accept(this);
}

void
FindEscape::visit(TypeDec *dec)
{
	dec->ty->accept(this);
}

void
FindEscape::visit(NameTy *ty)
{
}

void
FindEscape::visit(RecordTy *ty)
{
	TypeFieldList::iterator it;
	it = ty->fields->begin();
	while (it != ty->fields->end()) {
		TypeField *f = *it;
		f->accept(this);
		++it;
	}
}

void
FindEscape::visit(ArrayTy *ty)
{
}


void 
FindEscape::beginStackFrame()
{
	VarScope *newScope = new VarScope();
	varScopeStack.push_back(newScope);
}

void 
FindEscape::endStackFrame()
{
	assert(!varScopeStack.empty());
	VarScope *scope = varScopeStack.back();
	varScopeStack.pop_back();
	delete scope;
}

void 
FindEscape::addSymbolToScope(Symbol *sym, bool *escape)
{
	assert(!varScopeStack.empty());
	VarScope *scope = varScopeStack.back();
	scope->insert(std::make_pair(sym, escape));
}

void 
FindEscape::checkEscape(Symbol *sym)
{
	assert(!varScopeStack.empty());
	VarScope *scope = varScopeStack.back();
	VarScope::iterator it = scope->find(sym);
	if (it == scope->end()) {
		findCheckEscape(sym);
	}
}

void 
FindEscape::findCheckEscape(Symbol *sym)
{
	VarScopeStack::reverse_iterator sco = varScopeStack.rbegin();  
	while (sco != varScopeStack.rend()) {
		VarScope *scope = *sco;
		VarScope::iterator it = scope->find(sym);
		if (it != scope->end()) {
			bool *escape = it->second;
			*escape = true;
			DBG(">>>>>>>> %s is escaped <<<<<<<<", sym->name.c_str());
		}
		++sco;
	}
}



