#ifndef BITMAP_H
#define BITMAP_H

#include <string>
#include <assert.h>

class Bitmap
{
public:
	Bitmap();
	Bitmap(unsigned int size);
	Bitmap(unsigned int size, unsigned int initd);
	Bitmap(unsigned int size, unsigned int *initd);
	Bitmap(const Bitmap &other);
	~Bitmap();

	unsigned int right();
	void set(unsigned int index);
	void reset(unsigned int index);
	void clear();
	bool get(unsigned int index) const;
	bool none() const;
	size_t size() const;

	void flip();

	Bitmap &operator=(const Bitmap &other);

	Bitmap operator|(const Bitmap &other) const;
	Bitmap operator-(const Bitmap &other) const;
	Bitmap operator &(const Bitmap &other) const;
	Bitmap operator ~() const;
	bool operator[](unsigned int index) const;

	bool operator==(const Bitmap &other) const;
	bool operator!=(const Bitmap &other) const;

	void operator|=(const Bitmap &other);
	void operator-=(const Bitmap &other);
	void operator&=(const Bitmap &other);

	operator const char *() {
		return this->toString().c_str();
	}
	std::string toString() const;

private:
	union Data{
		unsigned int bit32;
		unsigned int *bits;
	} d;

	unsigned int maxbit;
	unsigned int capacity;
};


inline unsigned int
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

inline void 
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

inline void 
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

inline void 
Bitmap::clear() 
{
	if (capacity == 1) {
		d.bit32 = 0;
	} else {
		memset(d.bits, 0, capacity * sizeof(int));
	}
}
	
inline bool 
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

inline bool 
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

inline size_t 
Bitmap::size() const
{
	return maxbit;
}

inline void 
Bitmap::flip()
{
	if (capacity == 0) {
		d.bit32 = ~d.bit32;
	} else {
		for (unsigned int n = 0; n < capacity; ++n) {
			d.bits[n] = ~d.bits[n];
		}
	}
}


inline Bitmap &
Bitmap::operator=(const Bitmap &other)
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
	
	if (capacity == 1) {
		d.bit32 = other.d.bit32;
	} else {
		memcpy(d.bits, other.d.bits, capacity * sizeof(int));
	}
	return *this;
}

inline Bitmap 
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

inline Bitmap 
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

inline Bitmap 
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
	
inline Bitmap 
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

inline bool 
Bitmap::operator[](unsigned int index) const
{
	assert(capacity != 0);
	assert(index < maxbit);
	return get(index);
}

inline bool 
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

inline bool 
Bitmap::operator!=(const Bitmap &other) const
{
	return !this->operator==(other);
}

inline void 
Bitmap::operator|=(const Bitmap &other)
{
	if (capacity == 1) {
		d.bit32 |= other.d.bit32;
	} else {
		for (unsigned int n = 0; n < capacity; ++n) {
			d.bits[n] |= other.d.bits[n];
		}
	}
}

inline void 
Bitmap::operator-=(const Bitmap &other)
{
	if (capacity == 1) {
		d.bit32 &= ~other.d.bit32;
	} else {
		for (unsigned int n = 0; n < capacity; ++n) {
			d.bits[n] &= ~other.d.bits[n];
		}
	}
}

inline void 
Bitmap::operator&=(const Bitmap &other)
{
	if (capacity == 1) {
		d.bit32 &= other.d.bit32;
	} else {
		for (unsigned int n = 0; n < capacity; ++n) {
			d.bits[n] &= other.d.bits[n];
		}
	}
}

inline std::string 
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

#endif //BITMAP_H
