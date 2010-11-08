#include <boost/foreach.hpp>
#include "Color.h"
#include "InterferenceGraph.h"
#include "tiger.h"
#include <algorithm>

using namespace graph;

namespace regalloc {

Color::Color(const InterferenceGraph &ig, const TempList &regs)
	: igraph(ig)
	, regs(regs)
{

	//precolor
	int i = 0;

	BOOST_FOREACH(Temp *r, regs) {
		Node *n = igraph.temp2node(r);
		if (n) {
			//DBG("%s is precolored on %p %d", r->toString().c_str(), n, i);
			coloredNodes[i].push_back(n);
		}
		++i;
	}

	build();
	makeWorkList();

	//We repeatedly remove (and push on a stack) nodes of degree less than K.
	//simplify
retry:
	
	if (isEnableColoring()) {
		coloring(); 
	} else { 
		if (coalesce()) { 
			goto retry;
		}
		//TODO:
		//freeze
		//spill 
	}  

	//select
	while (!simplifyWorks.empty()) { 
		Node *n = popFromSimplifyWorks();
		bool b = setColor(n);
		assert(b);
	} 
	//TODO:
	//Is all nodes colored?
	//rewrite program
}


void 
Color::build()
{
}

void 
Color::makeWorkList()
{
	const NodeList &nodes = igraph.getNodes();
	BOOST_FOREACH(Node *n, nodes) {
		if (K <= n->degree()) {
			pushToSpillWorkList(n);
		}
		else if (isMoveRelated(n)) {
			pushToFreezeWorkList(n);
		}
		else {
			pushToSimplifyWorkList(n);
		} 
	} 
}

void 
Color::simplify()
{
}

bool
Color::coalesce() 
{ 
}

void
Color::assignColors() 
{ 
}

bool 
Color::setColor(Node *n) 
{
} 

bool
Color::isPrecolored(Node *n)
{
	TempList t = igraph.node2temp(n);
	assert(!t.empty());
	return std::find(regs.begin(), regs.end(), t.front()) != regs.end();
}

int
Color::getColoredIndex(Node *n)
{
	int index = 0;
	BOOST_FOREACH(NodeList &colored, coloredNodes) {
		if (colored.contain(n)) {
			return index;
		}
		++index;
	}
	return -1;
}

bool 
Color::isEnableColoring() const
{
} 

void
Color::pushToSimplifyWorkList(Node *node) 
{
	node->setState(SIMPLIFY);
}

void
Color::pushToFreezeWorkList(Node *node) 
{
	node->setState(FREEZE);
}  

void
Color::pushToSpillWorkList(Node *node) 
{
	node->setState(SPILL);
}  

NodeList 
Color::adjacent(Node *n)
{
	NodeList adj = n->adj();
	NodeList tmp;
	tmp = selectStack;
	tmp += coalescedNodes;
	tmp.unique();
	adj -=  tmp;
	return adj;
}

bool
Color::isMoveRelated(Node *n)
{
	return igraph.isMove(n) && (activeMoves.contain(n) || workListMoves.contain(n));
}

std::string 
Color::tempMap(Temp *temp)
{
	//TODO: Do not depend on a specific target
	Node *n = igraph.temp2node(temp);
	int regnum = getColoredIndex(n);
	if (regnum == -1) {
		//ERROR("%s is not allocated to a register", temp->toString().c_str());
		return "";
	}
	return regs[regnum]->toString();
}



}//namespace regalloc
