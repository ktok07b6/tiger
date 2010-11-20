#ifndef IR_TRANSLATER_H
#define IR_TRANSLATER_H

#include <string>
#include <stack>
#include "AbsynVisitor.h"
#include "Tree.h"
#include "Exp.h"
#include "Fragment.h"

class Frame;
class Level;
struct Symbol;

struct IRTranslater : AbsynVisitor
{
	IRTranslater(Frame *f);

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

	translate::Exp *getExp();
	const FragmentList &getFragments();
 private:
	void procEntryExit(translate::Exp *exp);
	tree::Exp *convertAndOp(tree::Exp *el, tree::Exp *er);
	tree::Exp *convertOrOp(tree::Exp *el, tree::Exp *er);

	tree::Exp *callMalloc(int);

	translate::Exp *texp;
	Level *currentLevel;
	Level *topLevel;
	Symbol *currentFuncName;
	Label *currentLoopExit;
	FragmentList fragments;
};

#endif //IR_TRANSLATER_H
