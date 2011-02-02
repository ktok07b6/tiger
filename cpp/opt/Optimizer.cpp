#include "Optimizer.h"

namespace opt {
int option[MAX_OPTIMIZATION_OPTION];

extern assem::InstructionList removeUselessMove(const assem::InstructionList &ilist);


assem::InstructionList 
optimize(const assem::InstructionList &il)
{
	assem::InstructionList ilist = il;

	if (option[REMOVE_USELESS_MOVE]) {
		ilist = removeUselessMove(ilist);
	}
	return ilist;
}

void 
setOptimizationOption(OptimizationOptionKey key, int value)
{
	option[key] = value;
}

int 
getOptimizationOption(OptimizationOptionKey key)
{
	return option[key];
}

}//namespace opt
