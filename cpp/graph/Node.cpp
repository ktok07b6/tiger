#include "Node.h"
#include <algorithm>

namespace graph {
Node::Node(Graph *g)
	: graph(g)
{
}

const NodeList & 
Node::succ() const
{
	return successors;
}

const NodeList & 
Node::pred() const
{
	return predecessors;
}

const NodeList & 
Node::adj() const
{
	//TODO: caching adjacencies
	adjacencies.clear();
	adjacencies.push_all(successors);
	adjacencies.push_all(predecessors);
	adjacencies.unique();

	return adjacencies;
}

int 
Node::outDegree() const
{ 
	return successors.size();
}

int 
Node::inDegree() const
{
	return predecessors.size();
}

int 
Node::degree() const
{
	return outDegree() + inDegree();
}

bool 
Node::goesTo(Node *n) const 
{
	return successors.contain(n);
}

bool 
Node::comesFrom(Node *n) const
{
	return predecessors.contain(n);
}

bool 
Node::adj(Node *n) const
{
	return goesTo(n) || comesFrom(n);
}

std::string 
Node::toString() const
{
	return "";
}

}//namespace graph
