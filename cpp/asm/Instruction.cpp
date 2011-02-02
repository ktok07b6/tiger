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
			std::string s;
			Temp *t = NULL;
			char type = *(++it);
			int num = *(++it) - 0x30;
			switch (type) {
			case 's':
				t = m->tempMap(uses[num]);
				if (t) s = t->toString();
				break;
			case 'd':
				t = m->tempMap(defs[num]);
				if (t) s = t->toString();
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
				return "\t@" + opcode + "\t" + operands +comment;
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

void
Instruction::update(TempMap *m)
{
	if (opcode.empty()) {
		return;
	}
	string  newOperands;
	string::iterator it = operands.begin();
	TempList uses = use();
	TempList defs = def();
	LabelList jmps = jumps();
	bool valid = true;
	while (it != operands.end()) {
		char c = *it;
		if (c == '$') {
			//assert(distance(it, assem.end() > 2));
			std::string s;
			Temp *t = NULL;
			char type = *(++it);
			int num = *(++it) - 0x30;
			switch (type) {
			case 's':
				t = m->tempMap(uses[num]);
				if (t) {
					s = t->toString();
					replaceUse(num, t);
				} else {
					valid = false;
				}
				break;
			case 'd':
			    t = m->tempMap(defs[num]);
				if (t) {
					s = t->toString();
					replaceDef(num, t);
				} else {
					valid = false;
				}
				break;
			case 'j':
				s = jmps[num]->toString();
				break;
			default:
				assert(!"invalid assem format");
			}
			newOperands += s;
		} else {
			newOperands += c;
		}
		++it;
	}
	if (valid) {
		operands = newOperands;
	} else {
		opcode = "@" + opcode;
	}
}

}//namespace assem
