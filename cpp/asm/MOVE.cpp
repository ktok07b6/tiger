#include "Instruction.h"

namespace assem {

MOVE::MOVE(const std::string &assem, Temp *dst, Temp *src)
	: Instruction(assem)
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
