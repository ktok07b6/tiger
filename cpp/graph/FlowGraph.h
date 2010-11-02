#ifndef FLOW_GRAPH_H
#define FLOW_GRAPH_H

#include "Graph.h"
#include "Temp.h"
#include <map>

namespace graph {

class FlowGraph : public Graph
{
public:
	virtual TempList def(const Node *node) const = 0;
	virtual TempList use(const Node *node) const  = 0;
	virtual bool isMove(const Node *node) const  = 0;
};

}//namespace graph
#endif
