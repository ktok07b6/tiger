#include "InterferenceGraph.h"
#include "Temp.h"
#include "AsmFlowGraph.h"
#include "HeapManager.h"

namespace graph {


InterferenceGraph::InterferenceGraph(const std::vector<TempList*> &liveouts)
{
	//enumerate all temps
	std::set<Temp*> temps;
	std::vector<TempList*>::const_iterator it = liveouts.begin();
	while (it != liveouts.end()) {
		TempList *liveout = (*it);
		std::copy(liveout->begin(),
				  liveout->end(),
				  std::insert_iterator< std::set<Temp*> >(temps, temps.end()));
		++it;
	}

	//create nodes for temps
	std::set<Temp*>::iterator ti = temps.begin();
	while (ti != temps.end()) {
		//Temp *pt = (*ti);
		TempNode *node = gcnew(TempNode, (this, *ti));
		Graph::addNode(node);
		temp2nodeMap.insert(std::make_pair((*ti), node)); 
		++ti;
	}

	it = liveouts.begin();
	while (it != liveouts.end()) {
		TempList *liveout = (*it);
		if (2 <= liveout->size()) {
			TempList::iterator t1 = liveout->begin();
			TempList::iterator t2 = liveout->begin();
			++t2;
			while (t2 != liveout->end()) {
				Node *n1 = temp2node(*t1);
				Node *n2 = temp2node(*t2);
				Graph::addEdge(n1, n2);
				++t1;
				++t2;
			}
		}
		++it;
	}
	

}

Node *
InterferenceGraph::temp2node(Temp *t)
{
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
	TempNode *tnode = (TempNode*)n;
	return tnode->getTemp();
}

const InterferenceGraph::NodePairList &
InterferenceGraph::moves()
{
	return movedNodes;
}

void 
InterferenceGraph::show() const
{
	DBG("InterferenceGraph ================");
	NodeList::iterator it = nodes.begin();
	while (it != nodes.end()) {
		TempNode *tnode = (TempNode*)(*it);
		Temp *t = tnode->getTemp();
		DBG("%s -->", t->toString().c_str());
		const NodeList &adj = tnode->adj();
		NodeList::const_iterator it2 = adj.begin();
		while (it2 != adj.end()) {
			TempNode *tnode2 = (TempNode*)(*it2);
			Temp *t2 = tnode2->getTemp();
			DBG("       %s", t2->toString().c_str());
			++it2;
		}
		++it;
	}
}

}//namespace graph
