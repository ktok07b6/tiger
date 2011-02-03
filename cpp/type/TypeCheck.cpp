#include "TypeCheck.h"
#include "Absyn.h"
#include "Type.h"
#include "Table.h"
#include "debug.h"
#include <cassert>
#include "NameEntry.h"

using namespace std;

extern Table<Type> typeTable;
extern Table<NameEntry> nameTable;
extern Scopes<Type> typeScopes;
extern Scopes<NameEntry> nameScopes;

TypeCheck::TypeCheck()
	: accept(true)
{
}

bool TypeCheck::isAccepted()
{
	return accept;
}

void TypeCheck::push(Type *t)
{
	assert(t);
	stack.push(t);
}

Type *TypeCheck::pop()
{
	assert(!stack.empty());
	Type *t = stack.top();
	stack.pop();
	return t;
}

void TypeCheck::error(const string &msg)
{
	accept = false;
	fprintf(stderr, msg.c_str());
	abort();
	//throw msg;
}

void TypeCheck::visit(SimpleVar *var)
{
	FUNCLOG;
	NameEntry *e = nameTable.get(var->getSymbol());
	assert(e);
	if (!e->isVariable()) {
		error("unknown variable");
	}
	
	var->symInfo = static_cast<VarEntry*>(e);
	push(static_cast<VarEntry*>(e)->type);
}

void TypeCheck::visit(FieldVar *var)
{
	FUNCLOG;
	NameEntry *e = nameTable.get(var->getSymbol());
	assert(e);
	if (!e->isVariable()) {
		error("unknown variable");
	}

	var->var->accept(this);
	Type *t = pop();
	if (!t->actual()->isRecordT()) {
		//TODO:
		error("var is not record type");
	}
	RecordT *rec_t = static_cast<RecordT*>(t);
	while(rec_t) {
		if (rec_t->fieldName == var->sym) {
			break;
		}
		rec_t = rec_t->tail;
	}
	if (rec_t == NULL) {
		error(var->sym->name + " is invalid member name");
	}

	var->symInfo = static_cast<VarEntry*>(e);
	push(rec_t->fieldType);
}

void TypeCheck::visit(SubscriptVar *var)
{
	FUNCLOG;
	NameEntry *e = nameTable.get(var->getSymbol());
	assert(e);
	if (!e->isVariable()) {
		error("unknown variable");
	}

	var->exp->accept(this);
	Type *exp_t = pop();
	if (!exp_t->actual()->isIntT()) {
		error("must be Int type");
	}

	var->var->accept(this);
	Type *var_t = pop();
	if (!var_t->actual()->isArrayT()) {
		error("must be Array type");
	}

	var->symInfo = static_cast<VarEntry*>(e);
	push(static_cast<ArrayT*>(var_t)->element);
}

void TypeCheck::visit(VarExp *exp)
{
	FUNCLOG;
	exp->var->accept(this);
	Type *t = pop();
	push(t);
}

void TypeCheck::visit(NilExp *exp)
{
	FUNCLOG;
	push(NilT);
}

void TypeCheck::visit(IntExp *exp)
{
	FUNCLOG;
	push(IntT);
}

void TypeCheck::visit(StringExp *exp)
{
	FUNCLOG;
	push(StrT);
}

void TypeCheck::visit(CallExp *exp)
{
	FUNCLOG;
	NameEntry *entry = nameTable.get(exp->func);
	if (entry == NULL || entry->isVariable()) {
		error("unknown function name");
	}
	FuncEntry *fn = static_cast<FuncEntry*>(entry);
	
	if (exp->explist->size() != fn->params.size()) {
		//DBG("exp->explist->size()=%d, fn->params.size()=%d",exp->explist->size(), fn->params.size());
		error("invalid number of params");
	}
	ExpList::iterator it;
	it = exp->explist->begin();
	vector<Type*>::iterator it2;
	it2 = fn->params.begin();
	while(it != exp->explist->end()) {
		Exp *e = *it;
		e->accept(this);
		Type *var_t = pop();
		Type *param_t = *it2;
		if (!var_t->coerceTo(param_t)) {
			error("function param type missmatch");
		}
		++it;
		++it2;
	}
	exp->fnInfo = fn;
	push(fn->result);
}

