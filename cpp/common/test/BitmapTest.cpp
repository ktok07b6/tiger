#include "Bitmap.h"
#include <assert.h>
#include <stdio.h>

void test1();
void test2();

int main(int argc, char **argv)
{
	test1();
	test2();

	printf("\nOK\n\n");
	return 0;
}

void test1()
{
	Bitmap b(8);
	Bitmap b2(8);
	Bitmap b3(8);

	std::string s;
	assert(b.size() == 8);
	assert(b.none());

	b.set(0);
	b.set(2);
	b.set(4);

	assert(b.get(0));
	assert(!b.get(1));
	assert(b.get(2));
	assert(!b.get(3));
	assert(b.get(4));

	assert(b.toString() == "00010101");

	b.reset(0);
	b.reset(2);
	b.reset(4);

	assert(!b.get(0));
	assert(!b.get(1));
	assert(!b.get(2));
	assert(!b.get(3));
	assert(!b.get(4));

	assert(b.none());
	assert(b.toString() == "00000000");

	b.set(0);
	b2.set(0);
	b2.set(1);
	b2.set(2);

	Bitmap r1 = b | b2;
	assert(r1.get(0));
	assert(r1.get(1));
	assert(r1.get(2));
	assert(!r1.get(3));

	Bitmap r2 = b & b2;
	assert(r2.get(0));
	assert(!r2.get(1));
	assert(!r2.get(2));
	assert(!r2.get(3));
	
	b.flip();
	assert(!b.get(0));
	assert(b.get(1));
	assert(b.get(2));
	assert(b.get(3));
	
	Bitmap r3 = ~b;
	assert(r3.get(0));
	assert(!r3.get(1));
	assert(!r3.get(2));
	assert(!r3.get(3));
	/*
	assert(!b[0]);
	assert(b[1]);
	assert(b[2]);
	assert(b[3]);
	*/
	b.clear();
	assert(b.none());

	b |= b2;
	assert(b.get(0));
	assert(b.get(1));
	assert(b.get(2));
	assert(!b.get(3));

	b -= b2;
	assert(b.none());

	b.set(2);
	b &= b2;
	assert(!b.get(0));
	assert(!b.get(1));
	assert(b.get(2));
	assert(!b.get(3));

	Bitmap zero;
	zero = b;
	assert(zero == b);
	assert(!zero.get(0));
	assert(!zero.get(1));
	assert(zero.get(2));
	assert(!zero.get(3));

	Bitmap zero2;
	zero2 |= b;
	assert(zero2 == b);
	assert(!zero2.get(0));
	assert(!zero2.get(1));
	assert(zero2.get(2));
	assert(!zero2.get(3));

	Bitmap zero3;
	assert(zero3.none());
	zero3 -= b;
	assert(zero3.none());

	Bitmap zero4;
	zero4 &= b;
	assert(zero4.none());
}

//--------------------------------
void test2()
{
	Bitmap b(40);
	Bitmap b2(40);
	Bitmap b3(40);

	std::string s;
	assert(b.size() == 40);
	assert(b.none());

	b.set(30);
	b.set(32);
	b.set(34);

	assert(b.get(30));
	assert(!b.get(31));
	assert(b.get(32));
	assert(!b.get(33));
	assert(b.get(34));

	assert(b.toString() == "0000010101000000000000000000000000000000");

	b.reset(30);
	b.reset(32);
	b.reset(34);

	assert(!b.get(30));
	assert(!b.get(31));
	assert(!b.get(32));
	assert(!b.get(33));
	assert(!b.get(34));

	assert(b.none());
	assert(b.toString() == "0000000000000000000000000000000000000000");

	b.set(30);
	b2.set(30);
	b2.set(31);
	b2.set(32);

	Bitmap r1 = b | b2;
	assert(r1.get(30));
	assert(r1.get(31));
	assert(r1.get(32));
	assert(!r1.get(33));

	Bitmap r2 = b & b2;
	assert(r2.get(30));
	assert(!r2.get(31));
	assert(!r2.get(32));
	assert(!r2.get(33));
	
	b.flip();
	assert(!b.get(30));
	assert(b.get(31));
	assert(b.get(32));
	assert(b.get(33));
	
	Bitmap r3 = ~b;
	assert(r3.get(30));
	assert(!r3.get(31));
	assert(!r3.get(32));
	assert(!r3.get(33));
	/*
	assert(!b[0]);
	assert(b[1]);
	assert(b[2]);
	assert(b[3]);
	*/
	b.clear();
	assert(b.none());

	b |= b2;
	assert(b.get(30));
	assert(b.get(31));
	assert(b.get(32));
	assert(!b.get(33));

	b -= b2;
	assert(b.none());

	b.set(32);
	b &= b2;
	assert(!b.get(30));
	assert(!b.get(31));
	assert(b.get(32));
	assert(!b.get(33));

	Bitmap zero;
	zero = b;
	assert(zero == b);
	assert(!zero.get(30));
	assert(!zero.get(31));
	assert(zero.get(32));
	assert(!zero.get(33));

	Bitmap zero2;
	zero2 |= b;
	assert(zero2 == b);
	assert(!zero2.get(30));
	assert(!zero2.get(31));
	assert(zero2.get(32));
	assert(!zero2.get(33));

	Bitmap zero3;
	assert(zero3.none());
	zero3 -= b;
	assert(zero3.none());

	Bitmap zero4;
	zero4 &= b;
	assert(zero4.none());
}
