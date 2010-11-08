#include "ARMFrame.h"
#include <vector>
#include <string>
#include "Tree.h"
#include "Instruction.h"

using namespace std;
using namespace tree;

int main(int argc, char **argv)
{
	Symbol *frameName = Symbol::symbol("test");
	ARMFrame *frame = new ARMFrame(frameName, vector<int>());

	std::string s;
	tree::Stm *stm;
	assem::InstructionList ilist;
	assem::InstructionList *il;
	/*

	  t16 = 0;
L1:	  
      t17 = t16 + 1;
	  t18 = t18 + t17;
	  t16 = t17 * 2;
	  if (a < 10) goto l1
	  return t18
	 */
	Temp *t16 = new Temp();
	Temp *t17 = new Temp();
	Temp *t18 = new Temp();
	Label *l1 = new Label();
	Label *l2 = new Label();
	assem::Instruction *inst;

	stm = _MOVE(_TEMP(t16), _CONST(0));
	il = frame->codegen(stm);
	std::copy(il->begin(), il->end(), std::back_inserter(ilist));

	inst = il->at(0);
	s = inst->format(frame);
	DBG("%s", s.c_str());
	//assert("mov $d0, #0" == s);

	stm = _LABEL(l1);
	il = frame->codegen(stm);
	std::copy(il->begin(), il->end(), std::back_inserter(ilist));

	inst = il->at(0);
	s = inst->format(frame);
	DBG("%s", s.c_str());

	stm = _MOVE(_TEMP(t17), _BINOP(BINOP::oPLUS, _TEMP(t16), _CONST(1)));
	il = frame->codegen(stm);
	std::copy(il->begin(), il->end(), std::back_inserter(ilist));

	inst = il->at(0);
	s = inst->format(frame);
	DBG("%s", s.c_str());
	inst = il->at(1);
	s = inst->format(frame);
	DBG("%s", s.c_str());


	return 0;
}
