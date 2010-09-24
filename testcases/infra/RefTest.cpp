#include "Ref.h"
#include <stdio.h>

struct B
{
	virtual ~B(){
		printf("B dtor\n");
	}
};

struct D : B
{
	~D(){
		printf("D dtor\n");
	}
};

int main(int argc, char **argv)
{
	Ref<D> d1 = new D();
	Ref<D> d2 = new D();
	d1 = d2;
	Ref<B> b1 = new D();
	//Ref<D> d3 = new B();
}
