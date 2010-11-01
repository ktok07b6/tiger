#ifndef LIVENESS_H
#define LIVENESS_H

#include "Temp.h"
#include <vector>

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
	TempList tempListSub(const TempList &liveout, const TempList &def);
	TempList getAllLiveinsAtSuccessors(const graph::Node *node);
	const TempList &getLivein(const graph::Node *node);
	bool isContinuing(int n, const TempList &oldLivein, const TempList &oldLiveout);
	void makeInterferenceGraph();

	struct LiveInfo
	{
		TempList def;
		TempList use;
		TempList livein;
		TempList liveout;
		graph::Node *node;
	};
	typedef std::vector<LiveInfo*> LiveInfoVec;
	LiveInfoVec info;

	graph::InterferenceGraph *igraph;
};
}//namespace regalloc

#endif
