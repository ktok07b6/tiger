#include "Bitmap.h"
#include <assert.h>
#include <stdio.h>

int main(int argc, char **argv)
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

	assert(!b[0]);
	assert(b[1]);
	assert(b[2]);
	assert(b[3]);

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

	printf("\nOK\n\n");
	return 0;
}
