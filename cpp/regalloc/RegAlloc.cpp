#include "RegAlloc.h"
#include "Liveness.h"
#include "Color.h"
#include "AsmFlowGraph.h"
#include "Frame.h"
#include "debug.h"
#include <boost/foreach.hpp>

namespace regalloc {

RegAlloc::RegAlloc(const assem::InstructionList &proc, Frame *frame)
	: proc(proc)
	, frame(frame)
	, color(NULL)
{
	allocate();
	assert(color);
}

bool
RegAlloc::isRewritten()
{
	return false;
}

const assem::InstructionList &
RegAlloc::getProc()
{
	return proc;
}

TempMap *
RegAlloc::getTempMap()
{
	return color;
}

TempList
RegAlloc::getUsedRegs()
{
	BOOST_FOREACH(assem::Instruction *inst, proc) {
		inst->format(color);
	}
	return color->getUsedRegs();
}

int count = 0;
void
RegAlloc::allocate()
{
	DBG("allocate");
	count++;
	if (count == 10) {
		return;
	}
	const graph::AsmFlowGraph flow(proc);
	const Liveness liveness(flow);
	const graph::InterferenceGraph *igraph = liveness.getInterferenceGraph();
	//igraph->show();

	const Frame::Registers &regs = frame->registers();
	assert(color == NULL);
	color = new Color(*igraph, regs.all);
	color->setNumWorkRegs(regs.callerSaves.size() + regs.calleeSaves.size());

 	if (!color->coloring()) {
		TempList spills = color->getTempForSpill();
		rewriteProgram(spills);
		delete color;
		color = NULL;
		allocate();
	}
}

void
RegAlloc::rewriteProgram(const TempList &spills)
{
	BOOST_FOREACH(Temp *spill, spills) {
		proc = frame->spillTemp(proc, spill);
	}
}


} //namespace regalloc
