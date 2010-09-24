#include "SeqExp.h"

namespace translate{
SeqExp::SeqExp(tree::SEQ *s)
	: seq(s)
{
}

tree::Exp *
SeqExp::unEx()
{
	tree::Stm *s = getLastStm();
	//TODO: check
	tree::EXPR *expr = static_cast<tree::EXPR*>(s);
	return expr->exp;
}

tree::Stm *
SeqExp::unNx()
{
	tree::Stm *s = getLastStm();
	return s;
}

tree::CJUMP *
SeqExp::unCx(Label *t, Label *f)
{
	tree::Stm *s = getLastStm();
	//TODO: check
	tree::CJUMP *cjump = static_cast<tree::CJUMP*>(s);
	return cjump;

}

tree::Stm *
SeqExp::getLastStm()
{
	tree::SEQ *s = seq;
	tree::Stm *lastStm;
	while (s) {
		if (s->r == NULL) {
			lastStm = s->l;
			break;
		}
		//TODO: check
		s = static_cast<tree::SEQ*>(s->r);
	}
	return lastStm;
}


} //namespace translate
