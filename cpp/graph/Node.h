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
    NodeList succ();
    NodeList pred();
    NodeList adj();
    int outDegree();
    int inDegree();
    int degree();
    bool goesTo(Node *n);
    bool comesFrom(Node *n);
    bool adj(Node *n);
	virtual std::string toString();

private:
	Graph *graph;
	NodeList successors;
	NodeList predecessors;
	friend class Graph;
};

}//namespace graph

#endif
