#ifndef FIND_ESCAPE_H
#define FIND_ESCAPE_H

#include "AbsynVisitor.h"

#include <vector>
#include <map>

struct Symbol;

class FindEscape : public AbsynVisitor
{
public:
	FindEscape();
	~FindEscape();

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
	typedef std::map<Symbol *, bool *> VarScope;
	typedef std::vector<VarScope*> VarScopeStack;

	void beginStackFrame();
	void endStackFrame();
	void addSymbolToScope(Symbol *sym, bool *escape);
	void checkEscape(Symbol *s);
	void findCheckEscape(Symbol *s);

	VarScopeStack varScopeStack;
};
#endif //FIND_ESCAPE_H

