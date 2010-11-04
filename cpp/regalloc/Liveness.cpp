#include <boost/foreach.hpp>
#include "Liveness.h"
#include "Node.h"
#include "FlowGraph.h"
#include "AsmFlowGraph.h"
#include "InterferenceGraph.h"

#include <algorithm>

using namespace graph;

namespace regalloc {

void printTempList(const TempList &li)
{
	std::string result;

	BOOST_FOREACH(Temp *t, li) {
		assert(t);
		result += t->toString();
		result += " ";
	}
	DBG("%s", result.c_str());
}


Liveness::Liveness(const graph::FlowGraph &flow)
{
	const NodeList &flowNodes = flow.getNodes();
	BOOST_FOREACH(Node *n, flowNodes) {
		LiveInfo *li = new LiveInfo();//TODO: delete
		li->def = flow.def(n);
		li->use = flow.use(n);
		li->node = n;
		info.push_back(li);
	}
	calcLives();

#if 0
	int i = 0;
	BOOST_FOREACH(Node *n, flowNodes) {
		const AsmFlowGraph::InstNode *inst = (AsmFlowGraph::InstNode*)n;
		DBG("============================= %d", i);
		DBG("%s", inst->getInst()->toString().c_str());
		DBG("def:");
		printTempList(info[i]->def);
		DBG("use:");
		printTempList(info[i]->use);
		DBG("livein:");
		printTempList(info[i]->livein);
		DBG("liveout:");
		printTempList(info[i]->liveout);
		++i;
	}
#endif
	makeInterferenceGraph();
	//moved temps
	BOOST_FOREACH(Node *n, flowNodes) {
		const AsmFlowGraph::InstNode *inst = (AsmFlowGraph::InstNode*)n;
		if (flow.isMove(inst)) {
			assem::MOVE *mv = (assem::MOVE*)inst->getInst();
			Temp *src = mv->getSrc();
			Temp *dst = mv->getDst();
			if (src && dst) {
				Node *nsrc = igraph->temp2node(src);
				Node *ndst = igraph->temp2node(dst);
				InterferenceGraph::NodePair nodes(nsrc, ndst);
				igraph->addMove(nodes);
			}
		}
	}
}

Liveness::~Liveness()
{
	delete igraph;
}

void
Liveness::calcLives()
{
	//TODO: test & improve performance
 retry:
	bool continuing = false;
	for (int n = info.size()-1; 0 <= n; --n) {
		TempList oldLivein = info[n]->livein;
		TempList oldLiveout = info[n]->liveout;
		
		info[n]->liveout = getAllLiveinsAtSuccessors(n);

		//info[n]->livein = info[n]->use + (info[n]->liveout - info[n]->def);
		TempList tmp = tempListSub(info[n]->liveout, info[n]->def);
		std::copy(info[n]->use.begin(), info[n]->use.end(), std::back_inserter(tmp));
		std::sort(tmp.begin(), tmp.end());
		tmp.erase(std::unique(tmp.begin(), tmp.end()), tmp.end());
		info[n]->livein = tmp;
		
		continuing |= isContinuing(n, oldLivein, oldLiveout);
	}
	if (continuing) {
		DBG("continue liveness calculation");
		goto retry;
	}
}

TempList
Liveness::tempListSub(const TempList &liveout, const TempList &def)
{
	TempList result;
	result = liveout;
	BOOST_FOREACH(const Temp *t, def) {
		result.erase(std::remove(result.begin(), result.end(), t), result.end());
		if(result.empty()) {
			break;
		}
	}
	return result;
}

TempList
Liveness::getAllLiveinsAtSuccessors(int n)
{
	TempList liveins;
	Node *node = info[n]->node;
	const NodeList &successors = node->succ();
	if (successors.empty()) {
		return info[n]->def;
	} else {
		
		BOOST_FOREACH(Node *succ, successors) {
			const TempList &livein = getLivein(succ);
			std::copy(livein.begin(), livein.end(), std::back_inserter(liveins));
		}
	}
	std::sort(liveins.begin(), liveins.end());
	liveins.erase(std::unique(liveins.begin(), liveins.end()), liveins.end());

	return liveins;
}

const TempList &
Liveness::getLivein(const Node *node)
{
	//TODO: imporove performance
	LiveInfoVec::iterator it = info.begin();
	while (it != info.end()) {
		LiveInfo *i = (*it);
		if (i->node == node) {
			return i->livein;
		}
		++it;
	}
	assert(0);
	return TempList();
}

bool 
Liveness::isContinuing(int n, const TempList &oldLivein, const TempList &oldLiveout)
{
	const TempList &livein = info[n]->livein;
	const TempList &liveout = info[n]->liveout;
	//DBG("%s %d ========================", __FUNCTION__, n);
	//DBG("livein");printTempList(livein);
	//DBG("oldlivein");printTempList(oldLivein);
	//DBG("liveout");printTempList(liveout);
	//DBG("oldliveout");printTempList(oldLiveout);

	if (livein.size() == oldLivein.size() &&
		liveout.size() == oldLiveout.size()) {
		return std::equal(livein.begin(), livein.end(), oldLivein.begin()) == false ||
			std::equal(liveout.begin(), liveout.end(), oldLiveout.begin()) == false;
	}
	return true;
}

void
Liveness::makeInterferenceGraph()
{
	std::vector<TempList*> liveouts;
	LiveInfoVec::iterator it = info.begin();
	while (it != info.end()) {
		LiveInfo *linfo = *it;
		liveouts.push_back(&(linfo->liveout));
		++it;
	}

	igraph = new InterferenceGraph(liveouts);
}

}//namespace regalloc
