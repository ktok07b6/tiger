#ifndef GRAPH_H
#define GRAPH_H

#include <set>
#include <string>
#include "Object.h"
#include "Vector.h"

namespace graph {

class Node;
typedef Vector<Node> NodeList;

class Graph : public Object
{
public:
	Graph();
	const NodeList &getNodes() const;
	void addNode(Node *n);
	void addEdge(Node *from, Node *to);
	void rmEdge(Node *from, Node *to);

	size_t size() const;
	virtual void rmNode(Node *n);
	virtual void show() const;
protected:
	NodeList nodes;
};

}//namespace graph

#endif //GRAPH_H
