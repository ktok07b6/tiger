#ifndef COLOR2_H
#define COLOR2_H

#include "TempMap.h"
#include "Node.h"
#include <map>
#include "InterferenceGraph.h"
#include "Bitmap.h"

namespace graph {
	//class InterferenceGraph;
}

namespace regalloc {

class Color2 : public TempMap
{
public:
	Color2(const graph::InterferenceGraph &igraph, const TempList &regs);
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

	Bitmap simplifyWorkList;
	Bitmap freezeWorkList;
	Bitmap spillWorkList;
	Bitmap spilledNodes;
	Bitmap coalescedNodes;
	//Bitmap coloredNodes;
	Bitmap selectStack;

	Bitmap coalescedMoves;
	Bitmap constrainedMoves;
	Bitmap frozenMoves;
	Bitmap workListMoves;
	Bitmap activeMoves;

	typedef std::map<graph::Node *, int> DegreeMap;
	DegreeMap degreeMap;
	typedef std::map<graph::Node *, graph::Node *> AliasMap;
	AliasMap aliasMap;
	typedef std::map<graph::Node *, Bitmap> MoveListMap;
	MoveListMap moveList;

	graph::InterferenceGraph igraph;
	Bitmap coloredNodes[K];
	TempList regs;
};

} //namespace regalloc
#endif
