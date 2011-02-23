#include "Cx.h"
#include "Tree.h"
#include "Label.h"

namespace translate{

Cx::Cx(tree::CJUMP::RelOp o, tree::Exp *l, tree::Exp *r)
	: Exp(CX)
	, op(o)
	, left(l)
	, right(r)
{
}

tree::Exp *
Cx::unEx()
{
	Temp *tmp = gcnew(Temp, ());
	tree::TEMP *r = _TEMP(tmp);

	tree::CONST *const_1 = _CONST(1);
	tree::CONST *const_0 = _CONST(0);
	tree::MOVE *r_1 = _MOVE(r, const_1);
	tree::MOVE *r_0 = _MOVE(r, const_0);

	Label *t = gcnew(Label, ());
	Label *f = gcnew(Label, ());
	tree::LABEL *l_T = _LABEL(t);
	tree::LABEL *l_F = _LABEL(f);

	tree::Stm *cmp = unCx(t, f);

	tree::SEQMaker sm;
	sm.add(r_1);
	sm.add(cmp);
	sm.add(l_F);
	sm.add(r_0);
	sm.add(l_T);
	tree::SEQ *seq = sm.make();//tree::makeSEQ(r_1, cmp, l_F, r_0, l_T);
	tree::ESEQ *eseq = _ESEQ(seq, r);
	return eseq;
}

tree::Stm *
Cx::unNx()
{
	Label *l = gcnew(Label, ());
	tree::LABEL *join = _LABEL(l);
	return _SEQ(unCx(l, l), join);
}

tree::CJUMP *
Cx::unCx(Label *t, Label *f)
{
	return _CJUMP(op, left, right, t, f);
}

}//namespace translate
