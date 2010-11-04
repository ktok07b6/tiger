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
	const TempList &node2temp(Node *n);

	const NodePairList &moves();
	void addMove(const NodePair &nodes);

	void coalesce(Node *n1, Node *n2);
	virtual void show() const;

 private:
	class TempNode : public Node
	{
	public:
	TempNode(InterferenceGraph *g, Temp *t) : Node(g) {
			addTemp(t);
		}
		const TempList &getTemp() const { return temps; }
		void addTemp(Temp *t) {
			temps.push_back(t);
		}
	virtual std::string toString() const;
	private:
		TempList temps;
	};


	NodePairList movedNodes;
	std::map<Temp *, TempNode *> temp2nodeMap;
};

}//namespace graph
#endif
