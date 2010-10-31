#ifndef PROPERTY_H
#define PROPERTY_H

template <typename T>
class Property
{
public:

	Property() {}
 Property(const Property &other): elem(other.elem) {}

	//read
	operator T() {
		return elem;
	}

	operator T* () {
		return &elem;
	}

	operator T &() {
		return elem;
	}

	operator const T &() const {
		return elem;
	}


	//write
private:
	T elem;
};
#endif //
