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
			char type = *(++it);
			int num = *(++it) - 0x30;
			switch (type) {
			case 's':
				result += m->tempMap(uses[num]);
				break;
			case 'd':
				result += m->tempMap(defs[num]);
				break;
			case 'j':
				//TODO:
				result += jmps[num]->toString().c_str();
				break;
			default:
				assert(!"invalid assem format");
			}
		} else {
			result += c;
		}
		++it;
	}
	DBG("                          old assem = %s", assem.c_str());
	//DBG("new assem = %s", result.c_str());
	assem = result;
	return result;
}

}//namespace assem
