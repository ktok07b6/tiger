#ifndef REG_ALLOC_H
#define REG_ALLOC_H

#include "Instruction.h"
class Frame;

namespace regalloc {

class RegAlloc
{
 public:
	RegAlloc(const assem::InstructionList &proc, Frame *);

	void allocate();
 private:
	void rewriteProgram(Temp*);

	assem::InstructionList proc;
	Frame *frame;
};

} //namespace regalloc

#endif //REG_ALLOC_H
