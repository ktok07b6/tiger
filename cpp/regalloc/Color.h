#ifndef COLOR_H
#define COLOR_H

#include "TempMap.h"
#include "Node.h"
#include <map>
#include "InterferenceGraph.h"
namespace graph {
	//class InterferenceGraph;
}

namespace regalloc {

class Color : public TempMap
{
public:
	Color(const graph::InterferenceGraph &igraph, const TempList &regs);
	virtual std::string tempMap(Temp *temp);

private:
	void build();
	void makeWorkList();
	void simplify();
	bool coalesce();
	void freeze();
	void selectSpill();
	void assignColors();

	void decrementDegree(graph::Node *);
	void enableMoves(const graph::NodeList &);
	void addWorkList(graph::Node *);

	bool ok(graph::Node *t, graph::Node *r);
	bool conservative(const graph::NodeList &);

	void combine(graph::Node *, graph::Node *);
	graph::Node *getAlias(graph::Node *);

	graph::NodeList adjacent(graph::Node *);

	bool setColor(graph::Node *);
	bool isPrecolored(graph::Node *);
	int getColoredIndex(graph::Node *);
	graph::NodeList nodeMoves(graph::Node *);
	bool isMoveRelated(graph::Node *);

	enum {
		//TODO: Do not depend on a specific target
		K = 16
	};


	graph::NodeList simplifyWorkList;
	graph::NodeList freezeWorkList;
	graph::NodeList spillWorkList;
	graph::NodeList spilledNodes;
	graph::NodeList coalescedNodes;
	//graph::NodeList coloredNodes;
	graph::NodeList selectStack;

	graph::NodeList coalescedMoves;
	graph::NodeList constrainedMoves;
	graph::NodeList frozenMoves;
	graph::NodeList workListMoves;
	graph::NodeList activeMoves;

	typedef std::map<graph::Node *, int> DegreeMap;
	DegreeMap degreeMap;
	typedef std::map<graph::Node *, graph::Node *> AliasMap;
	AliasMap aliasMap;
	typedef std::map<graph::Node *, graph::NodeList> MoveListMap;
	MoveListMap moveList;

	graph::InterferenceGraph igraph;
	graph::NodeList coloredNodes[K];
	TempList regs;
};

} //namespace regalloc
#endif
