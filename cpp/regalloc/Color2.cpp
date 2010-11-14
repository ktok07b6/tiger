#include <boost/foreach.hpp>
#include "Color2.h"
#include "InterferenceGraph.h"
#include "tiger.h"
#include <algorithm>

using namespace graph;

namespace regalloc {

Color2::Color2(const InterferenceGraph &ig, const TempList &regs)
	: tempSize(ig.size())
	, igraph(ig)
	, regs(regs)
	, simplifyWorkList(tempSize)
	, freezeWorkList(tempSize)
	, spillWorkList(tempSize)
	, spilledNodes(tempSize)
	, coalescedNodes(tempSize)
	, coloredNodes(tempSize)
	, coalescedMoves()
	, constrainedMoves()
	, frozenMoves()
	, workListMoves()
	, activeMoves()
	, precolored(tempSize)
{
}

void 
Color2::coloring()
{
	build();
	makeWorkList();
	do {
		if (!simplifyWorkList.none()) {
			simplify();
		}
		else if (!workListMoves.empty()) {
			coalesce();
		}
		else if (!freezeWorkList.none()) {
			freeze();
		}
		else if (!spillWorkList.none()) {
			selectSpill();
		}
	} while (!simplifyWorkList.none() ||
			 !workListMoves.empty() ||
			 !freezeWorkList.none() ||
			 !spillWorkList.none());

	assignColors();

	if (!spilledNodes.none()) {
		//TODO:
	}
}

void 
Color2::build()
{
	const NodeList &nodes = igraph.getNodes();
	BOOST_FOREACH(Node *n, nodes) {
		int nid = igraph.node2nid(n);
		degreeMap[nid] = n->degree();
		aliasMap[nid] = -1;
		color[nid] = -1;
	}
	//precolor
	int regnum = 0;
	BOOST_FOREACH(Temp *r, regs) {
		Node *regnode = igraph.temp2node(r);
		if (regnode) {
			int regnode_id = igraph.node2nid(regnode);
			//DBG("%s is precolored on %p %d", r->toString().c_str(), n, i);
			precolored.set(regnode_id);
			color[regnode_id] = regnum;
		}
		++regnum;
	}

	const InterferenceGraph::Moves &moves = igraph.getMoves();
	InterferenceGraph::Moves::const_iterator it = moves.begin();
	while (it != moves.end()) {
		Node *x = it->first;
		Node *y = it->second;
		int nidx = igraph.node2nid(x);
		int nidy = igraph.node2nid(y);
		NidPair mv = std::make_pair(nidx, nidy);
		workListMoves.push_back(mv);
		Moves &movesx = moveList[nidx];
		movesx.push_back(mv);
		Moves &movesy = moveList[nidy];
		movesy.push_back(mv);

		++it;
	}
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
	selectStack.push_back(igraph.nid2node(nid));

	Bitmap adj = adjacent(nid);
	for (unsigned int i = 0; i < adj.size(); ++i) {
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
		Bitmap adj = adjacent(nid);
		adj |= nid;
		enableMoves(adj);
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
	for (unsigned int nid = 0; nid < nodes.size(); ++nid) {
		if (!nodes.get(nid)) {
			continue;
		}
		enableMoves(nid);
	}
}

void
Color2::enableMoves(int nid)
{
	Moves moves = nodeMoves(nid);
	BOOST_FOREACH(NidPair mv, moves) {
		Moves::iterator it = std::find(activeMoves.begin(), activeMoves.end(), mv);
		if (it != activeMoves.end()) {
			activeMoves.erase(it);
			workListMoves.push_back(*it);
		}
	}
}

void 
Color2::addWorkList(int nid)
{
	if (!precolored.get(nid) && !isMoveRelated(nid) && degreeMap[nid] < K) {
		freezeWorkList.reset(nid);
		simplifyWorkList.set(nid);
	}
}

bool 
Color2::isOK(int nid1, int nid2)
{
	Node *n1 = igraph.nid2node(nid1);
	Bitmap adj = nodes2bitmap(n1->adj());
	return degreeMap[nid1] < K || 
		precolored.get(nid1) ||
		adj.get(nid2);
}

bool 
Color2::isConservative(const Bitmap &nodes)
{
	int k = 0;
	for (unsigned int nid = 0; nid < nodes.size(); ++nid) {
		if (!nodes.get(nid)) {
			continue;
		}
		if (degreeMap[nid] >= K) {
			++k;
		}
	}
	return (k < K);
}

void
Color2::coalesce() 
{ 
	assert(!workListMoves.empty());
	
	NidPair mv = workListMoves.back();
	workListMoves.pop_back();
	workListMoves.erase(workListMoves.end()-1);

	int x = getAlias(mv.first);
	int y = getAlias(mv.second);
	int u, v;
	if (precolored.get(y)) {
		u = y;
		v = x;
	} else {
		u = x;
		v = y;
	}
	if (u == v) {
		coalescedMoves.push_back(mv);
		addWorkList(u);
	} else if (precolored.get(v) || adj(u, v)) {
		constrainedMoves.push_back(mv);
		addWorkList(u);
		addWorkList(v);
	} else {
		Bitmap adju = adjacent(u);
		Bitmap adjv = adjacent(v);

		bool ok = true;
		for (unsigned int a = 0; a < adjv.size(); ++a) {
			if (!isOK(a, u)) {
				ok = false;
				break;
			}
		}
		if ((precolored.get(u) && ok) || 
			(!precolored.get(u) && isConservative(adju | adjv))) {
			coalescedMoves.push_back(mv);
			combine(u, v);
			addWorkList(u);
		} else {
			activeMoves.push_back(mv);
		}
	}
}

void 
Color2::combine(int nid1, int nid2)
{
	if (freezeWorkList.get(nid2)) {
		freezeWorkList.reset(nid2);
	} else {
		spillWorkList.reset(nid2);
	}
	coalescedNodes.set(nid2);
	aliasMap[nid2] = nid1;
	//combine move list
	Moves &moves1 = moveList[nid1];
	Moves &moves2 = moveList[nid2];
	std::copy(moves2.begin(), moves2.end(), std::back_inserter(moves1));

	enableMoves(nid2);
	Bitmap adj2 = adjacent(nid2);
	for (unsigned int a = 0; a < adj2.size(); ++a) {
		if (!adj2.get(a)) {
			continue;
		}

		igraph.addEdge(igraph.nid2node(a), igraph.nid2node(nid1));
		decrementDegree(a);
	}
	if (degreeMap[nid1] >= K && freezeWorkList.get(nid1)) {
		freezeWorkList.reset(nid1);
		spillWorkList.set(nid1);
	}
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
	int nid = freezeWorkList.right();
	freezeWorkList.reset(nid);
	simplifyWorkList.set(nid);
	freezeMoves(nid);
}


void
Color2::freezeMoves(int nid)
{
	Moves moves = nodeMoves(nid);
	Moves::iterator it = moves.begin();
	while (it != moves.end()) {
		int x = it->first;
		int y = it->second;
		int v;
		if (getAlias(y) == getAlias(nid)) {
			v = getAlias(x);
		} else {
			v = getAlias(y);
		}

		activeMoves.erase(it);
		frozenMoves.push_back(*it);
		if (nodeMoves(v).empty() && degreeMap[v] < K) {
			freezeWorkList.reset(v);
			simplifyWorkList.set(v);
		}
		++it;
	}

}

void
Color2::selectSpill()
{
	//FIXME:
	int nid = spillWorkList.right();
	spillWorkList.reset(nid);
	simplifyWorkList.set(nid);
	freezeMoves(nid);
}

void
Color2::assignColors() 
{ 
	while (!selectStack.empty()) {
		Node *n = selectStack.pop_back();
		Bitmap okColors(K);
		for (int i = 0; i < K; ++i) {
			okColors.set(i);
		}
		NodeList adj = n->adj();
		BOOST_FOREACH(Node *a, adj) {
			int reala = getAlias(igraph.node2nid(a));
			//干渉辺がすでに色付けされている?
			if (coloredNodes.get(reala) || precolored.get(reala)) {
				okColors.reset(color[reala]);
			}
		}
		int nid = igraph.node2nid(n);
		if (!okColors.none() && okColors.right() < 10) {
			coloredNodes.set(nid);
			color[nid] = okColors.right();
		} else {
			spilledNodes.set(nid);
		}
	}
	for (unsigned int nid = 0; nid < coalescedNodes.size(); ++nid) {
		if (!coalescedNodes.get(nid)) {
			continue;
		}
		color[nid] = color[getAlias(nid)];
	}
}

Bitmap
Color2::adjacent(int nid)
{
	Node *n =  igraph.nid2node(nid);
	Bitmap adj = nodes2bitmap(n->adj());
	Bitmap selects = nodes2bitmap(selectStack);
	Bitmap tmp = selects | coalescedNodes;
	adj -=  tmp;
	return adj;
}

Color2::Moves
Color2::nodeMoves(int nid)
{
	Moves &moves = moveList[nid];
	if (!moves.empty()) {
		//TODO:  moves & (activeMoves | workListMoves);
		Moves tmp;
		tmp = activeMoves;
		std::copy(workListMoves.begin(), workListMoves.end(), std::back_inserter(tmp));
		Moves tmp2;
		Moves::iterator it = moves.begin();
		while (it != moves.end()) {
			if (std::find(tmp.begin(), tmp.end(), *it) != tmp.end()) {
				tmp2.push_back(*it);
			}
			++it;
		}
		return tmp2;
	} else {
		return Moves();
	}
}

bool
Color2::isMoveRelated(int nid)
{
	Moves moves = nodeMoves(nid);
	return !moves.empty();
}

std::string 
Color2::tempMap(Temp *temp)
{
	//TODO: Do not depend on a specific target
	Node *n = igraph.temp2node(temp);
	int nid = igraph.node2nid(n);
	int regnum = color[nid];
	if (regnum == -1) {
		//ERROR("%s is not allocated to a register", temp->toString().c_str());
		return "";
	}
	return regs[regnum]->toString();
}


Bitmap 
Color2::nodes2bitmap(const graph::NodeList &nodes)
{
	Bitmap bm(tempSize);
	BOOST_FOREACH(Node *n, nodes) {
		bm.set(igraph.node2nid(n));
	}
	return bm;
}

bool
Color2::adj(int nid1, int nid2)
{
	Node *n1 = igraph.nid2node(nid1);
	Node *n2 = igraph.nid2node(nid2);
	return n1->adj(n2);
}

}//namespace regalloc
