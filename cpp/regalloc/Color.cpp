#include "Color.h"
#include "InterferenceGraph.h"

using namespace graph;

namespace regalloc {

Color::Color(const InterferenceGraph &igraph)
{
	//We repeatedly remove (and push on a stack) nodes of degree less than K.
	const NodeList &nodes = igraph.getNodes();
	NodeList::const_iterator it;
	it = nodes.begin();
	while (it != nodes.end()) {
		Node *node = *it;
		if (node->degree() < K) {
			pushToSimplifyWorkList(node);
		}
		++it;
	}
}

void
Color::pushToSimplifyWorkList(Node *node)
{
}

std::string 
Color::tempMap(Temp *temp)
{
}


}//namespace regalloc
