INCS=-I../../cpp -I../../cpp/asm -I../../cpp/ast -I../../cpp/canon -I../../cpp/common -I../../cpp/frame -I../../cpp/graph -I../../cpp/parse -I../../cpp/regalloc -I../../cpp/symbol -I../../cpp/target/arm -I../../cpp/temp -I../../cpp/translate -I../../cpp/tree -I../../cpp/type

CFLAGS=$(INCS) -g -Wall -DDEBUG

LivenessTest:
	g++ $(CFLAGS) ../../cpp/common/Object.cpp ../../cpp/common/HeapManager.cpp ../../cpp/asm/Instruction.cpp ../../cpp/asm/OPER.cpp ../../cpp/asm/MOVE.cpp ../../cpp/asm/LABEL.cpp ../../cpp/common/tiger.cpp ../../cpp/symbol/Symbol.cpp ../../cpp/temp/Temp.cpp ../../cpp/temp/Label.cpp ../../cpp/graph/Graph.cpp ../../cpp/graph/Node.cpp ../../cpp/graph/AsmFlowGraph.cpp ../../cpp/graph/InterferenceGraph.cpp ../../cpp/regalloc/Liveness.cpp ../../cpp/asm/CodeGen.cpp ../../cpp/target/arm/ARMCodeGen.cpp ../../cpp/tree/Tree.cpp ../../cpp/tree/TreeMatcher.cpp LivenessTest.cpp -oLivenessTest

ARMCodeGenTest:
	g++ $(CFLAGS) ../../cpp/common/Object.cpp ../../cpp/common/HeapManager.cpp ../../cpp/tree/Tree.cpp  ../../cpp/tree/TreeMatcher.cpp ../../cpp/target/arm/ARMFrame.cpp ../../cpp/target/arm/ARMCodeGen.cpp ../../cpp/asm/CodeGen.cpp ../../cpp/asm/Instruction.cpp ../../cpp/asm/OPER.cpp ../../cpp/asm/MOVE.cpp ../../cpp/asm/LABEL.cpp ../../cpp/common/tiger.cpp ../../cpp/symbol/Symbol.cpp ../../cpp/temp/Temp.cpp ../../cpp/temp/Label.cpp ARMCodeGenTest.cpp -oARMCodeGenTest


TreeMatcherTest:
	g++ $(CFLAGS) ../../cpp/common/Object.cpp ../../cpp/common/HeapManager.cpp ../../cpp/tree/Tree.cpp  ../../cpp/tree/TreeMatcher.cpp TreeMatcherTest.cpp -oTreeMatcherTest

CanonTest:
	g++ $(CFLAGS) ../../cpp/common/Object.cpp ../../cpp/temp/Temp.cpp ../../cpp/tree/TreePrinter.cpp ../../cpp/common/HeapManager.cpp ../../cpp/tree/Tree.cpp ../../cpp/canon/Canon.cpp CanonTest.cpp -oCanonTest

RefTest:
	g++ $(CFLAGS) RefTest.cpp -oRefTest