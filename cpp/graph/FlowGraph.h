#ifndef FLOW_GRAPH_H
#define FLOW_GRAPH_H

#include "Graph.h"
#include "Temp.h"
#include <map>

namespace graph {

typedef std::map<Node *, TempList> TempListNodeMap;
typedef std::map<Node *, bool> BoolNodeMap;

class FlowGraph : public Graph
{
public:
	virtual TempList def(Node *node);
	virtual TempList use(Node *node);
	virtual bool isMove(Node *node);

	TempListNodeMap defsTable;
	TempListNodeMap usesTable;
	BoolNodeMap moveTable;
};

}//namespace graph
#endif
