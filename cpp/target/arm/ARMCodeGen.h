#ifndef ARM_CODE_GEN_H
#define ARM_CODE_GEN_H

#include "Tree.h"
#include <list>

class Temp;

namespace assem {
class Instruction;
typedef std::list<Instruction*> InstructionList;
}

class ARMCodeGen
{
 public:
	ARMCodeGen();

	void generate(tree::Stm *s, assem::InstructionList &instList);
	void munchStm(tree::Stm *s);
	void munchSEQ(tree::Stm *l, tree::Stm *r);
	void munchMOVE(tree::Exp *dst, tree::Exp *src);
	void munchLABEL(Label *lab);
	void munchJUMP(Label *lab);
	void munchCJUMP(tree::CJUMP *cj);
	void munchEXPR(tree::Exp *exp);
	void munchArgs(const tree::ExpList &exp);
	Temp *munchExp(tree::Exp *e);
	Temp *munchMEM(tree::MEM *m);
	Temp *munchBINOP(tree::BINOP *b);
	Temp *munchCONST(tree::CONST *c);
	Temp *munchTEMP(tree::TEMP *t);

 private:
	void emit(assem::Instruction *inst);
	assem::InstructionList ilist;
};


#endif //ARM_CODE_GEN_H
