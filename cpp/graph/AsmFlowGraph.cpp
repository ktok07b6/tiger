#include <boost/foreach.hpp>
#include "AsmFlowGraph.h"
#include "Node.h"
#include "CodeGen.h"

using namespace std;

namespace graph {

AsmFlowGraph::AsmFlowGraph(const assem::InstructionList &instrs)
{
	codegen = assem::CodeGen::create();
	makeNodes(instrs);
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
AsmFlowGraph::makeNodes(const assem::InstructionList &instrs)
{
	Node *prevNode = NULL;
	assem::Instruction *prevInst = NULL;
	BOOST_FOREACH(assem::Instruction *inst, instrs) {
		Node *node = gcnew(InstNode, (this, inst));
		Graph::addNode(node);

		if (prevNode && !codegen->isJump(prevInst)) {
			addEdge(prevNode, node);
		}
		
		prevInst = inst;
		prevNode = node;
	}

	//

	const NodeList &nodes = Graph::getNodes();
	BOOST_FOREACH(Node *n, nodes) {
		InstNode *fromNode = (InstNode*)(n);
		assem::Instruction *inst = fromNode->getInst();
		LabelList targets = inst->jumps();
		if (!targets.empty()) {
			Label *la = targets.front();
			InstNode *toNode = findLABELNode(la);
			if (toNode) {
				addEdge(fromNode, toNode);
			}
		}
	}
}

AsmFlowGraph::InstNode *
AsmFlowGraph::findLABELNode(Label *l)
{
	const NodeList &nodes = Graph::getNodes();
	BOOST_FOREACH(Node *n, nodes) {
		InstNode *inode = (InstNode*)n;
		assem::Instruction *inst = inode->getInst();
		if (inst->isLABEL()) {
			assem::LABEL *asmlab = (assem::LABEL*)(inst);
			//TODO:文字列でなくポインタで比較したい
			//Labelで同じ名前を同じオブジェクトにすることが必要
			if (l->toString() == asmlab->label()->toString()) {
				return inode;
			}
		}
	}
	return NULL;
}


TempList 
AsmFlowGraph::def(const Node *node) const
{
	assert(node);
	assert(((const InstNode*)node)->getInst());
	return ((const InstNode*)node)->getInst()->def();
}

TempList 
AsmFlowGraph::use(const Node *node) const
{
	assert(node);
	assert(((const InstNode*)node)->getInst());
	return ((const InstNode*)node)->getInst()->use();
}

bool 
AsmFlowGraph::isMove(const Node *node) const
{
	assert(node);
	assert(((const InstNode*)node)->getInst());
	return codegen->isMove(((const InstNode*)node)->getInst());
}

void 
AsmFlowGraph::show() const
{
	DBG("AsmFlowGraph ================");
	BOOST_FOREACH(Node *n1, nodes) {
		InstNode *in = (InstNode*)(n1);
		DBG("%s", in->toString().c_str());
	}
}

std::string
AsmFlowGraph::InstNode::toString() const
{
	return "inst node:" + inst->toString();
}

}//namespace graph
