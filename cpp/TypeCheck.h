#ifndef TYPE_CHECK_H
#define TYPE_CHECK_H

#include "AbsynVisitor.h"
#include <stack>
#include <string>

struct Type;

struct TypeCheck : AbsynVisitor
{
	TypeCheck();
	bool isAccepted();
	void push(Type *t);
	Type *pop();
	std::stack<Type*> stack;
	void error(const std::string &);

	virtual void visit(SimpleVar *exp);
	virtual void visit(FieldVar *exp);
	virtual void visit(SubscriptVar *exp);
	virtual void visit(VarExp *exp);
	virtual void visit(NilExp *exp);
	virtual void visit(IntExp *exp);
	virtual void visit(StringExp *exp);
	virtual void visit(CallExp *exp);
	virtual void visit(OpExp *exp);
	virtual void visit(RecordField *exp);
	virtual void visit(RecordExp *exp);
	virtual void visit(SeqExp *exp);
	virtual void visit(AssignExp *exp);
	virtual void visit(IfExp *exp);
	virtual void visit(WhileExp *exp);
	virtual void visit(ForExp *exp);
	virtual void visit(BreakExp *exp);
	virtual void visit(LetExp *exp);
	virtual void visit(ArrayExp *exp);
	virtual void visit(TypeField *exp);
	virtual void visit(FunDec *exp);
	virtual void visit(VarDec *exp);
	virtual void visit(TypeDec *exp);
	virtual void visit(NameTy *exp);
	virtual void visit(RecordTy *exp);
	virtual void visit(ArrayTy *exp);
 private:
	void visit2(FunDec *dec);
	bool accept;
};

#endif //TYPE_CHECK_H
