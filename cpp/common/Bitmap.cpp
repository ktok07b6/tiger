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
					return n * 32 + i;
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
		int n = index >> 5;//index / 32;//
		int idx = index & 0x1F;//index % 32;
		return (d.bits[n] & (1<<idx)) != 0;
	}
}

void 
Bitmap::getIndexes(std::vector<int> &indexes) const
{
	if (capacity == 1) {
		unsigned int v = d.bit32;
		if (v & 0x0000FFFF) {
			if (v & 0x000000FF) {
				if (v & (1<<0)) indexes.push_back(0);
				if (v & (1<<1)) indexes.push_back(1);
				if (v & (1<<2)) indexes.push_back(2);
				if (v & (1<<3)) indexes.push_back(3);
				if (v & (1<<4)) indexes.push_back(4);
				if (v & (1<<5)) indexes.push_back(5);
				if (v & (1<<6)) indexes.push_back(6);
				if (v & (1<<7)) indexes.push_back(7);
			}
			if (v & 0x0000FF00) {
				if (v & (1<<8)) indexes.push_back(8);
				if (v & (1<<9)) indexes.push_back(9);
				if (v & (1<<10)) indexes.push_back(10);
				if (v & (1<<11)) indexes.push_back(11);
				if (v & (1<<12)) indexes.push_back(12);
				if (v & (1<<13)) indexes.push_back(13);
				if (v & (1<<14)) indexes.push_back(14);
				if (v & (1<<15)) indexes.push_back(15);
			}
		}
		if (v & 0xFFFF0000) {
			if (v & 0x00FF0000) {
				if (v & (1<<16)) indexes.push_back(16);
				if (v & (1<<17)) indexes.push_back(17);
				if (v & (1<<18)) indexes.push_back(18);
				if (v & (1<<19)) indexes.push_back(19);
				if (v & (1<<20)) indexes.push_back(20);
				if (v & (1<<21)) indexes.push_back(21);
				if (v & (1<<22)) indexes.push_back(22);
				if (v & (1<<23)) indexes.push_back(23);
			}
			if (v & 0xFF000000) {
				if (v & (1<<24)) indexes.push_back(24);
				if (v & (1<<25)) indexes.push_back(25);
				if (v & (1<<26)) indexes.push_back(26);
				if (v & (1<<27)) indexes.push_back(27);
				if (v & (1<<28)) indexes.push_back(28);
				if (v & (1<<29)) indexes.push_back(29);
				if (v & (1<<30)) indexes.push_back(30);
				if (v & (1<<31)) indexes.push_back(31);
			}
		}
	} else {
		unsigned int bits = 0;
		for (unsigned int n = 0; n < capacity; ++n) {
			unsigned int v = d.bits[n];
			int offset = n * 32;
			if (v & 0x0000FFFF) {
				if (v & 0x000000FF) {
					if (v & (1<<0)) indexes.push_back(0+offset);
					if (v & (1<<1)) indexes.push_back(1+offset);
					if (v & (1<<2)) indexes.push_back(2+offset);
					if (v & (1<<3)) indexes.push_back(3+offset);
					if (v & (1<<4)) indexes.push_back(4+offset);
					if (v & (1<<5)) indexes.push_back(5+offset);
					if (v & (1<<6)) indexes.push_back(6+offset);
					if (v & (1<<7)) indexes.push_back(7+offset);
				}
				if (v & 0x0000FF00) {
					if (v & (1<<8)) indexes.push_back(8+offset);
					if (v & (1<<9)) indexes.push_back(9+offset);
					if (v & (1<<10)) indexes.push_back(10+offset);
					if (v & (1<<11)) indexes.push_back(11+offset);
					if (v & (1<<12)) indexes.push_back(12+offset);
					if (v & (1<<13)) indexes.push_back(13+offset);
					if (v & (1<<14)) indexes.push_back(14+offset);
					if (v & (1<<15)) indexes.push_back(15+offset);
				}
			}
			if (v & 0xFFFF0000) {
				if (v & 0x00FF0000) {
					if (v & (1<<16)) indexes.push_back(16+offset);
					if (v & (1<<17)) indexes.push_back(17+offset);
					if (v & (1<<18)) indexes.push_back(18+offset);
					if (v & (1<<19)) indexes.push_back(19+offset);
					if (v & (1<<20)) indexes.push_back(20+offset);
					if (v & (1<<21)) indexes.push_back(21+offset);
					if (v & (1<<22)) indexes.push_back(22+offset);
					if (v & (1<<23)) indexes.push_back(23+offset);
				}
				if (v & 0xFF000000) {
					if (v & (1<<24)) indexes.push_back(24+offset);
					if (v & (1<<25)) indexes.push_back(25+offset);
					if (v & (1<<26)) indexes.push_back(26+offset);
					if (v & (1<<27)) indexes.push_back(27+offset);
					if (v & (1<<28)) indexes.push_back(28+offset);
					if (v & (1<<29)) indexes.push_back(29+offset);
					if (v & (1<<30)) indexes.push_back(30+offset);
					if (v & (1<<31)) indexes.push_back(31+offset);
				}
			}
		}
	}
}

bool 
Bitmap::none() const
{
	if (capacity == 0) {
		return true;
	} else if (capacity == 1) {
		return d.bit32 == 0;
	} else {
		unsigned int bits = 0;
		for (unsigned int n = 0; n < capacity; ++n) {
			bits |= d.bits[n];
		}
		return bits == 0;
	}
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
	assert(capacity == other.capacity);
	if (capacity == 0) {
		return Bitmap();
	} else if (capacity == 1) {
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
	assert(capacity == other.capacity);
	if (capacity == 0) {
		return Bitmap();
	} else if (capacity == 1) {
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
	assert(capacity == other.capacity);
	if (capacity == 0) {
		return Bitmap();
	} else if (capacity == 1) {
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
	if (capacity == 0) {
		return Bitmap();
	} else if (capacity == 1) {
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
	assert(capacity == other.capacity);
	if (capacity == 0) {
		return true;
	} else if (capacity == 1) {
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
