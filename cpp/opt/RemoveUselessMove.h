#ifndef REMOVE_USELESS_MOVE_H
#define REMOVE_USELESS_MOVE_H

#include "Instruction.h"

namespace opt {
assem::InstructionList removeUselessMove(const assem::InstructionList &ilist);
}//namespace opt
#endif //REMOVE_USELESS_MOVE_H
