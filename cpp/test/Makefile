TARGET_CPU=arm
SUB_DIRS=asm ast canon common frame graph parse regalloc symbol temp translate tree type target/$(TARGET_CPU)
INCS=-I/usr/local/include -I./ $(addprefix -I../,$(SUB_DIRS))
VPATH=./ $(SUB_DIRS)
FLAGS=-g -Wall $(INCS) -L../

LivenessTest: libcore.a
	g++ $(FLAGS) LivenessTest.cpp -lcore -o $@

ARMCodeGenTest:
	g++ $(FLAGS) ARMCodeGenTest.cpp -lcore -o $@


TreeMatcherTest:
	g++ $(FLAGS) TreeMatcherTest.cpp -lcore -o $@

CanonTest:
	g++ $(FLAGS) CanonTest.cpp -lcore -o $@

RefTest:
	g++ $(CFLAGS) RefTest.cpp -lcore -o $@

libcore.a:
	make -C ../ -f Makefile libcore.a

clean:
	-@rm -f LivenessTest ARMCodeGenTest TreeMatcherTest CanonTest RefTest
