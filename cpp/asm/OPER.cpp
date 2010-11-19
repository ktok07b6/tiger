#include "Instruction.h"

namespace assem {
OPER::OPER(const std::string &opcode, 
		   const std::string &operands,
		   const TempList &dst, 
		   const TempList &src,
		   const std::string &comment,
		   int sourceLine)
	: Instruction(opcode, operands, comment, sourceLine)
	, dst(dst)
	, src(src)
{
}


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

void
OPER::setJumpTargets(const LabelList &jmps)
{
	targets = jmps;
}

}//namespace assem
