#ifndef LIVENESS_H
#define LIVENESS_H

#include "Temp.h"
#include <vector>
#include "Bitmap.h"

namespace graph {
class Node;
class FlowGraph;
class InterferenceGraph;
}

namespace regalloc {

class Liveness
{
public:
	Liveness(const graph::FlowGraph &flow);
	~Liveness();

	graph::InterferenceGraph *getInterferenceGraph() const {
		return igraph;
	}

private:
	void calcLives();
	Bitmap getAllLiveinsAtSuccessors(int n);
	Bitmap *getLivein(const graph::Node *node);

	Bitmap tempList2bitmap(const TempList &tlist);
	TempList bitmap2tempList(const Bitmap &bm);

	void makeInterferenceGraph();
	void printBitmap(const Bitmap &bm);
	struct LiveInfo
	{
		Bitmap *def;
		Bitmap *use;
		Bitmap *livein;
		Bitmap *liveout;
		graph::Node *node;
	};
	typedef std::vector<LiveInfo*> LiveInfoVec;
	LiveInfoVec info;
	TempList temps;
	graph::InterferenceGraph *igraph;
};
}//namespace regalloc

#endif
