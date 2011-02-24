#include "Instruction.h"
#include <assert.h>

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
OPER::replaceUse(Temp *oldt, Temp *newt)
{
	TempList::iterator it;
	it = std::find(src.begin(), src.end(), oldt);
	assert(it != src.end());
	*it = newt;
	assert(std::find(src.begin(), src.end(), oldt) == src.end());
	assert(std::find(src.begin(), src.end(), newt) != src.end());
}

void 
OPER::replaceDef(Temp *oldt, Temp *newt)
{
	TempList::iterator it;
	it = std::find(dst.begin(), dst.end(), oldt);
	assert(it != dst.end());
	*it = newt;
	assert(std::find(dst.begin(), dst.end(), oldt) == dst.end());
	assert(std::find(dst.begin(), dst.end(), newt) != dst.end());
}

void 
OPER::replaceUse(unsigned int index, Temp *newt)
{
	assert(0 <= index && index < src.size());
	src[index] = newt;
}

void 
OPER::replaceDef(unsigned int index, Temp *newt)
{
	assert(0 <= index && index < dst.size());
	dst[index] = newt;
}

void
OPER::setJumpTargets(const LabelList &jmps)
{
	targets = jmps;
}

}//namespace assem
