#ifndef COLOR_H
#define COLOR_H

#include "TempMap.h"
#include "Node.h"
#include <map>
#include <vector>
#include "InterferenceGraph.h"
#include "Bitmap.h"

namespace graph {
	//class InterferenceGraph;
}

namespace regalloc {

class Color : public TempMap
{
public:
	Color(const graph::InterferenceGraph &igraph, const TempList &regs);
	virtual std::string tempMap(Temp *temp);
	bool coloring();
	TempList getTempForSpill();
	void setNumWorkRegs(int n);
private:

	typedef std::pair<int, int> NidPair;
	typedef std::vector<NidPair> Moves;

	void build();
	void makeWorkList();
	void simplify();
	void coalesce();
	void freeze();
	void selectSpill();
	void assignColors();

	void decrementDegree(int nid);
	void enableMoves(const Bitmap &);
	void enableMoves(int nid);
	void addWorkList(int nid);

	bool isOK(int nid1, int nid2);
	bool isConservative(const Bitmap &);

	void combine(int nid1, int nid2);
	int getAlias(int nid);

	Bitmap adjacent(int nid);

	Moves nodeMoves(int nid);
	bool isMoveRelated(int nid);

	void freezeMoves(int nid);
	bool adj(int n1, int n2);

	void show();

	Bitmap nodes2bitmap(const graph::NodeList &nodes);

	const size_t tempSize;
	graph::InterferenceGraph igraph;
	TempList regs;

	Bitmap simplifyWorkList;
	Bitmap freezeWorkList;
	Bitmap spillWorkList;
	Bitmap spilledNodes;
	Bitmap coalescedNodes;
	Bitmap coloredNodes;
	graph::NodeList selectStack;
	
	Moves coalescedMoves;
	Moves constrainedMoves;
	Moves frozenMoves;
	Moves workListMoves;
	Moves activeMoves;

	Bitmap precolored;
	typedef std::map<int, int> DegreeMap;
	DegreeMap degreeMap;
	typedef std::map<int, int> AliasMap;
	AliasMap aliasMap;
	typedef std::map<int, Moves> MoveListMap;
	MoveListMap moveList;
	typedef std::map<int, int> ColorMap;
	ColorMap color;
	TempList spillTemps;
	int numMaxRegs;
	int numWorkRegs;
};

} //namespace regalloc
#endif
