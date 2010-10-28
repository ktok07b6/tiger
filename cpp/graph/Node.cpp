#include "Node.h"
#include <algorithm>

namespace graph {
Node::Node(Graph *g)
	: graph(g)
{
}

NodeList 
Node::succ()
{
	return successors;
}

NodeList 
Node::pred()
{
	return predecessors;
}

NodeList 
Node::adj()
{
	NodeList adjacencies;
	std::merge(successors.begin(), successors.end(), 
			   predecessors.begin(), predecessors.end(), 
			  std::insert_iterator<NodeList>(adjacencies,
								   adjacencies.end()));
	return adjacencies;
}

int 
Node::outDegree()
{
	return successors.size();
}

int 
Node::inDegree()
{
	return predecessors.size();
}

int 
Node::degree()
{
	return outDegree() + inDegree();
}

bool 
Node::goesTo(Node *n)
{
	return successors.find(n) != successors.end();
}

bool 
Node::comesFrom(Node *n)
{
	return predecessors.find(n) != predecessors.end();
}

bool 
Node::adj(Node *n)
{
	return goesTo(n) || comesFrom(n);
}

std::string 
Node::toString()
{
	return "";
}

}//namespace graph
