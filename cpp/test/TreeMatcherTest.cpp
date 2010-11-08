#include "TreeMatcher.h"

using namespace tree;

int main(int argc, char **argv)
{
	CONST *skonst = new CONST(0);	
	MEM *smem2 = new MEM(skonst);
	MEM *smem1 = new MEM(smem2);


	Tree *test_tree = smem1;
	
	MEM *mem1 = new MEM(NULL);
	MEM *mem2 = new MEM(NULL);
	CONST *konst = new CONST(0);

	bool ret = _MMEM(mem1, _MMEM(mem2, _MCONST(konst))) == test_tree;
	assert(ret);
	assert(smem1 == mem1);
	assert(smem2 == mem2);
	assert(skonst == konst);
	return 0;
}
