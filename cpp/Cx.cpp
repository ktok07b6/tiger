#include "Cx.h"
#include "Tree.h"
#include "Label.h"

namespace translate{

Cx::Cx(tree::CJUMP::RelOp o, tree::Exp *l, tree::Exp *r)
	: op(o)
	, left(l)
	, right(r)
{
}

tree::Exp *
Cx::unEx()
{
	Temp *tmp = gcnew(Temp, ());
	tree::TEMP *r = gcnew(tree::TEMP, (tmp));

	tree::CONST *const_1 = gcnew(tree::CONST, (1));
	tree::CONST *const_0 = gcnew(tree::CONST, (0));
	tree::MOVE *r_1 = gcnew(tree::MOVE, (r, const_1));
	tree::MOVE *r_0 = gcnew(tree::MOVE, (r, const_0));

	Label *t = gcnew(Label, ());
	Label *f = gcnew(Label, ());
	tree::LABEL *l_T = gcnew(tree::LABEL, (t));
	tree::LABEL *l_F = gcnew(tree::LABEL, (f));

	tree::Stm *cmp = unCx(t, f);

	tree::SEQMaker sm;
	sm.add(r_1);
	sm.add(cmp);
	sm.add(l_F);
	sm.add(r_0);
	sm.add(l_T);
	tree::SEQ *seq = sm.make();//tree::makeSEQ(r_1, cmp, l_F, r_0, l_T);
	tree::ESEQ *eseq = gcnew(tree::ESEQ, (seq, r));
	return eseq;
}

tree::Stm *
Cx::unNx()
{
	Label *l = gcnew(Label, ());
	tree::LABEL *join = gcnew(tree::LABEL, (l));
	return gcnew(tree::SEQ, (unCx(l, l), join));
}

tree::CJUMP *
Cx::unCx(Label *t, Label *f)
{
	return gcnew(tree::CJUMP, (op, left, right, t, f));
}

}//namespace translate
