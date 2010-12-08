#ifndef REG_ALLOC_H
#define REG_ALLOC_H

#include "Instruction.h"
#include "Color.h"
class Frame;

namespace regalloc {

class RegAlloc
{
 public:
	RegAlloc(const assem::InstructionList &proc, Frame *);

	bool isRewritten();

	const assem::InstructionList &getProc();
	TempMap *getTempMap();
	TempList getUsedRegs();
private:
	void allocate();
	void rewriteProgram(const TempList &spills);

	assem::InstructionList proc;
	Frame *frame;
	Color *color;
};

} //namespace regalloc

#endif //REG_ALLOC_H
