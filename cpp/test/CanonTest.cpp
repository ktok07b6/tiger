#include "Canon.h"
#include "Tree.h"
#include "TreePrinter.h"

int main(int argc, char **argv)
{
	Canon canon;

	
	tree::TEMP *t1 = _TEMP(gcnew(Temp, ()));
	tree::TEMP *t2 = _TEMP(gcnew(Temp, ()));
	tree::MOVE *s2 = _MOVE(t1, t2);
	tree::MOVE *s1 = _MOVE(t1, t2);

	tree::ESEQ *eseq2 = _ESEQ(s2, t1);
	tree::ESEQ *eseq1 = _ESEQ(s1, eseq2);

	tree::Stm *stm = _EXPR(eseq1);
	tree::TreePrinter::printTree(stm);

	tree::StmList slist;
	slist = canon.linearize(stm);
	tree::TreePrinter::printStmList(slist);
}
