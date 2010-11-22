#include "Frame.h"
#include "tiger.h"

Frame::Frame()
	: name(NULL), endLabel(NULL)
{
	endLabel = gcnew(Label, (END_FUNCTION_LABEL_NAME));
}

Label *
Frame::getEndLabel()
{
	return endLabel;
}

