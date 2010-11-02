#ifndef INTERFERENCE_GRAPH_H
#define INTERFERENCE_GRAPH_H

#include "Graph.h"
#include "Node.h"
#include <list>
#include <map>
#include <vector>
#include "Temp.h"

namespace graph {

class AsmFlowGraph;

class InterferenceGraph : public Graph
{
public:
	typedef std::pair<Node*, Node*> NodePair;
	typedef std::list<NodePair> NodePairList;

	InterferenceGraph(const std::vector<TempList*> &liveouts);
	Node *temp2node(Temp *t);
	Temp *node2temp(Node *n);
	const NodePairList &moves();
	void addMove(const NodePair &nodes);

	virtual void show() const;

	class TempNode : public Node
	{
	public:
	TempNode(InterferenceGraph *g, Temp *t) : Node(g), temp(t) {}
		Temp *getTemp() { return temp; }
	private:
		Temp *temp;
	};

 private:
	NodePairList movedNodes;
	std::map<Temp *, TempNode *> temp2nodeMap;
};

}//namespace graph
#endif
