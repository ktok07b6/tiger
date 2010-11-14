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
Color::build()
{
}

void 
Color::makeWorkList()
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
Color::simplify()
{
	assert(!simplifyWorkList.empty());

	Node *n = simplifyWorkList.pop_back();
	selectStack.push_back(n);

	NodeList adj = adjacent(n);
	BOOST_FOREACH(Node *a, adj) {
		decrementDegree(a);
	}
}

void
Color::decrementDegree(Node *n)
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
Color::enableMoves(const NodeList &nodes)
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
Color::addWorkList(graph::Node *n)
{
	if (!isPrecolored(n) && !isMoveRelated(n) && degreeMap[n] < K) {
		freezeWorkList.remove(n);
		simplifyWorkList.push_back(n);
	}
}

bool 
Color::ok(graph::Node *t, graph::Node *r)
{
	return degreeMap[t] < K && isPrecolored(t) && t->adj(r);
}

bool 
Color::conservative(const graph::NodeList &nodes)
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
Color::coalesce() 
{ 
	assert(!workListMoves.empty());
	//TODO:
}

void 
Color::combine(graph::Node *, graph::Node *)
{
	//TODO:
}

Node *
Color::getAlias(graph::Node *n)
{
	if (coalescedNodes.contain(n)) {
		getAlias(aliasMap[n]);
	} else {
		return n;
	}
}

void
Color::freeze()
{
}

void
Color::selectSpill()
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

NodeList
Color::nodeMoves(Node *n)
{
	NodeList moves;
	moves += activeMoves;
	moves += workListMoves;
	moves.unique();
	//moves = moves.intersection(moveList[n]);
	return moves;
}

bool
Color::isMoveRelated(Node *n)
{
	//return igraph.isMove(n) && (activeMoves.contain(n) || workListMoves.contain(n));
	return false;
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
