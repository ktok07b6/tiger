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

	void decrementDegree(int nid *);
	void enableMoves(const Bitmap &);
	void addWorkList(int nid);

	bool ok(int nid1, int nid2);
	bool conservative(const Bitmap &);

	void combine(int nid1, int nid2);
	int getAlias(int nid);

	Bitmap adjacent(int nid);

	bool setColor(int nid);
	bool isPrecolored(int nid);
	int getColoredIndex(int nid);
	Bitmap nodeMoves(int nid);
	bool isMoveRelated(int nid);

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

	Bitmap precolored;
	typedef std::map<int, int> DegreeMap;
	DegreeMap degreeMap;
	typedef std::map<int, int> AliasMap;
	AliasMap aliasMap;
	typedef std::map<int, Bitmap> MoveListMap;
	MoveListMap moveList;

	graph::InterferenceGraph igraph;
	Bitmap coloredNodes[K];
	TempList regs;
};

} //namespace regalloc
#endif
