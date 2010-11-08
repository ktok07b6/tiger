#ifndef COLOR_H
#define COLOR_H

#include "TempMap.h"
#include "Node.h"
#include <stack>
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

	void pushToSpillWorkList(graph::Node *);
	void pushToFreezeWorkList(graph::Node *);
	void pushToSimplifyWorkList(graph::Node *);

	NodeList adjacent(graph::Node *);

	bool setColor(graph::Node *);
	bool isPrecolored(graph::Node *);
	int getColoredIndex(graph::Node *);
	bool isEnableColoring() const;
	bool isMoveRelated(graph::Node *);

	enum {
		//TODO: Do not depend on a specific target
		K = 16
	};


	NodeList simplifyWorkList;
	NodeList freezeWorkList;
	NodeList spillWorkList;
	NodeList spilledNodes;
	NodeList coalescedNodes;
	NodeList coloredNodes;
	NodeList selectStack;

	NodeList coalescedMoves;
	NodeList constrainedMoves;
	NodeList frozenMoves;
	NodeList worklistMoves;
	NodeList activeMoves;

	graph::InterferenceGraph igraph;
	graph::NodeList coloredNodes[K];
	TempList regs;
};

} //namespace regalloc
#endif
