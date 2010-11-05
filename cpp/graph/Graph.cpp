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
	nodes.push_back(n);
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
	nodes.remove(n);
}

void 
Graph::addEdge(Node *from, Node *to)
{
	assert(from->graph == this);
	assert(to->graph == this);
	assert(from != to);
	if (!from->successors.contain(to)) {
		if (to->successors.contain(from)) {
			WARN(">>>>>> edge is crossover <<<<<<");
			return;
		}
		from->successors.push_back(to);
	}
	if (!to->predecessors.contain(from)) {
		if (from->predecessors.contain(to)) {
			WARN(">>>>>> edge is crossover <<<<<<");
			return;
		}
		to->predecessors.push_back(from);
	}
}

void 
Graph::rmEdge(Node *from, Node *to)
{
	assert(from->graph == this);
	assert(to->graph == this);
	assert(from != to);
	from->successors.remove(to);
	to->predecessors.remove(from);
}

void 
Graph::show() const
{
}


}//namespace graph
