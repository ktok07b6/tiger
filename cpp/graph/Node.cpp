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
	std::merge(successors.begin(), successors.end(), 
			   predecessors.begin(), predecessors.end(), 
			  std::insert_iterator<NodeList>(adjacencies,
								   adjacencies.end()));
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
	return successors.find(n) != successors.end();
}

bool 
Node::comesFrom(Node *n) const
{
	return predecessors.find(n) != predecessors.end();
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
