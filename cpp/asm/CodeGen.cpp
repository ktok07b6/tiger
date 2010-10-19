#include "CodeGen.h"
#include "Temp.h"
#include "Instruction.h"

using namespace std;

namespace assem {

CodeGen::CodeGen()
{
}

void 
CodeGen::generate(tree::Stm *s, InstructionList &instList)
{
	munchStm(s);
	instList = ilist;
	ilist.clear();
}

void 
CodeGen::munchStm(tree::Stm *s)
{
	if (s->isSEQ_T()) {
		tree::SEQ *seq = (tree::SEQ*)s;
		munchSEQ(seq->l, seq->r);
	} else if (s->isMOVE_T()) {
		tree::MOVE *mv = (tree::MOVE*)s;
		munchMOVE(mv->dst, mv->src);
	} else if (s->isLABEL_T()) {
		tree::LABEL *l = (tree::LABEL*)s;
		munchLABEL(l->label);
	} else {
	}
}

void
CodeGen::munchSEQ(tree::Stm *a, tree::Stm *b)
{
	munchStm(a); 
	munchStm(b);
}
void
CodeGen::munchMOVE(tree::Exp *dst, tree::Exp *src)
{
	if (dst->isMEM_T()) {
		tree::MEM *mem = (tree::MEM*)dst;
		if (mem->exp->isBINOP_T()) {
			tree::BINOP *binop = (tree::BINOP*)mem->exp;
			if (binop->op == tree::BINOP::oPLUS) {
				if (binop->r->isCONST_T()) {
					//(MEM(BINOP(PLUS,e1,CONST(i))), e2)
					/*
					  emit(new OPER("STORE M['s0+" + i + "] <- 's1\n",
					  null, L(munchExp(e1), L(munchExp(e2), null))));
					*/
					int i = ((tree::CONST*)binop->r)->value;
					char buf [32];
					sprintf(buf, "STORE M['s0+%d] <- 's1\n", i);
					string assem = buf;
					TempList tl;
					tl.push_back(munchExp(binop->l));
					tl.push_back(munchExp(src));
					emit(gcnew(OPER, (assem, TempList(), tl)));
					return;
				} else if(binop->l->isCONST_T()) {
					//MEM(BINOP(PLUS,CONST(i),e1)),e2))
					/*
					  emit(new OPER("STORE M['s0+" + i + "] <- 's1\n",
					  null, L(munchExp(e1), L(munchExp(e2), null))));
					*/
					int i = ((tree::CONST*)binop->l)->value;
					char buf [32];
					sprintf(buf, "STORE M['s0+%d] <- 's1\n", i);
					string assem = buf;
					TempList tl;
					tl.push_back(munchExp(binop->r));
					tl.push_back(munchExp(src));
					emit(gcnew(OPER, (assem, TempList(), tl)));
					return;
				}
			} 
			//... TODO
		} 

		if (src->isMEM_T()) {
			//(MEM(e1),MEM(e2))
			/*
			  emit(new OPER("MOVE M['s0] <- M['s1]\n",
			  null, L(munchExp(e1), L(munchExp(e2), null))));
			*/
			return;
		}

		if (mem->exp->isCONST_T()) {
			
			//(MEM(CONST(i)),e2)
			/*
			  emit(new OPER("STORE M[r0+" + i + "] <- 's0\n",
			  null, L(munchExp(e2), null)));
			*/
			return;
		} 

		{
			//(MEM(e1),e2)
			string assem = "STORE M['s0] <- 's1\n";
			TempList tl;
			tl.push_back(munchExp(mem->exp));
			tl.push_back(munchExp(src));
			emit(gcnew(OPER, (assem, TempList(), tl)));
			return;
		}

	} 

	if (dst->isTEMP_T()) {

		//(TEMP(i), e2)
		/*
	  emit(new OPER("ADD 'd0 <- 's0 + r0\n",
	  L(i,null), L(munchExp(e2), null)));
		*/
		return;
	}

	//TODO:
}

void
CodeGen::munchLABEL(Label *lab)
{
	emit(new LABEL(lab->toString() + ":\n", lab));
}

Temp *
CodeGen::munchExp(tree::Exp *e)
{
}

void 
CodeGen::emit(Instruction *inst)
{
	ilist.push_back(inst);
}

} //namespace assem
