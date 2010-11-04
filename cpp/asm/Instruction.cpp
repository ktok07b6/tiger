#include "Instruction.h"
#include "TempMap.h"

using namespace std;

namespace assem {

Instruction::Instruction(const string &assem)
	: assem(assem)
{
}

string 
Instruction::format(TempMap *m)
{
	string result;
	string::iterator it = assem.begin();
	TempList uses = use();
	TempList defs = def();
	LabelList jmps = jumps();
	while (it != assem.end()) {
		char c = *it;
		if (c == '$') {
			//assert(distance(it, assem.end() > 2));
			string s;
			char type = *(++it);
			int num = *(++it) - 0x30;
			switch (type) {
			case 's':
				s = m->tempMap(uses[num]);
				break;
			case 'd':
				s = m->tempMap(defs[num]);
				break;
			case 'j':
				s = jmps[num]->toString();
				break;
			default:
				assert(!"invalid assem format");
			}
			if (s.empty()) {
				//If a temp of instruction is not used,
				//this instruction is ignored.
				return "";
			}
			result += s;
		} else {
			result += c;
		}
		++it;
	}
	//DBG("                          old assem = %s", assem.c_str());
	//DBG("new assem = %s", result.c_str());
	return result;
}

}//namespace assem
