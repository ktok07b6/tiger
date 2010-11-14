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
	typedef std::map<Node *, NodeList> MoveList;
	typedef std::pair<Node *, Node *> NodePair;
	typedef std::vector<NodePair> Moves;

	InterferenceGraph();
	void newNode(Temp *t);
	Node *temp2node(Temp *t);
	const TempList &node2temp(Node *n);
	int node2nid(Node *n);
	Node *nid2node(int nid);

	void addMoveEdge(Node *n1, Node *n2);
	const Moves &getMoves() const;

	virtual void show() const;

 private:
	void addMoveEdge_(Node *from, Node *to);

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
	
	std::map<Temp *, TempNode *> temp2nodeMap;
	Moves moves;
};

}//namespace graph
#endif
