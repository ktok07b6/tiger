#ifndef BITMAP_H
#define BITMAP_H

#include <string>

class Bitmap
{
public:
	Bitmap(unsigned int size);
	Bitmap(unsigned int size, unsigned int initd);
	Bitmap(unsigned int size, unsigned int *initd);
	~Bitmap();

	unsigned int right();
	void set(unsigned int index);
	void reset(unsigned int index);
	void clear();
	bool get(unsigned int index) const;
	bool none() const;
	size_t size() const;

	void flip();

	Bitmap operator|(const Bitmap &other) const;
	Bitmap operator &(const Bitmap &other) const;
	Bitmap operator ~() const;
	bool operator[](unsigned int index) const;

	void operator|=(const Bitmap &other);
	void operator-=(const Bitmap &other);
	void operator&=(const Bitmap &other);

	std::string toString() const;

private:
	union Data{
		unsigned int bit32;
		unsigned int *bits;
	} d;

	const unsigned int maxbit;
	const unsigned int capacity;
};

inline unsigned int
Bitmap::right()
{
	//TODO: improvement perfomance
	for (int i = 0; i < maxbit; ++i) {
		if (d.bit32 & (1 << i)) {
			return i;
		}
	}
	return -1;
}

inline void 
Bitmap::set(unsigned int index) 
{
	if (capacity == 1) {
		d.bit32 |= (1<<index);
	} else {
	}
}

inline void 
Bitmap::reset(unsigned int index) 
{
	d.bit32 &= ~(1<<index);
}

inline void 
Bitmap::clear() 
{
	d.bit32 = 0;
}
	
inline bool 
Bitmap::get(unsigned int index) const
{
	return d.bit32 & (1<<index);
}

inline bool 
Bitmap::none() const
{
	return d.bit32 == 0;
}

inline size_t 
Bitmap::size() const
{
	return maxbit;
}

inline void 
Bitmap::flip()
{
	d.bit32 = ~d.bit32;
}

inline Bitmap 
Bitmap::operator|(const Bitmap &other) const
{
	return Bitmap(maxbit, d.bit32 | other.d.bit32);
}

inline Bitmap 
Bitmap::operator &(const Bitmap &other) const
{
	return Bitmap(maxbit, d.bit32 & other.d.bit32);
}
	
inline Bitmap 
Bitmap::operator ~() const
{
	return Bitmap(maxbit, ~d.bit32);
}

inline bool 
Bitmap::operator[](unsigned int index) const
{
	return get(index);
}

inline void 
Bitmap::operator|=(const Bitmap &other)
{
	d.bit32 |= other.d.bit32;
}

inline void 
Bitmap::operator-=(const Bitmap &other)
{
	d.bit32 &= ~other.d.bit32;
}

inline void 
Bitmap::operator&=(const Bitmap &other)
{
	d.bit32 &= other.d.bit32;
}

inline std::string 
Bitmap::toString() const
{
	std::string s;
	for (int i = 0; i < maxbit; ++i) {
		if (get(i)) {
			s = "1" + s;
		} else {
			s = "0" + s;
		}
	}
	return s;
}

#endif //BITMAP_H