void TypeCheck::visit(OpExp *exp)
{
	FUNCLOG;
	exp->left->accept(this);
	exp->l_t = pop();
	exp->right->accept(this);
	exp->r_t = pop();

	assert(exp->l_t);
	assert(exp->r_t);

	if (exp->l_t->coerceTo(exp->r_t)) {
		Type *t;
		if (exp->l_t->actual()->isIntT()) {
			t = IntT;
		} else if (exp->l_t->actual()->isStrT()) {
			if (exp->op == PlusOp) {
				t = StrT;
			} else if (exp->op == EqOp || exp->op == NeOp) {
				t = IntT;
			} else {
				error("such operation to string is not supported");
			}
		} else {
			//pointer equality test
			if (exp->op == EqOp || exp->op == NeOp) {
				t = IntT;
			} else {
				error("invalid operand type");
			}
		}
		push(t);
	} else {
		error("operand type is mismatch");
	}
}

void TypeCheck::visit(RecordField *field)
{
	FUNCLOG;
	field->init->accept(this);
	Type *init_t = pop();

	Type *name_t = typeTable.get(field->name);
	if (!init_t->coerceTo(name_t)) {
		error("assignee type missmatch");
	}

	push(VoidT);
}

void TypeCheck::visit(RecordExp *exp)
{
	FUNCLOG;
	Type *t = typeTable.get(exp->type);
	if (!t->actual()->isRecordT()) {
		error("not record type");
	}
	RecordT *rec_t = static_cast<RecordT*>(t);

	RecordFieldList::iterator it;
	it = exp->fields->begin();
	while(it != exp->fields->end()) {
		RecordField *f = *it;
		if (rec_t->fieldName != f->name) {
			error("invalid fieldname");
		}
		
		f->init->accept(this);
		Type *init_t = pop();
		if (!init_t->coerceTo(rec_t->fieldType)) {
			error("type missmatch");
		}
		++it;
		rec_t = rec_t->tail;
	}
	push(t);
}

void TypeCheck::visit(SeqExp *exp)
{
	FUNCLOG;
	Type *t;
	Seq::iterator it = exp->seq->begin();
	while(it != exp->seq->end()) {
		Exp *e = *it;
		assert(e);
		e->accept(this);
		t = pop();
		++it;
	}
	push(t);
}

void TypeCheck::visit(AssignExp *exp)
{
	FUNCLOG;
	exp->var->accept(this);
	Type *assignee = pop();
	exp->exp->accept(this);
	Type *assigner = pop();

	if (assignee->coerceTo(assigner)) {
		push(VoidT);
	} else {
		error("assignee type missmatch");
	}
}

void TypeCheck::visit(IfExp *exp)
{
	FUNCLOG;
	exp->test->accept(this);
	Type *test_t = pop();
	if (!test_t->actual()->isIntT()) {
		error("int type expected");
	}

	exp->thenexp->accept(this);
	Type *then_t = pop();

	if (exp->elseexp) {
		exp->elseexp->accept(this);
		Type *else_t = pop();
		if (!then_t->actual()->coerceTo(else_t)) {
			error("then and else type missmatch");
		}
		push(then_t->actual());
	} else {
		//if-then exp must return unit
		if (!then_t->actual()->coerceTo(VoidT)) {
			error(" if-then returns non unit");
		}
		push(VoidT);
	}
}

void TypeCheck::visit(WhileExp *exp)
{
	FUNCLOG;
	exp->test->accept(this);
	Type *test_t = pop();
	if (!test_t->actual()->isIntT()) {
		error("int type expected");
	}

	exp->body->accept(this);
	Type *body_t = pop();
	if (!body_t->actual()->isVoidT()) {
		error("body of while not unit");
	}

	push(VoidT);
}

