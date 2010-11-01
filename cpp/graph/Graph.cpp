#include "Graph.h"
#include "Node.h"

#include "HeapManager.h"
#include <assert.h>

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
		from->successors.insert(to);
	}
	if (to->successors.find(from) == to->successors.end()) {
		to->predecessors.insert(from);
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
