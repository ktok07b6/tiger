#ifndef CODE_GEN_H
#define CODE_GEN_H

#include "Tree.h"
#include <list>
#include "Temp.h"

namespace assem {

class Instruction;
typedef std::list<Instruction*> InstructionList;

class CodeGen
{
 public:
	static CodeGen *create();
	virtual ~CodeGen() {}

	void generate(tree::Stm *s, assem::InstructionList &instList);
	void munchStm(tree::Stm *s);
	void munchSEQ(tree::Stm *l, tree::Stm *r);
	Temp *munchExp(tree::Exp *e);

	virtual void munchMOVE(tree::Exp *dst, tree::Exp *src) = 0;
	virtual void munchLABEL(Label *lab) = 0;
	virtual void munchJUMP(Label *lab) = 0;
	virtual void munchCJUMP(tree::CJUMP *cj) = 0;
	virtual void munchEXPR(tree::Exp *exp) = 0;
	virtual void munchArgs(const tree::ExpList &exps, TempList *tsrc, TempList *tdst) = 0;

	virtual Temp *munchMEM(tree::MEM *m) = 0;
	virtual Temp *munchBINOP(tree::BINOP *b) = 0;
	virtual Temp *munchCALL(tree::CALL *c) = 0;
	virtual Temp *munchCONST(tree::CONST *c) = 0;
	virtual Temp *munchTEMP(tree::TEMP *t) = 0;

	virtual bool isMove(Instruction *inst) = 0;
	virtual bool isJump(Instruction *inst) = 0;
protected:
	CodeGen();
	void emit(Instruction *inst);
	InstructionList ilist;
};

}//namespace assem
#endif //CODE_GEN_H
