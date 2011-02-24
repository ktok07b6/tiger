#include "Instruction.h"

namespace assem {

LABEL::LABEL(const std::string &assem, Label *l)
	: Instruction(assem)
	, lab(l)
{
}

TempList 
LABEL::use()
{
	return TempList();
}

TempList 
LABEL::def()
{
	return TempList();
}

LabelList
LABEL::jumps()
{
	return LabelList();
}

void 
LABEL::replaceUse(Temp *oldt, Temp *newt)
{
	assert(!"do not reach here");
}

void 
LABEL::replaceDef(Temp *oldt, Temp *newt)
{
	assert(!"do not reach here");
}

void 
LABEL::replaceUse(unsigned int index, Temp *newt)
{
	assert(!"do not reach here");
}

void 
LABEL::replaceDef(unsigned int index, Temp *newt)
{
	assert(!"do not reach here");
}

}//namespace assem
