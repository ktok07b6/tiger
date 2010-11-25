#include "Bitmap.h"
#include <string.h>
#include <assert.h>

Bitmap::Bitmap()
: maxbit (0)
, capacity(0) 
{
	d.bit32 = 0;
}

Bitmap::Bitmap(unsigned int size)
: maxbit (size)
, capacity(size==0 ? 1 : ((maxbit-1) / 32 + 1)) 
{
	if (capacity > 1) {
		d.bits = new unsigned int [capacity];
		memset(d.bits, 0, capacity * 4);
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
, capacity(size==0 ? 1 : ((maxbit-1) / 32 + 1))
{
	if (capacity > 1) {
		d.bits = new unsigned int [capacity];
		memcpy(d.bits, initd, capacity * 4);
	} else {
		d.bit32 = initd[0];
	}
}

Bitmap::Bitmap(const Bitmap &other)
: maxbit (other.maxbit)
, capacity(other.capacity) 
{
	if (capacity > 1) {
		d.bits = new unsigned int [capacity];
		memcpy(d.bits, other.d.bits, capacity * 4);
	} else {
		d.bit32 = other.d.bit32;
	}
}


Bitmap::~Bitmap() 
{
	if (capacity > 1) {
		delete [] d.bits;
	}
}


unsigned int
Bitmap::right()
{
	//TODO: improvement perfomance
	if (capacity == 1) {
		for (unsigned int i = 0; i < maxbit; ++i) {
			if (d.bit32 & (1 << i)) {
				return i;
			}
		}
	} else {
		//FIXME
		for (unsigned int n = 0; n < capacity; ++n) {
			for (unsigned int i = 0; i < 32; ++i) {
				if (d.bits[n] & (1 << i)) {
					return capacity * 32 + i;
				}
			}
		}
	}

	return -1;
}

void 
Bitmap::set(unsigned int index) 
{
	assert(capacity != 0);
	assert(index < maxbit);
	if (capacity == 1) {
		d.bit32 |= (1<<index);
	} else {
		int n = index / 32;
		int idx = index % 32;
		d.bits[n] |= (1<<idx);
	}
}

void 
Bitmap::reset(unsigned int index) 
{
	assert(capacity != 0);
	assert(index < maxbit);
	if (capacity == 1) {
		d.bit32 &= ~(1<<index);
	} else {
		int n = index / 32;
		int idx = index % 32;
		d.bits[n] &= ~(1<<idx);
	}
}

void 
Bitmap::clear() 
{
	if (capacity == 1) {
		d.bit32 = 0;
	} else {
		memset(d.bits, 0, capacity * 4);
	}
}
	
bool 
Bitmap::get(unsigned int index) const
{
	assert(capacity != 0);
	assert(index < maxbit);
	if (capacity == 1) {
		return (d.bit32 & (1<<index)) != 0;
	} else {
		int n = index / 32;
		int idx = index % 32;
		return (d.bits[n] & (1<<idx)) != 0;
	}
}

bool 
Bitmap::none() const
{
	return d.bit32 == 0;
	assert(capacity != 0);
	if (capacity == 1) {
		return d.bit32 == 0;
	} else {
		unsigned int bits = 0;
		for (unsigned int n = 0; n < capacity; ++n) {
			return bits |= d.bits[n];
		}
		return bits == 0;
	}
}

size_t 
Bitmap::size() const
{
	return maxbit;
}

void 
Bitmap::flip()
{
	if (capacity == 1) {
		d.bit32 = ~d.bit32;
	} else {
		for (unsigned int n = 0; n < capacity; ++n) {
			d.bits[n] = ~d.bits[n];
		}
	}
}

Bitmap 
Bitmap::operator|(const Bitmap &other) const
{
	if (capacity == 1) {
		return Bitmap(maxbit, d.bit32 | other.d.bit32);
	} else {
		unsigned int *bits = new unsigned int[capacity];
		for (unsigned int n = 0; n < capacity; ++n) {
			bits[n] = d.bits[n] | other.d.bits[n];
		}
		Bitmap b(maxbit, bits);
		delete [] bits;
		return b;
	}
}

Bitmap 
Bitmap::operator-(const Bitmap &other) const
{
	if (capacity == 1) {
		return Bitmap(maxbit, d.bit32 & (~other.d.bit32));
	} else {
		unsigned int *bits = new unsigned int[capacity];
		for (unsigned int n = 0; n < capacity; ++n) {
			bits[n] = d.bits[n] & ~(other.d.bits[n]);
		}
		Bitmap b(maxbit, bits);
		delete [] bits;
		return b;
	}
}

Bitmap 
Bitmap::operator &(const Bitmap &other) const
{
	if (capacity == 1) {
		return Bitmap(maxbit, d.bit32 & other.d.bit32);
	} else {
		unsigned int *bits = new unsigned int[capacity];
		for (unsigned int n = 0; n < capacity; ++n) {
			bits[n] = d.bits[n] & other.d.bits[n];
		}
		Bitmap b(maxbit, bits);
		delete [] bits;
		return b;
	}
}
	
Bitmap 
Bitmap::operator ~() const
{
	if (capacity == 1) {
		return Bitmap(maxbit, ~d.bit32);
	} else {
		unsigned int *bits = new unsigned int[capacity];
		for (unsigned int n = 0; n < capacity; ++n) {
			bits[n] = ~(d.bits[n]);
		}
		Bitmap b(maxbit, bits);
		delete [] bits;
		return b;
	}
}

bool 
Bitmap::operator[](unsigned int index)
{
	assert(capacity != 0);
	assert(index < maxbit);
	return get(index);
}

bool 
Bitmap::operator==(const Bitmap &other) const
{
	if (capacity == 1) {
		return d.bit32 == other.d.bit32;
	} else {
		for (unsigned int n = 0; n < capacity; ++n) {
			if (d.bits[n] != other.d.bits[n]) return false;
		}
		return true;
	}
}

bool 
Bitmap::operator!=(const Bitmap &other) const
{
	return !this->operator==(other);
}


Bitmap &
Bitmap::operator=(const Bitmap &other)
{
	expandIfEmpty(other);

	if (capacity == 1) {
		d.bit32 = other.d.bit32;
	} else {
		memcpy(d.bits, other.d.bits, capacity * 4);
	}
	return *this;
}

void 
Bitmap::operator|=(const Bitmap &other)
{
	expandIfEmpty(other);

	if (capacity == 1) {
		d.bit32 |= other.d.bit32;
	} else {
		for (unsigned int n = 0; n < capacity; ++n) {
			d.bits[n] |= other.d.bits[n];
		}
	}
}

void 
Bitmap::operator-=(const Bitmap &other)
{
	expandIfEmpty(other);

	if (capacity == 1) {
		d.bit32 &= ~other.d.bit32;
	} else {
		for (unsigned int n = 0; n < capacity; ++n) {
			d.bits[n] &= ~other.d.bits[n];
		}
	}
}

void 
Bitmap::operator&=(const Bitmap &other)
{
	expandIfEmpty(other);

	if (capacity == 1) {
		d.bit32 &= other.d.bit32;
	} else {
		for (unsigned int n = 0; n < capacity; ++n) {
			d.bits[n] &= other.d.bits[n];
		}
	}
}

std::string 
Bitmap::toString() const
{
	std::string s;
	for (unsigned int i = 0; i < maxbit; ++i) {
		if (get(i)) {
			s = "1" + s;
		} else {
			s = "0" + s;
		}
	}
	return s;
}

void
Bitmap::expandIfEmpty(const Bitmap &other)
{
	if (capacity == 0) {
		if (other.capacity > 1) {
			d.bits = new unsigned int [capacity];
		}
		capacity = other.capacity;
		maxbit = other.maxbit;
	}
	assert(capacity == other.capacity);
	assert(maxbit == other.maxbit);
}