void TypeCheck::visit(ForExp *exp)
{
	FUNCLOG;

	typeTable.beginScope(exp);
	nameTable.beginScope(exp);
	
	NameT *var_t = gcnew(NameT, (exp->var));
	var_t->bind(typeTable.get(Symbol::symbol("int")));
	VarEntry *varEntry = gcnew(VarEntry, (var_t));

	nameTable.put(exp->var, varEntry);
	exp->symInfo = varEntry;

	exp->lo->accept(this);
	Type *lo_t = pop();
	if (!lo_t->actual()->isIntT()) {
		error("int type expected");
	}

	exp->hi->accept(this);
	Type *hi_t = pop();
	if (!hi_t->actual()->isIntT()) {
		error("int type expected");
	}

	exp->body->accept(this);
	Type *body_t = pop();
	(void)body_t;

	typeTable.endScope();
	nameTable.endScope();

	push(VoidT);
}

void TypeCheck::visit(BreakExp *exp)
{
	FUNCLOG;
	push(VoidT);
}

void TypeCheck::visit(LetExp *exp)
{
	FUNCLOG;

	typeTable.beginScope(exp);
	nameTable.beginScope(exp);

	DecList::iterator it = exp->decs->begin();
	while (it != exp->decs->end()) {
		Dec *d = *it;
		d->accept(this);
		Type *dec_t = pop();
		assert(dec_t->actual()->isVoidT());
		++it;
	}
	//check twice
	it = exp->decs->begin();
	while (it != exp->decs->end()) {
		Dec *d = *it;
		if (d->isFundec()) {
			visit2(static_cast<FunDec*>(d));
		}
		++it;
	}

	exp->body->accept(this);
	Type *body_t = pop();
	//TODO check
	
	typeTable.endScope();
	nameTable.endScope();

	push(body_t->actual());
}

void TypeCheck::visit(ArrayExp *exp)
{
	FUNCLOG;
	Type *arr_t = typeTable.get(exp->type);
	
	if (!arr_t->actual()->isArrayT()) {
		error(exp->type->name + " is not array type");
	}
	
	exp->size->accept(this);
	Type *sz_t = pop();
	if (!sz_t->actual()->isIntT()) {
		error("int type expected");
	}
	exp->init->accept(this);
	Type *init_t = pop();
	
	Type *elem_t = static_cast<ArrayT*>(arr_t->actual())->element;
	if (!init_t->coerceTo(elem_t)) {
		error("array type is different");
	}
	push(arr_t);
}

void TypeCheck::visit(TypeField *field)
{
	FUNCLOG;
	Type *t = typeTable.get(field->type);
	//In recursive type case, t is null
	if (t == NULL) {
		//none type
		t = gcnew(NameT, (field->type));
		static_cast<NameT*>(t)->bind(NoneT);
	}
	push(t);
}

void TypeCheck::visit2(FunDec *dec)
{
	FUNCLOG;

	NameEntry *entry = nameTable.get(dec->name);
	assert(entry);
	if (entry->isVariable()) {
		error("");
	}
	FuncEntry *fnEntry = static_cast<FuncEntry*>(entry);

	//TODO: 仮引数の変数名の取得のためにTypeFieldを再度利用しているが、
	//FunEntryのみでまかなうようにする
	typeTable.beginScope(dec);
	nameTable.beginScope(dec);
	TypeFieldList::const_iterator it = dec->params->begin();
	vector<Type*>::const_iterator it2 = fnEntry->params.begin();
	while(it != dec->params->end()) {
		TypeField *f = *it;
		Type *t = *it2;
		assert(t);

		VarEntry *varEntry = gcnew(VarEntry, (t));
		f->symInfo = varEntry;
		nameTable.put(f->name, varEntry);
		++it; 
		++it2;
	}

	dec->body->accept(this);
	Type *actual_res_t = pop();
	if (fnEntry->result) {
		if (!actual_res_t->coerceTo(fnEntry->result)) {
			error("function body result type missmatch");
		}
	}
	
	dec->fnInfo = fnEntry;
	typeTable.endScope();
	nameTable.endScope();
}

