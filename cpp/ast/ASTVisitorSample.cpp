
void
ASTVisitorSample::visit(SimpleVar *var)
{
	var->sym->name;
}

void
ASTVisitorSample::visit(FieldVar *var)
{
	var->var->accept(this);
	var->sym->name;
}

void
ASTVisitorSample::visit(SubscriptVar *var)
{
	var->var->accept(this);
	var->exp->accept(this);
}

void
ASTVisitorSample::visit(VarExp *exp)
{
	exp->var->accept(this);
}

void
ASTVisitorSample::visit(NilExp *exp)
{
}

void
ASTVisitorSample::visit(IntExp *exp)
{
	exp->n;
}

void
ASTVisitorSample::visit(StringExp *exp)
{
	exp->str;
}

void
ASTVisitorSample::visit(CallExp *exp)
{
	exp->func->name;

	ExpList::iterator it;
	it = exp->explist->begin();
	while (it != exp->explist->end()) {
		Exp *e = *it;
		e->accept(this);
		++it;
	}
}

void
ASTVisitorSample::visit(OpExp *exp)
{
	exp->op;
	exp->left->accept(this);
	exp->right->accept(this);
}

void
ASTVisitorSample::visit(RecordField *field)
{
	field->name->name;
	field->init->accept(this);
}

void
ASTVisitorSample::visit(RecordExp *exp)
{
	exp->type->name;

	RecordFieldList::iterator it;
	it = exp->fields->begin();
	while (it != exp->fields->end()) {
		RecordField *f = *it;
		f->accept(this);
		++it;
	}
	
}

void
ASTVisitorSample::visit(SeqExp *exp)
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
ASTVisitorSample::visit(AssignExp *exp)
{
	exp->var->accept(this);
	exp->exp->accept(this);
}

void
ASTVisitorSample::visit(IfExp *exp)
{
	exp->test->accept(this);
	exp->thenexp->accept(this);
	exp->elseexp->accept(this);
}

void
ASTVisitorSample::visit(WhileExp *exp)
{
	exp->test->accept(this);
	exp->body->accept(this);
}

void
ASTVisitorSample::visit(ForExp *exp)
{
	exp->var->name;
	exp->escape;
	exp->lo->accept(this);
	exp->hi->accept(this);
	exp->body->accept(this);
}

void
ASTVisitorSample::visit(BreakExp *exp)
{
}

void
ASTVisitorSample::visit(LetExp *exp)
{
	DecList::iterator it;
	it = exp->decs->begin();
	while (it != exp->decs->end()) {
		Dec *d = *it;
		++it;
	}

	exp->body->accept(this);
}

void
ASTVisitorSample::visit(ArrayExp *exp)
{
	exp->type->name;
	exp->size->accept(this);
	exp->init->accept(this);
}

void
ASTVisitorSample::visit(TypeField *field)
{
	field->name->name;
	field->escape;
	field->type->name;
}

void
ASTVisitorSample::visit(FunDec *dec)
{
	dec->name->name;
	dec->result->name;

	TypeFieldList::iterator it;
	it = dec->params->begin();
	while (it != dec->params->end()) {
		TypeField *f = *it;
		f->accept(this);
		++it;
	}
	dec->body->accept(this);

}

void
ASTVisitorSample::visit(VarDec *dec)
{
	dec->name->name;
	dec->escape;
	dec->type->name;
	dec->init->accept(this);
}

void
ASTVisitorSample::visit(TypeDec *dec)
{
	dec->name->name;
	dec->ty->accept(this);
}

void
ASTVisitorSample::visit(NameTy *ty)
{
	ty->name->name;
}

void
ASTVisitorSample::visit(RecordTy *ty)
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
ASTVisitorSample::visit(ArrayTy *ty)
{
	ty->name->name;
}

