#ifndef _ABSYN_VISITOR_H_
#define _ABSYN_VISITOR_H_

struct SimpleVar;
struct FieldVar;
struct SubscriptVar;
struct VarExp;
struct NilExp;
struct IntExp;
struct StringExp;
struct CallExp;
struct OpExp;
struct RecordField;
struct RecordExp;
struct SeqExp;
struct AssignExp;
struct IfExp;
struct WhileExp;
struct ForExp;
struct BreakExp;
struct LetExp;
struct ArrayExp;
struct TypeField;
struct FunDec;
struct VarDec;
struct TypeDec;
struct NameTy;
struct RecordTy;
struct ArrayTy;

struct AbsynVisitor
{
	virtual ~AbsynVisitor() {}

	virtual void visit(SimpleVar *exp)=0;
	virtual void visit(FieldVar *exp)=0;
	virtual void visit(SubscriptVar *exp)=0;
	virtual void visit(VarExp *exp)=0;
	virtual void visit(NilExp *exp)=0;
	virtual void visit(IntExp *exp)=0;
	virtual void visit(StringExp *exp)=0;
	virtual void visit(CallExp *exp)=0;
	virtual void visit(OpExp *exp)=0;
	virtual void visit(RecordField *exp)=0;
	virtual void visit(RecordExp *exp)=0;
	virtual void visit(SeqExp *exp)=0;
	virtual void visit(AssignExp *exp)=0;
	virtual void visit(IfExp *exp)=0;
	virtual void visit(WhileExp *exp)=0;
	virtual void visit(ForExp *exp)=0;
	virtual void visit(BreakExp *exp)=0;
	virtual void visit(LetExp *exp)=0;
	virtual void visit(ArrayExp *exp)=0;
	virtual void visit(TypeField *exp)=0;
	virtual void visit(FunDec *exp)=0;
	virtual void visit(VarDec *exp)=0;
	virtual void visit(TypeDec *exp)=0;
	virtual void visit(NameTy *exp)=0;
	virtual void visit(RecordTy *exp)=0;
	virtual void visit(ArrayTy *exp)=0;
};


#endif //_ABSYN_VISITOR_H_
