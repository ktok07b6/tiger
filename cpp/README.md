tiger
=====

This program is a compiler for the Tiger programming language. This language is the source language of Andrew Appel's "Modern Compiler Implementation". 
https://www.cs.princeton.edu/~appel/modern/ml/

#Supporting target architectures
* arm
* x86

#Example

###source
```
let
    function hello():string = "hello world\n"
in
    print(hello())
end
```

###asm

```
.text
    .align  2
    .global __tigermain
hello:
    pushl   %ebp
    movl    %esp, %ebp
    subl    $12, %esp
    movl    %ebx, -4(%ebp)
    movl    $17, 0(%esp)
    call    alloc
    movl    %eax, %ebx
    movl    $13, %eax
    movl    %eax, (%ebx)
    movl    %ebx, %eax
    movl    $4, %ecx
    addl    %ecx, %eax
    movl    L0, %ecx
    movl    %ecx, 4(%esp)
    movl    %eax, 0(%esp)
    call    strcpy
    movl    %ebx, %eax
    jmp end_of_hello
end_of_hello:
    addl    $12, %esp
    movl    -4(%ebp), %ebx
    movl    %ebp, %esp
    popl    %ebp
    ret

__tigermain:
    pushl   %ebp
    movl    %esp, %ebp
    subl    $4, %esp
    movl    %ebp, 0(%esp)
    call    hello
    movl    %eax, 0(%esp)
    call    print
    jmp end_of___tigermain
end_of___tigermain:
    addl    $4, %esp
    movl    %ebp, %esp
    popl    %ebp
    ret


L0: .long   L0_body
L0_body:    .asciz  "hello world\n"
```


#require packages:
  bison, flex


#how to make:
   $ make [TARGET_CPU=x86|arm]


#run a test program:
###example:
```
$ ./tiger.sh hello.tig
gcc -m32 -g -I../gc/boehmgc/include -c runtime/runtime.c -o obj/runtime.o
as -32 obj/hello.S -o obj/hello.o
gcc -m32 -g -L../gc/boehmgc/lib/x86 -static -lc  obj/runtime.o  obj/hello.o -lgc -o bin/hello
$ bin/hello
hello world
$
```

#run the test suite:
```
$ ./suite.sh [x86|arm]
```
