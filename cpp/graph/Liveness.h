#ifndef LIVENESS_H
#define LIVENESS_H

namespace graph {

class AsmFlowGraph;
class InterferenceGraph;

class Liveness
{
 public:
	Liveness(const assem::InstructionList &instrs);
};
}//namespace graph

#endif
