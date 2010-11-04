#include <boost/foreach.hpp>
#include "Color.h"
#include "InterferenceGraph.h"
#include "tiger.h"

using namespace graph;

namespace regalloc {

Color::Color(const InterferenceGraph &ig, const TempList &regs)
	: igraph(ig)
{
	//We repeatedly remove (and push on a stack) nodes of degree less than K.

	//precolor
	int i = 0;
	BOOST_FOREACH(Temp *r, regs) {
		Node *n = igraph.temp2node(r);
		if (n) {
			coloredNodes[i].insert(n);
		}
		++i;
	}

	//simplify
retry:
	const NodeList &nodes = igraph.getNodes();
	BOOST_FOREACH(Node *n, nodes) {
		if (isEnableColoring()) {
			break; 
		}
		if (n->degree() < K) {
			pushToSimplifyWorks(n);
		} 
	} 
	
	if (isEnableColoring()) {
		coloring(); 
	} else { 
		coalesce(); 
		goto retry;
		//TODO:
		//spill 
	}  

	while (!simplifyWorks.empty()) { 
		Node *n = popFromSimplifyWorks();
		bool b = setColor(n);
		assert(b);
	} 

}

void
Color::coalesce() 
{ 
	const InterferenceGraph::NodePairList &nodePairs = igraph.moves();
	BOOST_FOREACH(const InterferenceGraph::NodePair n, nodePairs) {
		Node *src = n.first;
		Node *dst = n.second;
		if (!src->adj(dst)) {
			igraph.coalesce(src, dst);
		}
	} 
}

void
Color::coloring() 
{ 
	const NodeList &nodes = igraph.getNodes();
	BOOST_FOREACH(Node *n, nodes) {
		bool b = setColor(n);
		assert(b);
	} 
}

bool 
Color::setColor(Node *n) 
{
	BOOST_FOREACH(NodeList &colored, coloredNodes) {
		const NodeList &adj = n->adj();
		bool conflict = false;
		BOOST_FOREACH(Node *n, adj) {
			conflict |= (colored.find(n) != colored.end());
		}
		if (!conflict) {
			colored.insert(n);
			return true;
		}
	} 
	return false; 
} 

int
Color::getColoredIndex(Node *n)
{
	int index = 0;
	BOOST_FOREACH(NodeList &colored, coloredNodes) {
		if (colored.find(n) != colored.end()) {
			return index;
		}
		++index;
	}
	return -1;
}

bool 
Color::isEnableColoring() const
{
	bool failed = false;
	const NodeList &nodes = igraph.getNodes();
	
	BOOST_FOREACH(const Node *n, nodes) {
		if (n->degree() >= K) {
			failed |= true;
		}
	} 
	return !failed; 
} 

void
Color::pushToSimplifyWorks(Node *node) 
{
	simplifyWorks.push(node);
	const NodeList &pred = node->pred();

	BOOST_FOREACH(Node *n, pred) {
		igraph.rmEdge(n, node);
	} 
	const NodeList &succ = node->succ();
	BOOST_FOREACH(Node *n, succ) {
		igraph.rmEdge(node, n);
	} 
}  

Node *
Color::popFromSimplifyWorks() 
{
	Node *node = simplifyWorks.top(); 
	simplifyWorks.pop();
	const NodeList &pred = node->pred();

	BOOST_FOREACH(Node *n, pred) {
		igraph.addEdge(n, node);
	}
	const NodeList &succ = node->succ();
	BOOST_FOREACH(Node *n, succ) { 
		igraph.addEdge(node, n);
	} 
	return node; 
}


std::string 
Color::tempMap(Temp *temp)
{
	//TODO: Do not depend on a specific target
	Node *n = igraph.temp2node(temp);
	int regnum = getColoredIndex(n);
	return format("r%d", regnum);
}



}//namespace regalloc
