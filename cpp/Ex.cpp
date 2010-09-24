#include "Ex.h"
#include "Tree.h"
#include "Label.h"
#include "HeapManager.h"

namespace translate{

Ex::Ex(tree::Exp *e)
	: exp(e)
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
	return gcnew(tree::EXPR, (exp));
}

tree::CJUMP *
Ex::unCx(Label *t, Label *f)
{
	//assert(0);
	return NULL;
}

}//namespace translate
