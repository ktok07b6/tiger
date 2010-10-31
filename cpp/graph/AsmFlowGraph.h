#ifndef ASM_FLOW_GRAPH_H
#define ASM_FLOW_GRAPH_H

#include "FlowGraph.h"
#include "Instruction.h"
#include "Node.h"

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
	class InstNode : public Node
	{
	public:
	InstNode(AsmFlowGraph *g) : Node(g) {}
		assem::Instruction *getInst() {
			return inst;
		}
	private:
		assem::Instruction *inst;
	};

	void makeInstructionTable(const assem::InstructionList &instrs);
	void makeMoveTable();
	InstNode *findLABELNode(Label *l);

	assem::CodeGen *codegen;
};

}//namespace graph
#endif
