#include "Liveness.h"
#include <vector>
#include <string>
#include "Instruction.h"
#include "AsmFlowGraph.h"
#include "InterferenceGraph.h"
#include "HeapManager.h"

using namespace std;
using namespace assem;
using namespace graph;
using namespace regalloc;

int main(int argc, char **argv)
{
	/*

	  a = 0;
L1:	  
      b = a + 1;
	  c = c + b;
	  a = b * 2;
	  if (a < 10) goto l1
	  return c
	 */
	Temp *a = new Temp();
	Temp *b = new Temp();
	Temp *c = new Temp();
	Label *l1 = new Label();
	Label *l2 = new Label();

	InstructionList ilist;
	Instruction *inst;
	TempList tdst, tsrc;
	LabelList targets;

	tdst.clear(); tsrc.clear();
	tdst.push_back(a);
	inst = gcnew(OPER, ("mov $d0, #0", tdst, tsrc));
	ilist.push_back(inst);

	tdst.clear(); tsrc.clear();
	inst = gcnew(assem::LABEL, ("L1:", l1));
	ilist.push_back(inst);

	tdst.clear(); tsrc.clear();
	tdst.push_back(b);
	tsrc.push_back(a);
	inst = gcnew(OPER, ("add $d0, $s0, #1", tdst, tsrc));
	ilist.push_back(inst);

	tdst.clear(); tsrc.clear();
	tdst.push_back(c);
	tsrc.push_back(c);
	tsrc.push_back(b);
	inst = gcnew(OPER, ("add $d0, $s0, $s1", tdst, tsrc));
	ilist.push_back(inst);

	tdst.clear(); tsrc.clear();
	tdst.push_back(a);
	tsrc.push_back(b);
	inst = gcnew(OPER, ("mul $d0, $s0, #2", tdst, tsrc));
	ilist.push_back(inst);

	tdst.clear(); tsrc.clear();
	tsrc.push_back(a);
	inst = gcnew(OPER, ("cmp $s0, #10", tdst, tsrc));
	ilist.push_back(inst);

	tdst.clear(); tsrc.clear();
	targets.clear();
	targets.push_back(l1);
	inst = gcnew(OPER, ("blt $j0", tdst, tsrc, targets));
	ilist.push_back(inst);

	tdst.clear(); tsrc.clear();
	tsrc.push_back(c);
	inst = gcnew(OPER, ("mov r0, $s0", tdst, tsrc));
	ilist.push_back(inst);

	const graph::AsmFlowGraph flow(ilist);
	const regalloc::Liveness liveness(flow);
	const graph::InterferenceGraph *igraph = liveness.getInterferenceGraph();
	igraph->show();
	return 0;
}
