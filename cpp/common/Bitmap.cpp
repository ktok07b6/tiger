#include "Bitmap.h"
#include <string.h>

Bitmap::Bitmap(unsigned int size)
: maxbit (size)
, capacity((maxbit-1) / 32 + 1) 
{
	if (capacity > 1) {
		d.bits = new unsigned int [capacity];
		memset(d.bits, 0, capacity);
	} else {
		d.bit32 = 0;
	}
}

Bitmap::Bitmap(unsigned int size, unsigned int initd)
: maxbit (size)
, capacity(1) 
{
	d.bit32 = initd;
}

Bitmap::Bitmap(unsigned int size, unsigned int *initd)
: maxbit (size)
, capacity((maxbit-1) / 32 + 1) 
{
	if (capacity > 1) {
		d.bits = new unsigned int [capacity];
		memcpy(d.bits, initd, capacity);
	} else {
		d.bit32 = initd[0];
	}
}

Bitmap::~Bitmap() 
{
	if (capacity > 1) {
		delete [] d.bits;
	}
}
