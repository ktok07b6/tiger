#include "Instruction.h"

namespace assem {

MOVE::MOVE(const std::string &opcode, 
		   const std::string &operands,
		   Temp *dst, 
		   Temp *src,
		   const std::string &comment,
		   int sourceLine)
	: Instruction(opcode, operands, comment, sourceLine)
	, dst(dst)
	, src(src)
{
}

TempList 
MOVE::use()
{
	TempList uses;
	if (src) {
		uses.push_back(src);
	}
	return uses;
}

TempList 
MOVE::def()
{
	TempList defs;
	if (dst) {
		defs.push_back(dst);
	}
	return defs;
}

LabelList 
MOVE::jumps()
{
	return LabelList();
}

}//namespace assem
