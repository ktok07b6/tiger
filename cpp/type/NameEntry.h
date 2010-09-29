#ifndef NAME_ENTRY_H
#define NAME_ENTRY_H

#include <string>
#include "Type.h"
#include "Level.h"

struct NameEntry : public Object
{
 NameEntry(int ntype) 
	 : ntype(ntype)
		{}
	virtual ~NameEntry() {}
	enum {
		VAR_ENTRY,
		FUNC_ENTRY
	};
	const int ntype;
	bool isVariable();
	bool isFunction();
	virtual std::string toString() = 0;
};

inline bool NameEntry::isVariable() 
{
	return ntype == VAR_ENTRY;
}
inline bool NameEntry::isFunction()
{
	return ntype == FUNC_ENTRY;
}

struct VarEntry : NameEntry
{
 VarEntry(Type *t) 
	 : NameEntry(VAR_ENTRY), type(t), access(NULL) 
		{}
	std::string toString() {
		assert(type);
		if (access) {
			return "VarEntry("+ type->toString() + ", " + access->toString() + ")";
		} else {
			return "VarEntry("+ type->toString() + ")";
		}
	}
	Type *type;
	Level::Access *access;
};

struct FuncEntry : NameEntry
{
 FuncEntry(Type *res, const TypeList &params) 
	 : NameEntry(FUNC_ENTRY), result(res), params(params), level(NULL) 
		{}
	std::string toString() {
		assert(result);
		std::string param_str;
		TypeList::iterator it =  params.begin();
		while(it != params.end()) {
			Type *t = *it;
			param_str += t->toString();
			++it;
			if (it != params.end()) {
				param_str += ", ";
			} else {
				break;
			}
		}
		if (result) {
			return "FuncEntry(" + param_str + "):" + result->toString();
		} else {
			return "FuncEntry(" + param_str + ")";
		}
	}
	Type *result;
	TypeList params;
	Level *level;
};



#endif //NAME_ENTRY_H
