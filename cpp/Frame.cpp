#include "Frame.h"
#include "ARMFrame.h"
#include "HeapManager.h"

Frame *
Frame::newFrame(Symbol *name, const std::vector<int> &formals)
{
	return gcnew(ARMFrame, (name, formals));
}
