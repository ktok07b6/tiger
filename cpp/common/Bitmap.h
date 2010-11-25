#ifndef BITMAP_H
#define BITMAP_H

#include <string>

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


#endif //BITMAP_H
