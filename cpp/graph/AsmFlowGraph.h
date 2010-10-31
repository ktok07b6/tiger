#ifndef ASM_FLOW_GRAPH_H
#define ASM_FLOW_GRAPH_H

#include "FlowGraph.h"
#include "Instruction.h"
#include "Node.h"

namespace assem {
class CodeGen;
}

namespace graph {

class AsmFlowGraph : public FlowGraph
{
public:
	AsmFlowGraph(const assem::InstructionList &instrs);
	~AsmFlowGraph();
	virtual TempList def(Node *node);
	virtual TempList use(Node *node);
	virtual bool isMove(Node *node);

	assem::Instruction *instr(Node *n);

private:
	class InstNode : public Node
	{
	public:
	InstNode(AsmFlowGraph *g, assem::Instruction *i) : Node(g), inst(i) {}
		assem::Instruction *getInst() {
			return inst;
		}
	private:
		assem::Instruction *inst;
	};

	void makeNodes(const assem::InstructionList &instrs);
	InstNode *findLABELNode(Label *l);

	assem::CodeGen *codegen;
};

}//namespace graph
#endif
