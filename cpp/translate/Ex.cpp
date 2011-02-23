#include "Ex.h"
#include "Tree.h"
#include "Label.h"
#include "HeapManager.h"

namespace translate{

Ex::Ex(tree::Exp *e)
	: Exp(EX), exp(e)
{
}

tree::Exp *
Ex::unEx()
{
	return exp;
}

tree::Stm *
Ex::unNx()
{
	return _EXPR(exp);
}

tree::CJUMP *
Ex::unCx(Label *t, Label *f)
{
	//assert(0);
	return NULL;
}

}//namespace translate
