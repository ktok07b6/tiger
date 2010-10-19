#ifndef FRAME_H
#define FRAME_H

#include "Object.h"
#include "Absyn.h"
#include "Temp.h"
#include "TempMap.h"
#include "Tree.h"
#include <vector>
#include <string>

class Stm;
class InstrList;
class Label;
class Proc;

class Frame : public Object, public TempMap
{
 public:
	virtual ~Frame() {}
	static Frame *newFrame(Symbol *name, const std::vector<int> &formals);

	class Access : public Object
	{
	public:
		virtual ~Access() {}
		virtual std::string toString() = 0;
		virtual tree::Exp *exp(tree::Exp *fp) = 0;
	};

	virtual Access *allocLocal(bool escape) = 0;
	virtual Temp *fp() = 0;
	virtual int wordSize() = 0;
	virtual tree::Exp *externalCall(const std::string &func, const tree::ExpList &args) = 0;
	virtual Temp *rv() = 0;
	virtual tree::Stm *procEntryExit1(tree::Stm *body) = 0;
	virtual std::string string(Label *label, const std::string &value) = 0;
	virtual tree::Exp *staticChain(tree::Exp *fp) = 0;
	virtual Label *badPtr() = 0;
	virtual Label *badSub() = 0;
	virtual InstrList *procEntryExit2(InstrList *body) = 0;
	virtual Proc *procEntryExit3(InstrList *body) = 0;
	virtual InstrList *codegen(tree::Stm *stm) = 0;
	virtual TempList *registers() = 0;
	Symbol *name;
	std::vector<Access*> formals;	
};

#endif //FRAME_H
