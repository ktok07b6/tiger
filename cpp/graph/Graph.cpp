#include "Graph.h"
#include "Node.h"

#include "HeapManager.h"
#include <assert.h>
#include "debug.h"

namespace graph {

Graph::Graph()
{
}

const NodeList & 
Graph::getNodes() const
{
	return nodes;
}

void
Graph::addNode(Node *n)
{
	nodes.insert(n);
}
void 
Graph::addEdge(Node *from, Node *to)
{
	assert(from != to);
	if (from->successors.find(to) == from->successors.end()) {
		if (to->successors.find(from) != to->successors.end()) {
			WARN(">>>>>> edge is crossover <<<<<<");
			return;
		}
		from->successors.insert(to);
	} else {
		WARN("already added");
	}
	if (to->predecessors.find(from) == to->predecessors.end()) {
		if (from->predecessors.find(to) != from->predecessors.end()) {
			WARN(">>>>>> edge is crossover <<<<<<");
			return;
		}
		to->predecessors.insert(from);
	} else {
		WARN("already added");
	}
}

void 
Graph::rmEdge(Node *from, Node *to)
{
	assert(from != to);
	from->successors.erase(to);
	to->predecessors.erase(from);
}

void 
Graph::show() const
{
}


}//namespace graph