void TypeCheck::visit(FunDec *dec)
{
	FUNCLOG;

	//TODO
	//未定義の再帰関数の呼び出しのために
	//bodyの評価はDecList全体の後まで遅延させる(bodyの評価はvisit2(FunDec)で行う)
	//ここではCallExpの処理に必要な仮引数の型と戻り値の型、関数名を登録しておく

	TypeList params;
	TypeFieldList::const_iterator it = dec->params->begin();
	while(it != dec->params->end()) {
		TypeField *f = *it;
		f->accept(this);
		Type *t = pop();
		if (!t) {
			error("unknown type name");
		}
		params.push_back(t);
		++it;
	}

	Type *result = typeTable.get(dec->result);
	if (!result) {
		result = VoidT;
	}
	FuncEntry *fnEntry = gcnew(FuncEntry, (result, params));
	nameTable.put(dec->name, fnEntry);

	push(VoidT);
}

void TypeCheck::visit(VarDec *dec)
{
	FUNCLOG;
	bool auto_type = false;
	Type *t = typeTable.get(dec->type);
	VarEntry *varEntry = gcnew(VarEntry, (t));
	if (t) {
		nameTable.put(dec->name, varEntry);
	} else {
		auto_type = true;
	} 

	dec->init->accept(this);
	Type *init_t = pop();
	
	if (auto_type) {
		//auto type
		varEntry->type = init_t->actual();
		if (varEntry->type->isNilT()) {
			error("initializing nil expressions not constrained by record type");
		}
		nameTable.put(dec->name, varEntry);
	} else {
		//assign type check
		if (!init_t->coerceTo(t)) {
			error("type missmatch");
		}
	}
	dec->symInfo = varEntry;
	push(VoidT);
}

void
replaceNoneRecordType(RecordT *top, RecordT *r, Symbol *name)
{
	while (r) {
		if (r->fieldType->isNameT()) {
			NameT *n = static_cast<NameT*>(r->fieldType);
			//DBG("n->name = %p, name = %p", n->name, name);
			//DBG("n->binding->isNoneT() %d", n->binding->isNoneT());
			if (n->name == name &&
				n->binding->isNoneT()) {
				r->fieldType = top;
				//DBG("recursive ok");
				//DBG("%s", top->toString().c_str());
				break;
			}
		} else if (r->fieldType->isRecordT()) {
			replaceNoneRecordType(top, static_cast<RecordT*>(r->fieldType), name);
		}
		r = r->tail;
	}
}

void TypeCheck::visit(TypeDec *dec)
{
	FUNCLOG;
	dec->ty->accept(this);
	Type *t = pop();

	//DBG("%s", t->toString().c_str());
	//recursive check
	if (t->isRecordT()) {
		replaceNoneRecordType(static_cast<RecordT*>(t), 
							  static_cast<RecordT*>(t), 
							  dec->name);
	}

	typeTable.put(dec->name, t);
	push(VoidT);
}

void TypeCheck::visit(NameTy *ty)
{
	FUNCLOG;
	//ex: ty->name == "int";
	//    typeTable.get("int") == IntT
	NameT *newt = gcnew(NameT, (ty->name));
	Type *t = typeTable.get(ty->name);
	if (!t) {
		error("unknown type name");
	}
	newt->bind(t);
	push(newt);
}

void TypeCheck::visit(RecordTy *ty)
{
	FUNCLOG;
	/*
	if (ty->fields->empty()) {
		error("fields not found");
		return;
	}
	*/
	TypeFieldList::reverse_iterator it = ty->fields->rbegin();
	RecordT *tail = NULL;
	RecordT *rec = NULL;
	while(it != ty->fields->rend()) {
		TypeField *f = *it;
		f->accept(this);
		Type *t = pop();
		//DBG("%s", t->toString().c_str());
		if (!t) {
			error("unknown type name");
		} else if (t->isNoneT()){
			//TODO: recursive record
		}
		rec = gcnew(RecordT, (f->name, t, tail));
		tail = rec;
		++it;
	}
	if (rec) {
		push(rec);
	} else {
		push(gcnew(RecordT, (Symbol::symbol(""), VoidT, NULL)));
	}
}

void TypeCheck::visit(ArrayTy *ty)
{
	FUNCLOG;
	Type *elem_t = typeTable.get(ty->name);
	if (!elem_t) {
		error("unknown type name");
	}
	Type *t = gcnew(ArrayT, (elem_t));
	push(t);
}

