#include <boost/foreach.hpp>
#include "Color.h"
#include "InterferenceGraph.h"

using namespace graph;

namespace regalloc {

Color::Color(const InterferenceGraph &ig)
	: igraph(ig)
{
	//We repeatedly remove (and push on a stack) nodes of degree less than K.
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

std::string 
Color::tempMap(Temp *temp)
{
	return "";
}


void
Color::coalesce() 
{ 
	const InterferenceGraph::NodePairList &nodePairs = igraph.moves();
	BOOST_FOREACH(const InterferenceGraph::NodePair n, nodePairs) {
		Node *src = n.first;
		Node *dst = n.second;
		if (!src->adj(dst)) {
			merge(src, dst);
		}
	} 
}

void
Color::merge(Node *n1, Node *n2)
{
	//copy succ & pred nodes
	NodeList succ = n2->succ();
	NodeList pred = n2->pred();
	Temp *t2 = igraph.node2temp(n2);

	igraph.rmNode(n2);

	BOOST_FOREACH(Node *other, succ) {
		igraph.addEdge(n1, other);
	} 
	BOOST_FOREACH(Node *other, pred) {
		igraph.addEdge(other, n1);
	}
	

	//rewrite program
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
	/*
	BOOST_FOREACH(r, regs) {
		const NodeList &rnodes = r->getNodes();
		adj = n->adj();
		if (!rnodes->contain(adj)) {
			rnodes->push(n);
			return true;
		} 
	} 
	*/
	return false; 
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



}//namespace regalloc
