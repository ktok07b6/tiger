#ifndef ASM_FLOW_GRAPH_H
#define ASM_FLOW_GRAPH_H

#include "FlowGraph.h"
#include "Instruction.h"

namespace assem {
class CodeGen;
}

namespace graph {

typedef std::map<Node *, assem::Instruction *> InstructionNodeMap;

class AsmFlowGraph : public FlowGraph
{
public:
	AsmFlowGraph(const assem::InstructionList &instrs);
	~AsmFlowGraph();
	assem::Instruction *instr(Node *n);
private:
	void makeInstructionTable(const assem::InstructionList &instrs);
	void makeMoveTable();
	InstructionNodeMap instTable;

	assem::CodeGen *codegen;
};

}//namespace graph
#endif
