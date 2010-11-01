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
	TempList::const_iterator it = li.begin();
	while (it != li.end()) {
		Temp *t = *it;
		assert(t);
		result += t->toString();
		result += " ";
		++it;
	}
	DBG("%s", result.c_str());
}


Liveness::Liveness(const graph::FlowGraph &flow)
{
	const NodeList &flowNodes = flow.getNodes();
	NodeList::const_iterator it = flowNodes.begin();
	while (it != flowNodes.end()) {
		LiveInfo *li = new LiveInfo();
		li->def = flow.def(*it);
		li->use = flow.use(*it);
		li->node = *it;
		info.push_back(li);
		++it;
	}
	calcLives();

	int n = 0;
	it = flowNodes.begin();
	while (it != flowNodes.end()) {
		const AsmFlowGraph::InstNode *inst = (AsmFlowGraph::InstNode*)(*it);
		DBG("============================= %d", n);
		DBG("%s", inst->getInst()->toString().c_str());
		//DBG("def:");
		//printTempList(info[n]->def);
		//DBG("use:");
		//printTempList(info[n]->use);
		DBG("livein:");
		printTempList(info[n]->livein);
		DBG("liveout:");
		printTempList(info[n]->liveout);
		++it;
		++n;
	}

}


void
Liveness::calcLives()
{
	//TODO: test & improve performance
 retry:
	bool continuing = false;	
	for (size_t n = 0; n < info.size(); ++n) {
		TempList oldLivein = info[n]->livein;
		TempList oldLiveout = info[n]->liveout;
		
		//info[n]->livein = info[n]->use + (info[n]->liveout - info[n]->def);
		TempList tmp = tempListSub(info[n]->liveout, info[n]->def);
		std::copy(info[n]->use.begin(), info[n]->use.end(), std::back_inserter(tmp));
		std::sort(tmp.begin(), tmp.end());
		tmp.erase(std::unique(tmp.begin(), tmp.end()), tmp.end());
		info[n]->livein = tmp;
		
		info[n]->liveout = getAllLiveinsAtSuccessors(info[n]->node);
		//TempList allLivein = getAllLiveinsAtSuccessors(info[n]->node);
		//std::copy(allLivein.begin(), allLivein.end(), std::back_inserter(info[n]->liveout));
		
		continuing |= isContinuing(n, oldLivein, oldLiveout);
	}
	if (continuing) {
		DBG("continue liveness calc");
		goto retry;
	}
}

TempList
Liveness::tempListSub(const TempList &liveout, const TempList &def)
{
	TempList result;
	result = liveout;
	TempList::const_iterator it = def.begin();
	while (it != def.end() && !result.empty()) {
		const Temp *d = (*it);
		result.erase(std::remove(result.begin(), result.end(), d), result.end());
		++it;
	}
	return result;
}

TempList
Liveness::getAllLiveinsAtSuccessors(const Node *node)
{
	TempList liveins;
	const NodeList &successors = node->succ();
	NodeList::iterator suc = successors.begin();
	while (suc != successors.end()) {
		const TempList &in = getLivein(*suc);
		std::copy(in.begin(), in.end(), std::back_inserter(liveins));
		std::sort(liveins.begin(), liveins.end());
		liveins.erase(std::unique(liveins.begin(), liveins.end()), liveins.end());

		++suc;
	}
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

}//namespace regalloc
