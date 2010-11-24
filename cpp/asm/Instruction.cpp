#include "Instruction.h"
#include "TempMap.h"

using namespace std;

namespace assem {

Instruction::Instruction(const std::string &opcode, 
						 const std::string &operands,
						 const std::string &comment,
						 int sourceLine)
	: opcode(opcode)
	, operands(operands)
	, comment(comment)
	, sourceLine(sourceLine)
{
}


string 
Instruction::format(TempMap *m)
{
	string result;
	if (opcode.empty()) {
		return "\t" + comment;
	}
	if (!isLABEL()) {
		result = "\t" + opcode + "\t";
	} else {
		result = opcode;
	}
	string::iterator it = operands.begin();
	TempList uses = use();
	TempList defs = def();
	LabelList jmps = jumps();
	while (it != operands.end()) {
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
				return "\t" + comment;
			}
			result += s;
		} else {
			result += c;
		}
		++it;
	}
	//DBG("                          old assem = %s", assem.c_str());
	//DBG("new assem = %s", result.c_str());
	if (!comment.empty()) {
		result += "\t" + comment;
	}
	return result;
}

}//namespace assem
