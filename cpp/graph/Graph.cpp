#include "Graph.h"
#include "Node.h"

#include "HeapManager.h"
#include <assert.h>

namespace graph {

Graph::Graph()
{
}

const NodeList & 
Graph::getNodes()
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
	from->successors.insert(to);
	to->predecessors.insert(from);
}

void 
Graph::rmEdge(Node *from, Node *to)
{
	assert(from != to);
	from->successors.erase(to);
	to->predecessors.erase(from);
}

void 
Graph::show()
{
}


}//namespace graph
