#ifndef INTERFERENCE_GRAPH_H
#define INTERFERENCE_GRAPH_H

#include "Graph.h"
#include "Node.h"
#include <list>
#include <map>

class Temp;

namespace graph {

class AsmFlowGraph;
class InterferenceGraph : public Graph
{
public:
	typedef std::pair<Node*, Node*> NodePair;
	typedef std::list<NodePair> NodePairList;

	InterferenceGraph(const AsmFlowGraph &agraph);
	Node *temp2node(Temp *t);
	Temp *node2temp(Node *n);
	const NodePairList &moves();

 private:
	class TempNode : public Node
	{
	public:
	TempNode(InterferenceGraph *g, Temp *t) : Node(g), temp(t) {}
		Temp *getTemp() { return temp; }
	private:
		Temp *temp;
	};
	NodePairList movedNodes;
};

}//namespace graph
#endif
