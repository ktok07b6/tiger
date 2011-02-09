#ifndef ARM_CODE_GEN_H
#define ARM_CODE_GEN_H

#include "Tree.h"
#include "CodeGen.h"
class ARMFrame;

class ARMCodeGen : public assem::CodeGen
{
public:
	ARMCodeGen(ARMFrame *frame);

	virtual bool isMove(assem::Instruction *inst);
	virtual bool isJump(assem::Instruction *inst);

protected:
	virtual void munchMOVE(tree::Exp *dst, tree::Exp *src);
	virtual void munchLABEL(Label *lab);
	virtual void munchJUMP(Label *lab);
	virtual void munchCJUMP(tree::CJUMP *cj);
	virtual void munchEXPR(tree::Exp *exp);
	virtual void munchArgs(const tree::ExpList &exps, TempList *tsrc);

	virtual Temp *munchMEM(tree::MEM *m);
	virtual Temp *munchBINOP(tree::BINOP *b);
	virtual Temp *munchCALL(tree::CALL *c);
	virtual Temp *munchCONST(tree::CONST *c);
	virtual Temp *munchTEMP(tree::TEMP *t);
	virtual Temp *munchNAME(tree::NAME *n);
private:
	Temp *munchBINOP_PLUS(tree::BINOP *b);
	Temp *munchBINOP_MINUS(tree::BINOP *b);
	Temp *munchBINOP_MUL(tree::BINOP *b);
	Temp *munchBINOP_DIV(tree::BINOP *b);
	ARMFrame *frame;
};


#endif //ARM_CODE_GEN_H
