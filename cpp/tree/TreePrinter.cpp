#include "TreePrinter.h"
#include "Tree.h"
#include "debug.h"

#define BGN std::string old = tab;tab += "  "
#define END tab = old

namespace tree {

const char *op2str(BINOP::Oper op)
{
	switch (op) {
	case BINOP::oPLUS:   return "PLUS";
	case BINOP::oMINUS:  return "MINUS";
	case BINOP::oMUL:    return "MUL";
	case BINOP::oDIV:    return "DIV";
	case BINOP::oAND:    return "AND";
	case BINOP::oOR:     return "OR";
	case BINOP::oXOR:    return "XOR";
	case BINOP::oLSHIFT: return "LSHIFT";
	case BINOP::oRSHIFT: return "RSHIFT";
	case BINOP::oARSHIFT:return "ARSHIFT";
	default: return "";
	}
}

void
TreePrinter::visit(BINOP *binop)
{
	FUNCLOG;
	BGN;
	result += tab;
	result += "BINOP(\n";
	result += tab + "  ";
	result += op2str(binop->op);
	result += "\n";
	result += tab;
	result += ",\n";
	assert(binop->l);
	binop->l->accept(this);
	result += tab;
	result += ",\n";
	assert(binop->r);
	binop->r->accept(this);
	result += tab;
	result += ")\n";
	END;
}

void
TreePrinter::visit(CALL *call)
{
	FUNCLOG;
	BGN;
	result += tab;
	result += "CALL(\n";
	assert(call->func);
	call->func->accept(this);
	std::list<Exp*>::iterator it;
	it = call->args.li.begin();
	while (it != call->args.li.end()) {
		Exp *e = *it;
		result += ",\n";
		assert(e);
		e->accept(this);
		++it;
	}
	result += tab;
	result += ")\n";
	END;
}

void
TreePrinter::visit(CONST *konst)
{
	FUNCLOG;
	BGN;
	result += tab;
	result += "CONST(";
	char buf[16];
	sprintf(buf, "%d", konst->value);
	result += buf;
	result += ")\n";
	END;
}

void
TreePrinter::visit(ESEQ *eseq)
{
	FUNCLOG;
	BGN;
	result += tab;
	result += "ESEQ(\n";
	assert(eseq->stm);
	eseq->stm->accept(this);
	result += tab;
	result += ",\n";
	if (eseq->exp) {
		eseq->exp->accept(this);
	} else {
		result += "nil";
	}
	result += tab;
	result += ")\n";
	END;
}

void
TreePrinter::visit(MEM *mem)
{
	FUNCLOG;
	BGN;
	result += tab;
	result += "MEM(\n";
	assert(mem->exp);
	mem->exp->accept(this);
	result += tab;
	result += ")\n";
	END;
}

void
TreePrinter::visit(NAME *name)
{
	FUNCLOG;
	BGN;
	result += tab;
	result += "NAME(";
	result += name->label->toString();
	result += ")\n";
	END;
}

void
TreePrinter::visit(TEMP *temp)
{
	FUNCLOG;
	BGN;
	result += tab;
	result += "TEMP(";
	result += temp->temp->toString();
	result += ")\n";
	END;
}

void
TreePrinter::visit(EXPR *expr)
{
	FUNCLOG;
	BGN;
	result += tab;
	result += "EXPR(\n";
	if (expr->exp) {
	  expr->exp->accept(this);
	} else {
	  result += tab + "  ";
	  result += "Nil\n";
	}
	result += tab;
	result += ")\n";
	END;
}

const char *relop2str(CJUMP::RelOp op)
{
	switch (op) {
	case CJUMP::oEQ:  return "EQ";
	case CJUMP::oNE:  return  "NE";
	case CJUMP::oLT:  return "LT";
	case CJUMP::oGT:  return "GT";
	case CJUMP::oLE:  return "LE";
	case CJUMP::oGE:  return "GE";
	case CJUMP::oULT: return "ULT";
	case CJUMP::oUGT: return "UGT";
	case CJUMP::oULE: return "ULE";
	case CJUMP::oUGE: return "UGE";
	default: return "";
	}
}

void
TreePrinter::visit(CJUMP *cjump)
{
	FUNCLOG;
	BGN;
	assert(cjump->l);
	assert(cjump->r);
	assert(cjump->truelab);
	assert(cjump->falselab);
	result += tab;
	result += "CJUMP(\n";
	result += tab + "  ";
	result += relop2str(cjump->relop);
	result += "\n";
	result += tab;
	result += ",\n";
	cjump->l->accept(this);
	result += tab;
	result += ",\n";
	cjump->r->accept(this);
	result += tab;
	result += ",\n";
	result += tab + "  ";
	result += cjump->truelab->toString();
	result += "\n";
	result += tab;
	result += ",\n";
	result += tab + "  ";
	result += cjump->falselab->toString();
	result += "\n";
	result += tab;
	result += ")\n";
	END;
}

void
TreePrinter::visit(JUMP *jump)
{
	FUNCLOG;
	BGN;
	result += tab;
	result += "JUMP(\n";
	jump->exp->accept(this);

	LabelList::iterator it;
	it = jump->targets.begin();
	while (it != jump->targets.end()) {
		Label *l = *it;
		result += tab;
		result += ",\n";
		result += tab + "  ";
		result += l->toString();
		result += "\n";
		++it;
	}
	result += tab;
	result += ")\n";
	END;
}

void
TreePrinter::visit(LABEL *label)
{
	FUNCLOG;
	BGN;
	result += tab;
	result += "LABEL(";
	result += label->label->toString();
	result += ")\n";
	END;
}

void
TreePrinter::visit(MOVE *move)
{
	FUNCLOG;
	BGN;
	result += tab;
	result += "MOVE(\n";
	move->dst->accept(this);
	result += tab;
	result += ",\n";
	if (move->src) {
		move->src->accept(this);
	} else {
		result += tab + "  ";
		result += "Nil";
		result += "\n";
	}
	result += tab;
	result += ")\n";
	END;
}

void 
TreePrinter::visit(SEQ *seq)
{
	FUNCLOG;
	BGN;
	result += tab;
	result += "SEQ(\n";
	if (seq->l) {
		seq->l->accept(this);
	} else {
		result += tab + "  ";
		result += "Nil\n";
	}
	result += tab;
	result += ",\n";
	if (seq->r) {
		seq->r->accept(this);
	} else {
		result += tab + "  ";
		result += "Nil\n";
	}
	result += tab;
	result += ")\n";
	END;
}

void
TreePrinter::printTree(Tree *t)
{
	TreePrinter treePrinter;
	if (t) {
		t->accept(&treePrinter);
	}
	DBG("%s\n", treePrinter.result.c_str());
}

void
TreePrinter::printExpList(const ExpList &elist)
{
	ExpList::const_iterator it = elist.begin();
	while (it != elist.end()) {
		TreePrinter treePrinter;
		(*it)->accept(&treePrinter);
		DBG("-----\n");
		DBG("%s\n", treePrinter.result.c_str());
		++it;
	}
}

void
TreePrinter::printStmList(const StmList &slist)
{
	StmList::const_iterator it = slist.begin();
	while (it != slist.end()) {
		TreePrinter treePrinter;
		(*it)->accept(&treePrinter);
		DBG("-----\n");
		DBG("%s\n", treePrinter.result.c_str());
		++it;
	}
}

}//namespace tree
