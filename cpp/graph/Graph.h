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
	NodeList getNodes();
	void addNode(Node *n);
	void addEdge(Node *from, Node *to);
	void rmEdge(Node *from, Node *to);
	void show();
private:
	NodeList nodes;
};

}//namespace graph

#endif //GRAPH_H
