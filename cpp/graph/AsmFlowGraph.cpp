#include "AsmFlowGraph.h"
#include "Node.h"
#include "CodeGen.h"

using namespace std;

namespace graph {

AsmFlowGraph::AsmFlowGraph(const assem::InstructionList &instrs)
{
	makeInstructionTable(instrs);
	makeMoveTable();
	codegen = assem::CodeGen::create();
}

AsmFlowGraph::~AsmFlowGraph()
{
	delete codegen;
}

assem::Instruction *
AsmFlowGraph::instr(Node *n)
{
	InstructionNodeMap::iterator it = instTable.find(n);
	return it->second;
}

void 
AsmFlowGraph::makeInstructionTable(const assem::InstructionList &instrs)
{
	Node *prevNode = NULL;
	assem::Instruction *prevInst = NULL;
	assem::InstructionList::const_iterator it;
	it = instrs.begin();
	while (it != instrs.end()) {
		Node *node = gcnew(Node, (this));
		addNode(node);
		
		if (prevNode && !codegen->isJump(prevInst)) {
			addEdge(prevNode, node);
		}
		instTable.insert(make_pair(node, *it));
		prevInst = *it;
		prevNode = node;
		++it;
	}
}

void 
AsmFlowGraph::makeMoveTable()
{
	InstructionNodeMap::const_iterator it;
	it = instTable.begin();
	while (it != instTable.end()) {
		Node *node = it->first;
		assem::Instruction *inst = it->second;
		bool b = codegen->isMove(inst);
		moveTable.insert(make_pair(node, b));
		++it;
	}
}

}//namespace graph
