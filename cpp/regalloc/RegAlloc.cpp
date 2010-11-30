#include "RegAlloc.h"
#include "Liveness.h"
#include "Color.h"
#include "AsmFlowGraph.h"

namespace regalloc {

RegAlloc::RegAlloc(const assem::InstructionList &proc, Frame *frame)
	: proc(proc)
	, frame(frame)
{
	allocate();
}

void
RegAlloc::allocate()
{
	const graph::AsmFlowGraph flow(proc);
	const Liveness liveness(flow);
	const graph::InterferenceGraph *igraph = liveness.getInterferenceGraph();
	//igraph->show();

	TempList regs = frame->registers().all;
	
	Color color(*igraph, regs);
	if (!color.coloring()) {
		Temp *t = color.getTempForSpill();
		rewriteProgram(t);
		allocate();
	}
}

void
RegAlloc::rewriteProgram(Temp *spill)
{
	proc = frame->spillTemp(proc, spill);
}


} //namespace regalloc
