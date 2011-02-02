#include "Instruction.h"
#include <boost/foreach.hpp>
#include "debug.h"

namespace opt {
assem::InstructionList removeUselessJump(const assem::InstructionList &ilist)
{
	assem::InstructionList newlist;
	BOOST_FOREACH(assem::Instruction *inst, ilist) {
		//NIY:
		newlist.push_back(inst);
	}
	return newlist;
}

}//namespace opt

