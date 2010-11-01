#ifndef NODE_H
#define NODE_H

#include "Object.h"
#include <set>
#include <string>

namespace graph {

class Graph;
class Node;
typedef std::set<Node*> NodeList;

class Node : public Object
{
public:
	virtual ~Node() {}

	Node(Graph *g);
	const NodeList &succ() const;
	const NodeList &pred() const;
	const NodeList &adj() const;
	int outDegree() const;
	int inDegree() const;
	int degree() const;
	bool goesTo(Node *n) const;
	bool comesFrom(Node *n) const;
	bool adj(Node *n) const;
	virtual std::string toString() const;

private:
	Graph *graph;
	NodeList successors;
	NodeList predecessors;
	mutable NodeList adjacencies;
	friend class Graph;
};

}//namespace graph

#endif
