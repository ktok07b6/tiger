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
	InstNode *inode = (InstNode*)n;
	return inode->getInst();
}

void 
AsmFlowGraph::makeInstructionTable(const assem::InstructionList &instrs)
{
	Node *prevNode = NULL;
	assem::Instruction *prevInst = NULL;
	assem::InstructionList::const_iterator it;
	it = instrs.begin();
	while (it != instrs.end()) {
		Node *node = gcnew(InstNode, (this));
		Graph::addNode(node);

		if (prevNode && !codegen->isJump(prevInst)) {
			addEdge(prevNode, node);
		}
		
		prevInst = *it;
		prevNode = node;
		++it;
	}

	//
	const NodeList &nodes = Graph::getNodes();
	NodeList::iterator it2;
	it2 = nodes.begin();
	while (it2 != nodes.end()) {
		InstNode *fromNode = (InstNode*)(*it2);
		assem::Instruction *inst = fromNode->getInst();
		LabelList targets = inst->jumps();
		if (!targets.empty()) {
			Label *la = targets.front();
			InstNode *toNode = findLABELNode(la);
			if (toNode) {
				addEdge(fromNode, toNode);
			}
		}
		++it2;
	}
}

void 
AsmFlowGraph::makeMoveTable()
{
	const NodeList &nodes = Graph::getNodes();
	NodeList::const_iterator it;
	it = nodes.begin();
	while (it != nodes.end()) {
		InstNode *inode = (InstNode*)(*it);
		bool b = codegen->isMove(inode->getInst());
		++it;
	}
}

AsmFlowGraph::InstNode *
AsmFlowGraph::findLABELNode(Label *l)
{
	const NodeList &nodes = Graph::getNodes();
	NodeList::const_iterator it;
	it = nodes.begin();
	while (it != nodes.end()) {
		InstNode *inode = (InstNode*)(*it);
		assem::Instruction *inst = inode->getInst();
		if (inst->isLABEL()) {
			assem::LABEL *asmlab = (assem::LABEL*)(inst);
			//TODO:文字列でなくポインタで比較したい
			//Labelで同じ名前を同じオブジェクトにすることが必要
			if (l->toString() == asmlab->label()->toString()) {
				return inode;
			}
		}
		++it;
	}
	return NULL;
}

}//namespace graph
