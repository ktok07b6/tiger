#include "InterferenceGraph.h"
#include "Temp.h"
#include "AsmFlowGraph.h"

namespace graph {

InterferenceGraph::InterferenceGraph(const AsmFlowGraph &agraph)
{
}

Node *
InterferenceGraph::temp2node(Temp *t)
{
	return NULL;
}

Temp *
InterferenceGraph::node2temp(Node *n)
{
	return NULL;
}

const InterferenceGraph::NodePairList &
InterferenceGraph::moves()
{
	return movedNodes;
}


}//namespace graph
