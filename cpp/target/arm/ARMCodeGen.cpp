#include "ARMCodeGen.h"
#include "Temp.h"
#include "Instruction.h"
#include "TreeMatcher.h"
#include "tiger.h"

using namespace std;

assem::CodeGen *
assem::CodeGen::create()
{
	return new ARMCodeGen();
}


ARMCodeGen::ARMCodeGen()
{
}


void
ARMCodeGen::munchMOVE(tree::Exp *dst, tree::Exp *src)
{
	tree::MEM *mem;
	tree::BINOP *binop;
	tree::CONST *konst;
	tree::TEMP *temp;

	if (_M1(MEM_T, mem, _M0(BINOP_T, binop)) == dst) {
		if (binop->op == tree::BINOP::oPLUS) {
			if (_M0(CONST_T, konst) == binop->r) {
				//(MEM(BINOP(PLUS,e1,CONST(i))), e2)
				std::string assem;
				if (konst->value < 4096) {
					assem = format("str 's1, ['s0, #+%d]\n", konst->value);
				} else {
					//TODO:
					assert(0);
				}
				TempList tl;
				tl.push_back(munchExp(binop->l));
				tl.push_back(munchExp(src));
				emit(gcnew(assem::OPER, (assem, TempList(), tl)));
				return;
			} else if (_M0(CONST_T, konst) == binop->l) {
				//MEM(BINOP(PLUS,CONST(i),e1)),e2))
				std::string assem;
				if (konst->value < 4096) {
					assem = format("str 's1, ['s0, #+%d]\n", konst->value);
				} else {
					//TODO:
					assert(0);
				}
				TempList tl;
				tl.push_back(munchExp(binop->r));
				tl.push_back(munchExp(src));
				emit(gcnew(assem::OPER, (assem, TempList(), tl)));
				return;
			}
		} 
	}
	//(MEM(e1),e2)
	if (_M0(MEM_T, mem) == dst) {
		string assem = "str 's0, ['s1]\n";
		TempList tsrc;
		tsrc.push_back(munchExp(mem->exp));
		tsrc.push_back(munchExp(src));
		emit(gcnew(assem::OPER, (assem, TempList(), tsrc)));
		return;
	}
	//(TEMP(i), e2)
	if (_M0(TEMP_T, temp) == dst) {
		string assem = "mov 'd0, 's0\n";
		TempList tdst, tsrc;
		tdst.push_back(temp->temp);
		tsrc.push_back(munchExp(src));
		emit(gcnew(assem::OPER, (assem, tdst, tsrc)));
		return;
	}
}

void
ARMCodeGen::munchLABEL(Label *lab)
{
	emit(gcnew(assem::LABEL, (lab->toString() + ":\n", lab)));
}

void 
ARMCodeGen::munchJUMP(Label *lab)
{
	std::string assem = format("b %s", lab->toString().c_str());
	emit(gcnew(assem::OPER, (assem, TempList(), TempList())));
}

void 
ARMCodeGen::munchCJUMP(tree::CJUMP *cj)
{
	const char *cond[] = {"eq", "ne", 
						  "lt", "gt", 
						  "le", "ge",
						  "lo", "hi",
						  "ls", "hs"};
	std::string assem;
	assem = format("cmp 's0, 's1\n"										\
				   "b%s %s", cond[cj->relop], cj->truelab->toString().c_str());
	TempList tsrc;
	tsrc.push_back(munchExp(cj->l));
	tsrc.push_back(munchExp(cj->r));
	emit(gcnew(assem::OPER, (assem, TempList(), tsrc)));
}

void
ARMCodeGen::munchEXPR(tree::Exp *exp)
{
	tree::CALL *call;
	if (_M0(CALL_T, call) == exp) {
		munchArgs(call->args);
		
		std::string assem = format("bl %s", call->func->label->toString().c_str());
		emit(gcnew(assem::OPER, (assem, TempList(), TempList())));
	}
}

void 
ARMCodeGen::munchArgs(const tree::ExpList &exp)
{
	if (exp.size() <= 4) {
		tree::ExpList::const_iterator it;
		it = exp.begin();
		int i = 0;
		while (it != exp.end()) {
			tree::Exp *e = *it;
			Temp *t = munchExp(e);

			std::string assem = format("mov r%d, 's0", i);
			TempList tsrc;
			tsrc.push_back(t);
			emit(gcnew(assem::OPER, (assem, TempList(), tsrc)));
			++it;
			++i;
		}
	} else {
		//TODO:
	}
}


