#include "ARMCodeGen.h"
#include "Instruction.h"
#include "TreeMatcher.h"
#include "tiger.h"
#include "Frame.h"

using namespace std;

assem::CodeGen *
assem::CodeGen::create()
{
	return new ARMCodeGen(NULL);
}


ARMCodeGen::ARMCodeGen(Frame *frame)
	: frame(frame)
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
					assem = format("str $s1, [$s0, #+%d]", konst->value);
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
					assem = format("str $s1, [$s0, #+%d]", konst->value);
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
		string assem = "str $s1, [$s0]";
		TempList tsrc;
		tsrc.push_back(munchExp(mem->exp));
		tsrc.push_back(munchExp(src));
		emit(gcnew(assem::OPER, (assem, TempList(), tsrc)));
		return;
	}
	//(TEMP(i), CONST(e2))
	if (_M0(TEMP_T, temp) == dst && _M0(CONST_T, konst) == src) {
		std::string assem = format("mov $d0, #%d", konst->value);
		emit(gcnew(assem::MOVE, (assem, temp->temp, NULL)));
		return;
	}

	//(TEMP(i), e2)
	if (_M0(TEMP_T, temp) == dst) {
		string assem = "mov $d0, $s0";
		Temp *ts = munchExp(src);
		emit(gcnew(assem::MOVE, (assem, temp->temp, ts)));
		return;
	}
}

void
ARMCodeGen::munchLABEL(Label *lab)
{
	emit(gcnew(assem::LABEL, (lab->toString() + ":", lab)));
}

void 
ARMCodeGen::munchJUMP(Label *lab)
{
	LabelList targets;
	targets.push_back(lab);
	std::string assem = "b $j0";
	emit(gcnew(assem::OPER, (assem, TempList(), TempList(), targets)));
}

void 
ARMCodeGen::munchCJUMP(tree::CJUMP *cj)
{
	std::string assem;
	assem = "cmp $s0, $s1";
	TempList tsrc;
	tsrc.push_back(munchExp(cj->l));
	tsrc.push_back(munchExp(cj->r));
	emit(gcnew(assem::OPER, (assem, TempList(), tsrc)));

	const char *cond[] = {"eq", "ne", 
						  "lt", "gt", 
						  "le", "ge",
						  "lo", "hi",
						  "ls", "hs"};
	LabelList targets;
	targets.push_back(cj->truelab);
	targets.push_back(cj->falselab);
	assem = format("b%s $j0", cond[cj->relop]);
	emit(gcnew(assem::OPER, (assem, TempList(), TempList(), targets)));
}

void
ARMCodeGen::munchEXPR(tree::Exp *exp)
{
	tree::CALL *call;
	if (_M0(CALL_T, call) == exp) {
		TempList tsrc;
		TempList tdst;
		munchArgs(call->args, &tsrc, &tdst);

		LabelList targets;
		targets.push_back(call->func->label);
		std::string assem = format("bl $j0");
		emit(gcnew(assem::OPER, (assem, tdst, tsrc, targets)));
	}
}

void
ARMCodeGen::munchArgs(const tree::ExpList &exps, TempList *tsrc, TempList *tdst)
{
	const Frame::Registers &regs = frame->registers();
	if (exps.size() <= regs.args.size()) {
		tree::CONST *konst;
		tree::ExpList::const_iterator it;
		TempList::const_iterator arg_reg;
		it = exps.begin();
		arg_reg = regs.args.begin();
		while (it != exps.end()) {
			tree::Exp *e = *it;
			Temp *dst = *arg_reg;
			if (_M0(CONST_T, konst) == e) {
				std::string arg_reg_str = frame->tempMap(dst);
				std::string assem = format("mov %s, #%d", arg_reg_str.c_str(), konst->value);
				emit(gcnew(assem::MOVE, (assem, dst, NULL)));

			} else {
				Temp *src = munchExp(e);
				std::string arg_reg_str = frame->tempMap(dst);
				std::string assem = format("mov %s, $s0", arg_reg_str.c_str());
				emit(gcnew(assem::MOVE, (assem, dst, src)));
				tsrc->push_back(src);
			}
			tdst->push_back(dst);
			++it;
			++arg_reg;
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
				assem = format("ldr $d0, [$s0, #+%d]", konst->value);
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
				assem = format("ldr $d0, [$s0, #+%d]", konst->value);
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
		string assem = "ldr $d0, [$s0]";
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
		std::string assem = format("%s $d0, $s0, #%d", opcode, konst->value);

		Temp *r = gcnew(Temp, ());
		TempList tdst, tsrc;
		tdst.push_back(r);
		tsrc.push_back(munchExp(binop->l));
		emit(gcnew(assem::OPER, (assem, tdst, tsrc)));
		return r;
	}
	if (_M0(CONST_T, konst) == binop->l) {
		//(op,CONST(i),e1)
		std::string assem = format("%s $d0, $s0, #%d", opcode, konst->value);

		Temp *r = gcnew(Temp, ());
		TempList tdst, tsrc;
		tdst.push_back(r);
		tsrc.push_back(munchExp(binop->r));
		emit(gcnew(assem::OPER, (assem, tdst, tsrc)));
		return r;
	}
		
	{
		//(op,e1,e2)
		std::string assem = format("%s $d0, $s0, $s1", opcode);

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
	TempList tsrc;
	TempList tdst;
	munchArgs(c->args, &tsrc, &tdst);

	assert(frame);
	Temp *rv = frame->rv();
	tdst.push_back(rv);//TODO: r0の二重登録チェック

	LabelList targets;
	targets.push_back(c->func->label);
	std::string assem = "bl $j0";
	emit(gcnew(assem::OPER, (assem, tdst, tsrc, targets)));
	return rv;
}

Temp *
ARMCodeGen::munchCONST(tree::CONST *c)
{
	//(i)
	std::string assem = format("mov $d0, #%d", c->value);

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
	return inst->isMOVE();
	/*
	std::string assem = inst->toString();
	return strncmp("mov", assem.c_str(), 3) == 0;
	*/
}

bool 
ARMCodeGen::isJump(assem::Instruction *inst)
{
	std::string assem = inst->toString();
	return strncmp("b ", assem.c_str(), 2) == 0;
}

