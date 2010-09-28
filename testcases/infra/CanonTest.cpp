
#include "Canon.h"
#include "Tree.h"
#include "TreePrinter.h"

void printIR(tree::Stm *tree)
{
	tree::TreePrinter treePrinter;
	if (tree) {
		tree->accept(&treePrinter);
	}
	printf("%s\n", treePrinter.result.c_str());
}

void printStmList(const tree::StmList &slist)
{
	std::list<tree::Stm*>::const_iterator it = slist.li.begin();
	while (it != slist.li.end()) {
		printIR(*it);
		++it;
	}
}

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
	printIR(stm);

	tree::StmList slist;
	slist = canon.linearize(stm);
	printStmList(slist);
}
