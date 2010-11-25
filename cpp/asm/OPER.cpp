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

OPER::OPER(const std::string &opcode, 
		   const std::string &operands,
		   Temp *d, 
		   Temp *s,
		   const std::string &comment,
		   int sourceLine)
	: Instruction(opcode, operands, comment, sourceLine)
	, dst()
	, src()
{
	if (d) {
		dst.push_back(d);
	}
	if (s) {
		src.push_back(s);
	}
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
