#include "Fragment.h"
#include "TreePrinter.h"

ProcFragment::ProcFragment(tree::Stm *stm, Frame *frame)
	: Fragment(PROC_T)
	, stm(stm)
	, frame(frame)
{
}

std::string 
ProcFragment::toString()
{
	tree::TreePrinter tp;
	stm->accept(&tp);
	return tp.getResult();
}

tree::Stm *
ProcFragment::getStm()
{
	return stm;
}

Frame *
ProcFragment::getFrame()
{
	return frame;
}

DataFragment::DataFragment(const std::string &s)
	: Fragment(DATA_T)
	, data(s)
{
}

std::string
DataFragment::toString()
{
	return data;
}
