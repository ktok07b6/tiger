#include <boost/foreach.hpp>
#include "Liveness.h"
#include "Node.h"
#include "FlowGraph.h"
#include "AsmFlowGraph.h"
#include "InterferenceGraph.h"

#include <algorithm>

using namespace graph;

namespace regalloc {

void 
Liveness::printBitmap(const Bitmap &bm, const char *prefix)
{
	std::string result;
	if (prefix) {
		result += prefix;
	}
	for (unsigned int i = 0; i < bm.size(); ++i) {
		if (bm.get(i)) {
			result += temps[i]->toString();
			result += " ";
		}
	}
	DBG("%s", result.c_str());
}


Liveness::Liveness(const graph::FlowGraph &flow)
{
	//flow.show();
	const NodeList &flowNodes = flow.getNodes();
	BOOST_FOREACH(Node *n, flowNodes) {
		TempList def = flow.def(n);
		std::copy(def.begin(), def.end(), std::back_inserter(temps));
		TempList use = flow.use(n);
		std::copy(use.begin(), use.end(), std::back_inserter(temps));
	}
	std::sort(temps.begin(), temps.end(), LessTemp());
	temps.erase(std::unique(temps.begin(), temps.end()), temps.end());
#if 1	
	int ti = 0;
	BOOST_FOREACH(Temp *t, temps) {
		//DBG("%d:%s", ti, (const char*)*t);
		t->index = ti;
		++ti;
	}
#endif	
	BOOST_FOREACH(Node *n, flowNodes) {
		LiveInfo *li = new LiveInfo();
		li->def = new Bitmap(temps.size());
		li->use = new Bitmap(temps.size());
		li->livein = new Bitmap(temps.size());
		li->liveout = new Bitmap(temps.size());

		*li->def = tempList2bitmap(flow.def(n));
		*li->use = tempList2bitmap(flow.use(n));
		//TODO: 
		//n->setLivein(li->livein);

		//we make reference mutually to flowNode and liveInfo
		n->setTag((int)li);
		li->node = n;
		info.push_back(li);
	}

	calcLives();

#if 0
	int ii = 0;
	BOOST_FOREACH(Node *n, flowNodes) {
		const AsmFlowGraph::InstNode *inst = (AsmFlowGraph::InstNode*)n;
		DBG("============================= %d", ii);
		DBG("instruction(%s)", inst->getInst()->toString().c_str());
		printBitmap(*info[ii]->def,     "def    :");
		printBitmap(*info[ii]->use,     "use    :");
		printBitmap(*info[ii]->livein,  "livein :");
		printBitmap(*info[ii]->liveout, "liveout:");
		++ii;
	}
#endif

	makeInterferenceGraph();
	//add interference edges
	int i = 0;
	BOOST_FOREACH(Node *n, flowNodes) {
		const AsmFlowGraph::InstNode *inst = (AsmFlowGraph::InstNode*)n;
		if (flow.isMove(inst)) {
			assem::MOVE *mv = (assem::MOVE*)inst->getInst();
			Temp *use = mv->getSrc();
			Temp *def = mv->getDst();
			Node *ndef = igraph->temp2node(def);
			//add edge
			TempList liveoutList = bitmap2tempList(*info[i]->liveout);
			BOOST_FOREACH(Temp *liveout, liveoutList) {
				if (liveout != use && liveout != def && ndef) {
					Node *nliveout = igraph->temp2node(liveout);
					igraph->addEdge(nliveout, ndef);
				}
			}
			if (use) {
				//add move relative
				Node *nuse = igraph->temp2node(use);
				if ((nuse && ndef) && (use != def)) {
					igraph->addMoveEdge(nuse, ndef);
				}
			}
		} else {
			TempList def = inst->getInst()->def();
			TempList liveoutList = bitmap2tempList(*info[i]->liveout);
			BOOST_FOREACH(Temp *liveout, liveoutList) {
				Node *nliveout = igraph->temp2node(liveout);
				BOOST_FOREACH(Temp *d, def) {
					Node *ndef = igraph->temp2node(d);
					if (liveout != d && ndef) {
						igraph->addEdge(nliveout, ndef);
					}
				}
			}
		}
		++i;
	}
}

Liveness::~Liveness()
{
	delete igraph;

	BOOST_FOREACH(LiveInfo *li, info) {
		delete li->def; 
		delete li->use; 
		delete li->livein; 
		delete li->liveout;
		delete li;
	}

}

void
Liveness::calcLives()
{
	//TODO: test & improve performance
 retry:
	bool continuing = false;
	for (int n = info.size()-1; 0 <= n; --n) {
		Bitmap oldLivein = *info[n]->livein;
		Bitmap oldLiveout = *info[n]->liveout;
		
		*info[n]->liveout = getAllLiveinsAtSuccessors(n);

		*info[n]->livein = *info[n]->use | (*info[n]->liveout - *info[n]->def);
		
		if ((*info[n]->livein != oldLivein) || 
			(*info[n]->liveout != oldLiveout) ) {
			continuing = true;
		}
	}
	
	if (continuing) {
		VDBG("continue liveness calculation");
		goto retry;
	}
}

Bitmap
Liveness::getAllLiveinsAtSuccessors(int n)
{
	Bitmap liveins(temps.size());
	Node *node = info[n]->node;
	const NodeList &successors = node->succ();
		
	BOOST_FOREACH(Node *succ, successors) {
		//Bitmap *livein = getLivein(succ);
		Bitmap *livein = ((LiveInfo*)succ->getTag())->livein;
		if (livein) {
			liveins |= *livein;
		}
	}
	return liveins;
}

Bitmap *
Liveness::getLivein(const Node *node)
{
	//TODO: imporove performance
	LiveInfoVec::const_iterator it = info.begin();
	LiveInfoVec::const_iterator end = info.end();
	while (it != end) {
		LiveInfo *i = (*it);
		if (i->node == node) {
			return i->livein;
		}
		++it;
	}
	return NULL;
}


Bitmap 
Liveness::tempList2bitmap(const TempList &tlist)
{
	Bitmap bm(temps.size());
	BOOST_FOREACH(const Temp *t, tlist) {
		bm.set(t->index);
	}
	return bm;
}

TempList 
Liveness::bitmap2tempList(const Bitmap &bm)
{
	TempList tlist;
	std::vector<int> indexes;
	bm.getIndexes(indexes);
	std::vector<int>::iterator it = indexes.begin();
	while (it != indexes.end()) {
		Temp *t = temps[*it];
		tlist.push_back(t);
		++it;
	}
	/*
	for (unsigned int i = 0; i < sz; ++i) {
		Temp *t = temps[indexes[i]];
		tlist.push_back(t);
	}
	*/
	/*
	for (unsigned int i = 0; i < bm.size(); ++i) {
		if (bm.get(i)) {
			Temp *t = temps[i];
			tlist.push_back(t);
		}
	}
	*/
	return tlist;
}


void
Liveness::makeInterferenceGraph()
{
	VDBG("%s", __FUNCTION__);
	igraph = new InterferenceGraph();

	//enumerate all live temps
#if 0 //BUG?
	Bitmap liveouts;
	BOOST_FOREACH(LiveInfo *li, info) {
		liveouts |= *(li->liveout); 
	}
	TempList liveTemps = bitmap2tempList(liveouts);
#else
	TempList liveTemps;
	BOOST_FOREACH(LiveInfo *li, info) {
		TempList tlist = bitmap2tempList(*(li->liveout));
		std::copy(tlist.begin(), tlist.end(), std::back_inserter(liveTemps));
	}
	std::sort(liveTemps.begin(), liveTemps.end(), LessTemp());
	liveTemps.erase(std::unique(liveTemps.begin(), liveTemps.end()), liveTemps.end());
#endif

	//create nodes for temps,
	BOOST_FOREACH(Temp *t, liveTemps) {
		igraph->newNode(t);
	}

	//we must assign node id to each node
	int nid = 0;
	const NodeList &nodes = igraph->getNodes(); 
	BOOST_FOREACH(Node *n, nodes) {
		n->setTag(nid);
		++nid;
	}
}

}//namespace regalloc
