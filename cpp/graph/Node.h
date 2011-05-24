#ifndef NODE_H
#define NODE_H

#include "Object.h"
#include <string>
#include "Vector.h"
#include "debug.h"

namespace graph {

class Graph;
class Node;

typedef Vector<Node> NodeList;

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

	void setTag(int tag);
	int getTag() const;

	operator const char*() {
		return toString().c_str();
	}
	virtual std::string toString() const;

private:
	void addSuccessor(Node *n);
	void addPredecessor(Node *n);
	void removeSuccessor(Node *n);
	void removePredecessor(Node *n);

	Graph *graph;
	NodeList successors;
	NodeList predecessors;
	int tag;
	mutable NodeList adjacencies;
	mutable bool invalidAdj;
	friend class Graph;
};

}//namespace graph

#endif
