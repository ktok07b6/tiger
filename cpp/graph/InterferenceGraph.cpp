#include <boost/foreach.hpp>
#include "InterferenceGraph.h"
#include "Temp.h"
#include "AsmFlowGraph.h"
#include "HeapManager.h"

namespace graph {


InterferenceGraph::InterferenceGraph()
{
}


void
InterferenceGraph::newNode(Temp *t)
{
	TempNode *node = gcnew(TempNode, (this, t));
	Graph::addNode(node);
	temp2nodeMap.insert(std::make_pair(t, node)); 
}

Node *
InterferenceGraph::temp2node(Temp *t)
{
	assert(t);
	std::map<Temp*, TempNode*>::iterator it;
	it = temp2nodeMap.find(t);
	if (it != temp2nodeMap.end()) {
		return (Node*)it->second;
	}
	return NULL;
}

Temp *
InterferenceGraph::node2temp(Node *n)
{
	assert(n);
	TempNode *tnode = (TempNode*)n;
	return tnode->getTemp();
}

int
InterferenceGraph::node2nid(Node *n)
{
	return n->getTag();
}

Node *
InterferenceGraph::nid2node(int nid)
{
	assert(0 <= nid && nid < (int)nodes.size());
	Node *n = nodes.at(nid);
	assert(n->getTag() == nid);
	return n;
}

void 
InterferenceGraph::addMoveEdge(Node *n1, Node *n2)
{
	moves.push_back(std::make_pair(n1, n2));
}

const InterferenceGraph::Moves &
InterferenceGraph::getMoves() const
{
	return moves;
}

void 
InterferenceGraph::show() const
{
	DBG("InterferenceGraph ================");
	BOOST_FOREACH(Node *n1, nodes) {
		TempNode *tn1 = (TempNode*)(n1);

		//from
		DBG("%s-->", tn1->toString().c_str());

		//to
		const NodeList &adj = tn1->adj();
		BOOST_FOREACH(Node *n2, adj) {
			TempNode *tn2 = (TempNode*)(n2);
			DBG("       %s", tn2->toString().c_str());
		}
	}
}

std::string 
InterferenceGraph::TempNode::toString() const
{
	return temp->toString();
}

}//namespace graph
