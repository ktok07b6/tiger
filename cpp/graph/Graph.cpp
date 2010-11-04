#include <boost/foreach.hpp>
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
	assert(n->graph == this);
	nodes.insert(n);
}

void
Graph::rmNode(Node *n)
{
	assert(n->graph == this);
	const NodeList &succ = n->succ();
	BOOST_FOREACH(Node *to, succ) {
		rmEdge(n, to);
	}
	const NodeList &pred = n->pred();
	BOOST_FOREACH(Node *from, pred) {
		rmEdge(from, n);
	}
	nodes.erase(n);
}

void 
Graph::addEdge(Node *from, Node *to)
{
	assert(from->graph == this);
	assert(to->graph == this);
	assert(from != to);
	if (from->successors.find(to) == from->successors.end()) {
		if (to->successors.find(from) != to->successors.end()) {
			WARN(">>>>>> edge is crossover <<<<<<");
			return;
		}
		from->successors.insert(to);
	}
	if (to->predecessors.find(from) == to->predecessors.end()) {
		if (from->predecessors.find(to) != from->predecessors.end()) {
			WARN(">>>>>> edge is crossover <<<<<<");
			return;
		}
		to->predecessors.insert(from);
	}
}

void 
Graph::rmEdge(Node *from, Node *to)
{
	assert(from->graph == this);
	assert(to->graph == this);
	assert(from != to);
	from->successors.erase(to);
	to->predecessors.erase(from);
}

void 
Graph::show() const
{
}


}//namespace graph