Temp *
ARMCodeGen::munchMEM(tree::MEM *mem)
{
	tree::BINOP *binop;
	tree::CONST *konst;

	if (_M0(BINOP_T, binop) == mem->exp) {
		if (_M0(CONST_T, konst) == binop->r) {
			//(BINOP(PLUS,e1,CONST(i)))
			std::string assem;
			if (konst->value < 4096) {
				assem = format("ldr 'd0, ['s0, #+%d]\n", konst->value);
			} else {
				//TODO:
				assert(0);
			}
			Temp *r = gcnew(Temp, ());
			TempList tdst, tsrc;
			tdst.push_back(r);
			tsrc.push_back(munchExp(binop->l));
			emit(gcnew(assem::OPER, (assem, tdst, tsrc)));
			return r;
		}
		if (_M0(CONST_T, konst) == binop->l) {
			//(BINOP(PLUS,CONST(i),e1))
			std::string assem;
			if (konst->value < 4096) {
				assem = format("ldr 'd0, ['s0, #+%d]\n", konst->value);
			} else {
				//TODO:
				assert(0);
			}
			Temp *r = gcnew(Temp, ());
			TempList tdst, tsrc;
			tdst.push_back(r);
			tsrc.push_back(munchExp(binop->r));
			emit(gcnew(assem::OPER, (assem, tdst, tsrc)));
			return r;
		}
	}

	{
		//(e1)
		string assem = "ldr 'd0, ['s0]\n";
		TempList tdst, tsrc;
		Temp *r = gcnew(Temp, ());
		tdst.push_back(r);
		tsrc.push_back(munchExp(mem->exp));
		emit(gcnew(assem::OPER, (assem, tdst, tsrc)));
		return r;
	}
}

Temp *
ARMCodeGen::munchBINOP(tree::BINOP *binop)
{
	tree::CONST *konst;
	const char *opcode;
	switch (binop->op) {
	case tree::BINOP::oPLUS:
		opcode = "add";
		break;	
	case tree::BINOP::oMINUS:
		opcode = "sub";
		break;
	case tree::BINOP::oMUL:
		opcode = "mul";
		break;
	case tree::BINOP::oDIV:
		//TODO:
		opcode = "div???";
		break;
	default:
		//TODO:
		opcode = "???";
		break;
	}
	if (_M0(CONST_T, konst) == binop->r) {
		//(op,e1,CONST(i))
		std::string assem = format("%s 'd0, 's0, #%d\n", opcode, konst->value);

		Temp *r = gcnew(Temp, ());
		TempList tdst, tsrc;
		tdst.push_back(r);
		tsrc.push_back(munchExp(binop->l));
		emit(gcnew(assem::OPER, (assem, tdst, tsrc)));
		return r;
	}
	if (_M0(CONST_T, konst) == binop->l) {
		//(op,CONST(i),e1)
		std::string assem = format("%s 'd0, 's0, #%d\n", opcode, konst->value);

		Temp *r = gcnew(Temp, ());
		TempList tdst, tsrc;
		tdst.push_back(r);
		tsrc.push_back(munchExp(binop->r));
		emit(gcnew(assem::OPER, (assem, tdst, tsrc)));
		return r;
	}
		
	{
		//(op,e1,e2)
		std::string assem = format("%s 'd0, 's0, 's1\n", opcode);

		Temp *r = gcnew(Temp, ());
		TempList tdst, tsrc;
		tdst.push_back(r);
		tsrc.push_back(munchExp(binop->l));
		tsrc.push_back(munchExp(binop->r));
		emit(gcnew(assem::OPER, (assem, tdst, tsrc)));
		return r;
	}
}

Temp *
ARMCodeGen::munchCALL(tree::CALL *c)
{
	{
		munchArgs(c->args);
		std::string assem = format("bl %s", c->func->label->toString().c_str());
		emit(gcnew(assem::OPER, (assem, TempList(), TempList())));
	}
	{
		std::string assem = "mov 'd0, r0";
		Temp *r = gcnew(Temp, ());
		TempList tdst;
		tdst.push_back(r);
		emit(gcnew(assem::OPER, (assem, tdst, TempList())));
		return r;
	}
}

Temp *
ARMCodeGen::munchCONST(tree::CONST *c)
{
	//(i)
	std::string assem = format("mov 'd0, #%d\n", c->value);

	Temp *r = gcnew(Temp, ());
	TempList tdst;
	tdst.push_back(r);
	emit(gcnew(assem::OPER, (assem, tdst, TempList())));
	return r;
}

Temp *
ARMCodeGen::munchTEMP(tree::TEMP *t)
{
	return t->temp;
}

bool 
ARMCodeGen::isMove(assem::Instruction *inst)
{
	std::string assem = inst->toString();
	return strncmp("mov", assem.c_str(), 3) == 0;
}

bool 
ARMCodeGen::isJump(assem::Instruction *inst)
{
	std::string assem = inst->toString();
	return strncmp("b ", assem.c_str(), 2) == 0;
}

