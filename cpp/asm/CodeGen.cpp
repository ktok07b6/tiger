#include "CodeGen.h"
#include "Temp.h"
#include "Instruction.h"
#include "TreeMatcher.h"

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
#if 1
	tree::MEM *mem1;
	tree::MEM *mem2;
	tree::BINOP *binop;
	tree::CONST *konst;
	tree::TEMP *temp;

	if (_M1(mem1, _M0(binop)) == dst) {
		if (binop->op == tree::BINOP::oPLUS) {
			if (_M0(konst) == binop->r) {
				//(MEM(BINOP(PLUS,e1,CONST(i))), e2)
				char buf [32];
				sprintf(buf, "STORE M['s0+%d] <- 's1\n", konst->value);
				string assem = buf;
				TempList tl;
				tl.push_back(munchExp(binop->l));
				tl.push_back(munchExp(src));
				emit(gcnew(OPER, (assem, TempList(), tl)));
				return;
			} else if (_M0(konst) == binop->l) {
				//MEM(BINOP(PLUS,CONST(i),e1)),e2))
				char buf [32];
				sprintf(buf, "STORE M['s0+%d] <- 's1\n", konst->value);
				string assem = buf;
				TempList tl;
				tl.push_back(munchExp(binop->r));
				tl.push_back(munchExp(src));
				emit(gcnew(OPER, (assem, TempList(), tl)));
				return;
			}
		} 
	}
	//(MEM(e1),MEM(e2))
	if (_M0(mem1) == dst && _M0(mem2) == src) {
		string assem = "MOVE M['s0] <- 's1\n";
		TempList tsrc;
		tsrc.push_back(munchExp(mem1->exp));
		tsrc.push_back(munchExp(mem2->exp));
		emit(gcnew(OPER, (assem, TempList(), tsrc)));
		return;
	}
	//(MEM(CONST(i)),e2)
	if (_M1(mem1, _M0(konst)) == dst) {
		int i = konst->value;
		char buf [32];
		sprintf(buf, "STORE M[r0+%d] <- 's0\n", i);
		string assem = buf;
		TempList tsrc;
		tsrc.push_back(munchExp(src));
		emit(gcnew(OPER, (assem, TempList(), tsrc)));
		return;
	}
	//(MEM(e1),e2)
	if (_M0(mem1) == dst) {
		string assem = "STORE M['s0] <- 's1\n";
		TempList tsrc;
		tsrc.push_back(munchExp(mem1->exp));
		tsrc.push_back(munchExp(src));
		emit(gcnew(OPER, (assem, TempList(), tsrc)));
		return;
	}
	//(TEMP(i), e2)
	if (_M0(temp) == dst) {
		string assem = "ADD 'd0 <- 's0 + r0\n";
		TempList tdst, tsrc;
		tdst.push_back(temp->temp);
		tsrc.push_back(munchExp(src));
		emit(gcnew(OPER, (assem, tdst, tsrc)));
		return;
	}

#else
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
			string assem = "MOVE M['s0] <- 's1\n";
			TempList tsrc;
			tsrc.push_back(munchExp(mem->exp));
			tsrc.push_back(munchExp(((tree::MEM*)src)->exp));
			emit(gcnew(OPER, (assem, TempList(), tsrc)));
			return;
		}

		if (mem->exp->isCONST_T()) {
			//(MEM(CONST(i)),e2)
			int i = ((tree::CONST*)mem->exp)->value;
			char buf [32];
			sprintf(buf, "STORE M[r0+%d] <- 's0\n", i);
			string assem = buf;
			TempList tsrc;
			tsrc.push_back(munchExp(src));
			emit(gcnew(OPER, (assem, TempList(), tsrc)));
			return;
		} 

		{
			//(MEM(e1),e2)
			string assem = "STORE M['s0] <- 's1\n";
			TempList tsrc;
			tsrc.push_back(munchExp(mem->exp));
			tsrc.push_back(munchExp(src));
			emit(gcnew(OPER, (assem, TempList(), tsrc)));
			return;
		}

	} 

	if (dst->isTEMP_T()) {
		tree::TEMP *temp = (tree::TEMP*)dst;
		//(TEMP(i), e2)
		string assem = "ADD 'd0 <- 's0 + r0\n";
		TempList tdst, tsrc;
		tdst.push_back(temp->temp);
		tsrc.push_back(munchExp(src));
		emit(gcnew(OPER, (assem, tdst, tsrc)));
		return;
	}

	//TODO:
#endif
}

void
CodeGen::munchLABEL(Label *lab)
{
	emit(new LABEL(lab->toString() + ":\n", lab));
}

