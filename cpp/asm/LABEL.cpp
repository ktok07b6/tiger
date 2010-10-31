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

}//namespace assem
