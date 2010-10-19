#include "Instruction.h"

namespace assem {

OPER::OPER(const std::string &assem, const TempList &dst, const TempList &src)
{
}

OPER::OPER(const std::string &assem, const TempList &dst, const TempList &src, const LabelList &jmps)
{
}


TempList 
OPER::use()
{
}

TempList 
OPER::def()
{
}

Targets 
OPER::jumps()
{
}


}//namespace assem
