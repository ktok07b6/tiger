#include "Frame.h"

Frame::Frame()
	: name(NULL)
	, endFuncLabel(NULL)
{
}

Label *
Frame::getEndFuncLabel()
{
	return endFuncLabel;
}

