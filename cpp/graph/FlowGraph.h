#ifndef FLOW_GRAPH_H
#define FLOW_GRAPH_H

#include "Graph.h"
#include "Temp.h"
#include <map>

namespace graph {

class FlowGraph : public Graph
{
public:
	virtual TempList def(Node *node) = 0;
	virtual TempList use(Node *node) = 0;
	virtual bool isMove(Node *node) = 0;
};

}//namespace graph
#endif
