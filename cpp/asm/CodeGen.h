#ifndef CODE_GEN_H
#define CODE_GEN_H

#include "Tree.h"
#include <list>

class Temp;

namespace assem {

class Instruction;
typedef std::list<Instruction*> InstructionList;

class CodeGen
{
 public:
	CodeGen();

	void generate(tree::Stm *s, InstructionList &instList);
	void munchStm(tree::Stm *s);
	void munchSEQ(tree::Stm *l, tree::Stm *r);
	void munchMOVE(tree::Exp *dst, tree::Exp *src);
	void munchLABEL(Label *lab);
	Temp *munchExp(tree::Exp *e);

 private:
	void emit(Instruction *inst);
	InstructionList ilist;
};

}//namespace assem
#endif //CODE_GEN_H
