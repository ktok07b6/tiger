#include "Instruction.h"

namespace assem {

OPER::OPER(const std::string &assem, const TempList &dst, const TempList &src)
	: Instruction(assem)
	, dst(dst)
	, src(src)
{
}

OPER::OPER(const std::string &assem, const TempList &dst, const TempList &src, const LabelList &jmps)
	: Instruction(assem)
	, dst(dst)
	, src(src)
	, targets(jmps)
{
}


TempList 
OPER::use()
{
	return src;
}

TempList 
OPER::def()
{
	return dst;
}

LabelList 
OPER::jumps()
{
	return targets;
}


}//namespace assem
