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
			int nid = igraph.node2nid(n);
			//DBG("%s is precolored on %p %d", r->toString().c_str(), n, i);
			coloredNodes[i].set(nid);
		}
		++i;
	}

 retry:
	build();
	makeWorkList();
	do {
		if (!simplifyWorkList.none()) {
			simplify();
		}
		else if (!workListMoves.none()) {
			coalesce();
		}
		else if (!freezeWorkList.none()) {
			freeze();
		}
		else if (!spillWorkList.none()) {
			selectSpill();
		}
	} while (!simplifyWorkList.none() ||
			 !workListMoves.none() ||
			 !freezeWorkList.none() ||
			 !spillWorkList.none());

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
	int i = 0;
	const NodeList &nodes = igraph.getNodes();
	BOOST_FOREACH(Node *n, nodes) {
		if (K <= n->degree()) {
			spillWorkList.set(i);
		}
		else if (isMoveRelated(i)) {
			freezeWorkList.set(i);
		}
		else {
			simplifyWorkList.set(i);
		} 
		++i;
	} 
}

void 
Color2::simplify()
{
	assert(!simplifyWorkList.none());

	int nid = simplifyWorkList.right();
	selectStack.set(nid);

	Bitmap adj = adjacent(nid);
	for (int i = 0; i < adj.size(); ++i) {
		if (adj.get(i)) {
			decrementDegree(i);
		}
	}
}

void
Color2::decrementDegree(int nid)
{
	int degree = degreeMap[nid];
	degreeMap[nid] = degree - 1;
	if (degree == K) {
		Bitmap nodes = adjacent(nid);
		nodes += nid;
		enableMoves(nodes);
		spillWorkList.reset(nid);
		if (isMoveRelated(nid)) {
			freezeWorkList.set(nid);
		} else {
			simplifyWorkList.set(nid);
		}
	}
}

void
Color2::enableMoves(const Bitmap &nodes)
{
	for (int nid = 0; nid < adj.size(); ++nid) {
		if (!adj.get(nid)) {
			continue;
		}
		Bitmap moves = nodeMoves(nid);
		for (int m = 0; m < moves.size(); ++m) {
			if (!moves.get(m)) {
				continue;
			}
			if (activeMoves.get(m)) {
				activeMoves.reset(m);
				workListMoves.set(m);
			}
		}
	}
	
}

void 
Color2::addWorkList(int nid)
{
	if (!isPrecolored(nid) && !isMoveRelated(nid) && degreeMap[nid] < K) {
		freezeWorkList.reset(nid);
		simplifyWorkList.set(nid);
	}
}

bool 
Color2::ok(int nid1, int nid2)
{
	return degreeMap[nid1] < K && isPrecolored(nid1) && t->adj(nid2);
}

bool 
Color2::conservative(const Bitmap &nodes)
{
	int k = 0;
	for (int nid = 0; nid < adj.size(); ++nid) {
		if (!adj.get(nid)) {
			continue;
		}
		if (degreeMap[nid] >= K) {
			++k;
		}
	}
	return (k < K);
}

bool
Color2::coalesce() 
{ 
	assert(!workListMoves.none());
	//TODO:
}

void 
Color2::combine(int nid1, int nid2)
{
	//TODO:
}

int
Color2::getAlias(int nid)
{
	if (coalescedNodes.get(nid)) {
		getAlias(aliasMap[nid]);
	} else {
		return nid;
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
Color2::setColor(int nid) 
{
} 

bool
Color2::isPrecolored(int nid)
{
}

int
Color2::getColoredIndex(int nid)
{
	int index = 0;
	BOOST_FOREACH(Bitmap &colored, coloredNodes) {
		if (colored.get(nid)) {
			return index;
		}
		++index;
	}
	return -1;
}


Bitmap
Color2::adjacent(int nid)
{
	/*
	Bitmap adj = n->adj();
	Bitmap tmp;
	tmp = selectStack | coalescedNodes;
	adj -=  tmp;
	return adj;
	*/
}

Bitmap
Color2::nodeMoves(int nid)
{
	return moveList[nid] & activeMoves | workListMoves;
}

bool
Color2::isMoveRelated(int nid)
{
	return igraph.isMove(nid) && (activeMoves.get(nid) || workListMoves.get(nid));
}

std::string 
Color2::tempMap(Temp *temp)
{
	//TODO: Do not depend on a specific target
	Node *n = igraph.temp2node(temp);
	int nid = igraph.node2nid(n);
	int regnum = getColoredIndex(nid);
	if (regnum == -1) {
		//ERROR("%s is not allocated to a register", temp->toString().c_str());
		return "";
	}
	return regs[regnum]->toString();
}



}//namespace regalloc
