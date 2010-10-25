INCS=-I../../cpp -I../../cpp/asm -I../../cpp/ast -I../../cpp/canon -I../../cpp/common -I../../cpp/frame -I../../cpp/parse -I../../cpp/symbol -I../../cpp/target/arm -I../../cpp/temp -I../../cpp/translate -I../../cpp/tree -I../../cpp/type

CFLAGS=$(INCS) -g -Wall


TreeMatcherTest:
	g++ $(CFLAGS) ../../cpp/common/Object.cpp ../../cpp/common/HeapManager.cpp ../../cpp/tree/Tree.cpp  ../../cpp/tree/TreeMatcher.cpp TreeMatcherTest.cpp -oTreeMatcherTest

CanonTest:
	g++ $(CFLAGS) ../../cpp/common/Object.cpp ../../cpp/temp/Temp.cpp ../../cpp/tree/TreePrinter.cpp ../../cpp/common/HeapManager.cpp ../../cpp/tree/Tree.cpp ../../cpp/canon/Canon.cpp CanonTest.cpp -oCanonTest

RefTest:
	g++ $(CFLAGS) RefTest.cpp -oRefTest