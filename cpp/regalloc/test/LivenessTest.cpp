#include <stdio.h>

#include "Liveness.h"
#include "Instruction.h"

namespace regalloc {
class LivenessTest
{
public:
	LivenessTest() {
	}

	void test1() {
		assem::InstructionList proc;
		proc.push_back(gcnew(OPER, ("")));
	}
};

}//namespace

int main(int argc, char **argv)
{
	regalloc::LivenessTest test;
	if (test.ok()) {
		printf("\nOK\n\n");
	}
	return 0;
}
