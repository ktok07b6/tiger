#include "Fragment.h"


ProcFragment::ProcFragment(tree::Stm *stm, Frame *frame)
	: stm(stm)
	, frame(frame)
{
}

DataFragment::DataFragment(const std::string &s)
	: data(s)
{
}
