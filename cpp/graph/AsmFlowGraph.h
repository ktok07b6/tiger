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
	virtual TempList def(Node *node) const;
	virtual TempList use(Node *node) const;
	virtual bool isMove(Node *node) const;

	assem::Instruction *instr(Node *n);

	class InstNode : public Node
	{
	public:
	InstNode(AsmFlowGraph *g, assem::Instruction *i) : Node(g), inst(i) {}
		assem::Instruction *getInst() const{
			return inst;
		}
	private:
		assem::Instruction *inst;
	};

private:
	void makeNodes(const assem::InstructionList &instrs);
	InstNode *findLABELNode(Label *l);

	assem::CodeGen *codegen;
};

}//namespace graph
#endif
