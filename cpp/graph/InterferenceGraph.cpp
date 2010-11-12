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

const TempList &
InterferenceGraph::node2temp(Node *n)
{
	assert(n);
	TempNode *tnode = (TempNode*)n;
	return tnode->getTemp();
}

int
InterferenceGraph::node2nid(Node *n)
{
	assert(n);
	NodeList::iterator it = nodes.find(n);
	return std::distance(nodes.begin(), it);
}

const InterferenceGraph::NodePairList &
InterferenceGraph::moves()
{
	return movedNodes;
}

void 
InterferenceGraph::addMove(const NodePair &nodes)
{
	movedNodes.push_back(nodes);
	((TempNode*)nodes.first)->setMove(true);
	((TempNode*)nodes.second)->setMove(true);
}

bool
InterferenceGraph::isMove(Node *node)
{
	return ((TempNode*)node)->isMove();
}

void
InterferenceGraph::coalesce(Node *n1, Node *n2)
{
	//TODO: must be conservative

	//assert(n1->graph == this && n2->graph == this);
	TempNode *tn1 = (TempNode*)n1;
	TempNode *tn2 = (TempNode*)n2;

	//copy succ & pred nodes
	NodeList succ = tn2->succ();
	NodeList pred = tn2->pred();

	rmNode(n2);

	BOOST_FOREACH(Node *other, succ) {
		addEdge(n1, other);
	} 
	BOOST_FOREACH(Node *other, pred) {
		addEdge(other, n1);
	}

	const TempList &tlist = tn2->getTemp();
	BOOST_FOREACH(Temp *t, tlist) {
		//replace TempNode
		std::map<Temp*, TempNode*>::iterator it;
		it = temp2nodeMap.find(t);
		assert(it != temp2nodeMap.end());
		it->second = tn1; 
	
		tn1->addTemp(t);
	}
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
	std::string str;
	BOOST_FOREACH(Temp *t, temps) {
		str += t->toString() + " ";
	}
	return str;
}

}//namespace graph
