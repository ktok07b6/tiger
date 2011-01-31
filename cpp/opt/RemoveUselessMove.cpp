#include "RemoveUselessMove.h"
#include <boost/foreach.hpp>

namespace opt {
assem::InstructionList removeUselessMove(const assem::InstructionList &ilist)
{
	assem::InstructionList newlist;
	BOOST_FOREACH(assem::Instruction *inst, ilist) {
		if (inst->isMOVE()) {
			assem::MOVE *mv = (assem::MOVE*)inst;
			if (mv->getSrc() == mv->getDst()) {
				continue;
			}
		}
		newlist.push_back(inst);
	}
	return newlist;
}

}//namespace opt

