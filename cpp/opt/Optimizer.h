#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "Instruction.h"

namespace opt {

enum OptimizationOptionKey {
	REMOVE_USELESS_MOVE,
	REMOVE_USELESS_JUMP,
	CONSTANT_FOLDING,

	MAX_OPTIMIZATION_OPTION
};

enum {
	OPT_DISABLE = 0,
	OPT_ENABLE = 1
};

assem::InstructionList optimize(const assem::InstructionList &ilist);
void setOptimizationOption(OptimizationOptionKey key, int value);
int getOptimizationOption(OptimizationOptionKey key);

}//namespace opt

#endif //OPTIMIZER_H
