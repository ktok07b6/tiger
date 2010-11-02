#ifndef GRAPH_H
#define GRAPH_H

#include <set>
#include <string>
#include "Object.h"

namespace graph {

class Node;
typedef std::set<Node*> NodeList;

class Graph : public Object
{
public:
	Graph();
	const NodeList &getNodes() const;
	void addNode(Node *n);
	void addEdge(Node *from, Node *to);
	void rmEdge(Node *from, Node *to);

	virtual void rmNode(Node *n);
	virtual void show() const;
protected:
	NodeList nodes;
};

}//namespace graph

#endif //GRAPH_H