Temp *
CodeGen::munchExp(tree::Exp *e)
{
	if (e->isMEM_T()) {
		munchMEM((tree::MEM*)e);
	} else if (e->isBINOP_T()) {
		munchBINOP((tree::BINOP*)e);
	} else if (e->isCONST_T()) {
		munchCONST((tree::CONST*)e);
	} else if (e->isTEMP_T()) {
		munchTEMP((tree::TEMP*)e);
	}
}

Temp *
CodeGen::munchMEM(tree::MEM *mem)
{
	if (mem->exp->isBINOP_T()) {
		tree::BINOP *binop = (tree::BINOP*)mem->exp;
		if (binop->r->isCONST_T()) {
			//(BINOP(PLUS,e1,CONST(i)))
			int i = ((tree::CONST*)binop->r)->value;
			char buf [32];
			sprintf(buf, "LOAD 'd0 <- M['s0+%d]\n", i);
			string assem = buf;
			Temp *r = gcnew(Temp, ());
			TempList tdst, tsrc;
			tdst.push_back(r);
			tsrc.push_back(munchExp(binop->l));
			emit(gcnew(OPER, (assem, tdst, tsrc)));
			return r;
		}
		if (binop->l->isCONST_T()) {
			//(BINOP(PLUS,CONST(i),e1))
			int i = ((tree::CONST*)binop->l)->value;
			char buf [32];
			sprintf(buf, "LOAD 'd0 <- M['s0+%d]\n", i);
			string assem = buf;
			Temp *r = gcnew(Temp, ());
			TempList tdst, tsrc;
			tdst.push_back(r);
			tsrc.push_back(munchExp(binop->r));
			emit(gcnew(OPER, (assem, tdst, tsrc)));
			return r;
		}
	}

	if (mem->exp->isCONST_T()) {
		//(CONST(i))
		int i = ((tree::CONST*)mem->exp)->value;
		char buf [32];
		sprintf(buf, "LOAD 'd0 <- M[r0+%d]\n", i);
		string assem = buf;

		Temp *r = gcnew(Temp, ());
		TempList tdst;
		tdst.push_back(r);
		emit(gcnew(OPER, (assem, tdst, TempList())));
		return r;
	}

	{
		//(e1)
		Temp *r = gcnew(Temp, ());
		string assem = "LOAD 'd0 <- M['s0+0]\n";
		TempList tdst, tsrc;
		tdst.push_back(r);
		tsrc.push_back(munchExp(mem->exp));
		emit(gcnew(OPER, (assem, tdst, tsrc)));
		return r;
	}
}

Temp *
CodeGen::munchBINOP(tree::BINOP *binop)
{
	if (binop->op == tree::BINOP::oPLUS) {
		if (binop->r->isCONST_T()) {
			//(PLUS,e1,CONST(i))
			int i = ((tree::CONST*)binop->r)->value;
			char buf [32];
			sprintf(buf, "ADDI 'd0 <- 's0+%d\n", i);
			string assem = buf;

			Temp *r = gcnew(Temp, ());
			TempList tdst, tsrc;
			tdst.push_back(r);
			tsrc.push_back(munchExp(binop->l));
			emit(gcnew(OPER, (assem, tdst, tsrc)));
			return r;
		}
		if (binop->l->isCONST_T()) {
			//(PLUS,CONST(i),e1)
			int i = ((tree::CONST*)binop->l)->value;
			char buf [32];
			sprintf(buf, "ADDI 'd0 <- 's0+%d\n", i);
			string assem = buf;

			Temp *r = gcnew(Temp, ());
			TempList tdst, tsrc;
			tdst.push_back(r);
			tsrc.push_back(munchExp(binop->r));
			emit(gcnew(OPER, (assem, tdst, tsrc)));
			return r;
		}
		
		{
			//(PLUS,e1,e2)
			string assem = "ADD 'd0 <- 's0+'s1\n";
			Temp *r = gcnew(Temp, ());
			TempList tdst, tsrc;
			tdst.push_back(r);
			tsrc.push_back(munchExp(binop->l));
			tsrc.push_back(munchExp(binop->r));
			emit(gcnew(OPER, (assem, tdst, tsrc)));
			return r;
		}
	} else if (binop->op == tree::BINOP::oMINUS) {
	} else if (binop->op == tree::BINOP::oMUL) {
	} else if (binop->op == tree::BINOP::oDIV) {
	}
}

Temp *
CodeGen::munchCONST(tree::CONST *c)
{
	//(i)
	int i = c->value;
	char buf [32];
	sprintf(buf, "ADDI 'd0 <- r0+%d\n", i);
	string assem = buf;

	Temp *r = gcnew(Temp, ());
	TempList tdst;
	tdst.push_back(r);
	emit(gcnew(OPER, (assem, tdst, TempList())));
	return r;
}

Temp *
CodeGen::munchTEMP(tree::TEMP *t)
{
	return t->temp;
}

void 
CodeGen::emit(Instruction *inst)
{
	ilist.push_back(inst);
}

} //namespace assem
