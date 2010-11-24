#include "Nx.h"
#include "Tree.h"
#include "Label.h"
#include "HeapManager.h"

namespace translate{

Nx::Nx(tree::Stm *s)
	: Exp(NX), stm(s)
{
}

tree::Exp *
Nx::unEx()
{
	return NULL;
}

tree::Stm *
Nx::unNx()
{
	return stm;
}

tree::CJUMP *
Nx::unCx(Label *t, Label *f)
{
	assert(0);
	return NULL;
}

}//namespace translate
