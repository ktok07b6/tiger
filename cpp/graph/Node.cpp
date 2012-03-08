#include "Node.h"
#include <algorithm>

namespace graph {
Node::Node(Graph *g)
	: graph(g)
	, tag(0)
	, invalidAdj(true)
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
	if (invalidAdj) {
		adjacencies.clear();
		adjacencies.push_all(successors);
		adjacencies.push_all(predecessors);
		adjacencies.unique();
		invalidAdj = false;
	}
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

void
Node::setTag(uintptr_t t)
{
	tag = t;
}

int
Node::getTag() const
{
	return tag;
}

std::string 
Node::toString() const
{
	return "";
}

void 
Node::addSuccessor(Node *n)
{
	successors.push_back(n);
	invalidAdj = true;
}

void 
Node::addPredecessor(Node *n)
{
	predecessors.push_back(n);
	invalidAdj = true;
}

void 
Node::removeSuccessor(Node *n)
{
	successors.remove(n);
	invalidAdj = true;
}

void 
Node::removePredecessor(Node *n)
{
	predecessors.remove(n);
	invalidAdj = true;
}

}//namespace graph
