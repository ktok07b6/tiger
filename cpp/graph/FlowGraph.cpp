#include "FlowGraph.h"

namespace graph {

TempList 
FlowGraph::def(Node *node)
{
	TempListNodeMap::iterator it = defsTable.find(node);
	return it->second;
}

TempList 
FlowGraph::use(Node *node)
{
	TempListNodeMap::iterator it = usesTable.find(node);
	return it->second;

}

bool 
FlowGraph::isMove(Node *node)
{
	BoolNodeMap::iterator it = moveTable.find(node);
	return it->second;
}

}//namespace graph
