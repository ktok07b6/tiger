#ifndef TYPE_H
#define TYPE_H

#include "Object.h"
#include <cassert>
#include "Symbol.h"
#include "debug.h"

struct Type : public Object
{
Type() : id(NONE_T) {}
	virtual ~Type(){}
	virtual Type *actual() { return this; }
	virtual bool coerceTo(Type *t) {return false;}
	virtual std::string toString() {return "None Type";}

	enum TypeID{
		NONE_T,
		INT_T,
		STR_T,
		NAME_T,
		NIL_T,
		VOID_T,
		ARRAY_T,
		RECORD_T
	};
	TypeID id;
	bool isNoneT();
	bool isIntT();
	bool isStrT();
	bool isNameT();
	bool isVoidT();
	bool isNilT();
	bool isArrayT();
	bool isRecordT();
};

inline bool Type::isNoneT()
{
	return id == NONE_T;
}
inline bool Type::isIntT()
{
	return id == INT_T;
}
inline bool Type::isStrT()
{
	return id == STR_T;
}
inline bool Type::isNameT()
{
	return id == NAME_T;
}
inline bool Type::isVoidT()
{
	return id == VOID_T;
}
inline bool Type::isNilT()
{
	return id == NIL_T;
}
inline bool Type::isArrayT()
{
	return id == ARRAY_T;
}
inline bool Type::isRecordT()
{
	return id == RECORD_T;
}

struct IntT : Type 
{
	IntT() {
		id = INT_T;
	}
	bool coerceTo(Type *t) {
		return (t->actual()->isIntT());
	}
	std::string toString() {
		return "IntT";
	}
};

struct StrT : Type 
{
	StrT() {
		id = STR_T;
	}
	bool coerceTo(Type *t) {
		return (t->actual()->isStrT());
	}
	std::string toString() {
		return "StrT";
	}
};

struct NameT : Type 
{
	Symbol *name;
	Type *binding;
 NameT(Symbol *n) 
	 : name(n)
	 , binding(NULL) {
		id = NAME_T;
	}
   
	bool isLoop() {
		Type *b = binding; 
		bool any;
		binding = NULL;
		if (b == NULL) {
			any = true;
		} else if (b->isNameT()) {
			any = ((NameT*)b)->isLoop();
		} else {
			any = false;
		}
		binding = b;
		return any;
	}
     
	Type *actual() {
		return binding->actual();
	}

	bool coerceTo(Type *t) {
		assert(t);
		Type *a = this->actual();
		assert(a);
		return a->coerceTo(t);
	}
	std::string toString() {
		return "NameT("+ name->name + ")->" + actual()->toString();
	}

	void bind(Type *t) {
		binding = t;
	}
};

struct NilT : Type 
{
	NilT() { 
		id = NIL_T; 
	}
	bool coerceTo(Type *t) {
	    Type *a = t->actual();
	    return (a->isRecordT()) || (a->isNilT());
	}
	std::string toString() {
		return "NilT";
	}
};

struct VoidT : Type 
{
	VoidT() { 
		id = VOID_T;
	}
	bool coerceTo(Type *t) {
		return (t->actual()->isVoidT());
	}
	std::string toString() {
		return "VoidT";
	}

};

struct ArrayT : Type 
{
 ArrayT(Type *e) 
	 : element(e) {
		id = ARRAY_T; 
	}

	Type *element;
	bool coerceTo(Type *t) {
		return this == t->actual();
	}
	std::string toString() {
		return "ArrayT->" + element->toString();
	}
};

struct RecordT : Type 
{
	Symbol *fieldName;
	Type *fieldType;
	RecordT *tail;

	RecordT(Symbol *n, Type *t, RecordT *x) 
		: fieldName(n)
		, fieldType(t)
		, tail(x) { 
		id = RECORD_T; 
	}

	bool coerceTo(Type *t) {
		Type *a = t->actual();
		if (a->isNilT()) {
			return true;
		} else {
			return this == t->actual();
		}
	}

	std::string toString() {
		return toStringRec(this);
	}
 private:
	std::string toStringRec(RecordT *head) {
		assert(fieldName);
		assert(fieldType);
		std::string s;
		//DBG("fieldType %p", fieldType);
		//DBG("head %p", head);
		if (fieldType == head) {
			s =	"RecordT(" + 
				fieldName->name + 
				":(this))";
		} else {
			s =	"RecordT(" + 
				fieldName->name + 
				":";
			if (fieldType->isRecordT()) {
				s += ((RecordT*)fieldType)->toStringRec(head) + 
					")";
			} else {
				s += fieldType->toString() + 
					")";
			}
			if (tail) {
				s +=  "->" + tail->toStringRec(head);
			}
		}
		return s;

	}
};
   
extern Type *NoneT;
extern IntT *IntT;
extern StrT *StrT;
extern NilT *NilT;
extern VoidT *VoidT;
#include <vector>
typedef std::vector<Type *> TypeList;

#endif //TYPE_H
