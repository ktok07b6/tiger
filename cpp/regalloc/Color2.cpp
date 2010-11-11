#include <boost/foreach.hpp>
#include "Color2.h"
#include "InterferenceGraph.h"
#include "tiger.h"
#include <algorithm>

using namespace graph;

namespace regalloc {

Color2::Color2(const InterferenceGraph &ig, const TempList &regs)
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

 retry:
	build();
	makeWorkList();
	do {
		if (!simplifyWorkList.empty()) {
			simplify();
		}
		else if (!workListMoves.empty()) {
			coalesce();
		}
		else if (!freezeWorkList.empty()) {
			freeze();
		}
		else if (!spillWorkList.empty()) {
			selectSpill();
		}
	} while (!simplifyWorkList.empty() ||
			 !workListMoves.empty() ||
			 !freezeWorkList.empty() ||
			 !spillWorkList.empty());

	assignColors();

	if (!spilledNodes.empty()) {
		//TODO:
	}
}


void 
Color2::build()
{
}

void 
Color2::makeWorkList()
{
	const NodeList &nodes = igraph.getNodes();
	BOOST_FOREACH(Node *n, nodes) {
		if (K <= n->degree()) {
			spillWorkList.push_back(n);
		}
		else if (isMoveRelated(n)) {
			freezeWorkList.push_back(n);
		}
		else {
			simplifyWorkList.push_back(n);
		} 
	} 
}

void 
Color2::simplify()
{
	assert(!simplifyWorkList.empty());

	Node *n = simplifyWorkList.pop_front();
	selectStack.push_back(n);

	NodeList adj = adjacent(n);
	BOOST_FOREACH(Node *a, adj) {
		decrementDegree(a);
	}
}

void
Color2::decrementDegree(Node *n)
{
	int degree = degreeMap[n];
	degreeMap[n] = degree - 1;
	if (degree == K) {
		NodeList nodes = adjacent(n);
		nodes += n;
		enableMoves(nodes);
		spillWorkList.remove(n);
		if (isMoveRelated(n)) {
			freezeWorkList.push_back(n);
		} else {
			simplifyWorkList.push_back(n);
		}
	}
}

void
Color2::enableMoves(const NodeList &nodes)
{
	BOOST_FOREACH(Node *n, nodes) {
		NodeList moves = nodeMoves(n);
		BOOST_FOREACH(Node *m, moves) {
			if (activeMoves.contain(m)) {
				activeMoves.remove(m);
				workListMoves.push_back(m);
			}
		}
	}
}

void 
Color2::addWorkList(graph::Node *n)
{
	if (!isPrecolored(n) && !isMoveRelated(n) && degreeMap[n] < K) {
		freezeWorkList.remove(n);
		simplifyWorkList.push_back(n);
	}
}

bool 
Color2::ok(graph::Node *t, graph::Node *r)
{
	return degreeMap[t] < K && isPrecolored(t) && t->adj(r);
}

bool 
Color2::conservative(const graph::NodeList &nodes)
{
	int k = 0;
	BOOST_FOREACH(Node *n, nodes) {
		if (degreeMap[n] >= K) {
			++k;
		}
	}
	return (k < K);
}

bool
Color2::coalesce() 
{ 
	assert(!workListMoves.empty());
	//TODO:
}

void 
Color2::combine(graph::Node *, graph::Node *)
{
	//TODO:
}

Node *
Color2::getAlias(graph::Node *n)
{
	if (coalescedNodes.contain(n)) {
		getAlias(aliasMap[n]);
	} else {
		return n;
	}
}

void
Color2::freeze()
{
}

void
Color2::selectSpill()
{
}

void
Color2::assignColors() 
{ 
}

bool 
Color2::setColor(Node *n) 
{
} 

bool
Color2::isPrecolored(Node *n)
{
	TempList t = igraph.node2temp(n);
	assert(!t.empty());
	return std::find(regs.begin(), regs.end(), t.front()) != regs.end();
}

int
Color2::getColoredIndex(Node *n)
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


NodeList 
Color2::adjacent(Node *n)
{
	NodeList adj = n->adj();
	NodeList tmp;
	tmp = selectStack;
	tmp += coalescedNodes;
	tmp.unique();
	adj -=  tmp;
	return adj;
}

NodeList
Color2::nodeMoves(Node *n)
{
	NodeList moves;
	moves += activeMoves;
	moves += workListMoves;
	moves.unique();
	moves = moves.intersection(moveList[n]);
	return moves;
}

bool
Color2::isMoveRelated(Node *n)
{
	return igraph.isMove(n) && (activeMoves.contain(n) || workListMoves.contain(n));
}

std::string 
Color2::tempMap(Temp *temp)
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
