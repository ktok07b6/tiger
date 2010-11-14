#ifndef VECTOR_H
#define VECTOR_H

#include <vector>
#include <algorithm>

template <class T>
class Vector
{
 public:
	typedef typename std::vector<T*>::iterator iterator;
	typedef typename std::vector<T*>::const_iterator const_iterator;

	Vector();

	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;

	T *front();
	T *back();
	T *front() const;
	T *back() const;
	T *at(int idx);
	T *at(int idx) const;

	void push_back(T *);
	void push_all(const Vector<T> &other);
	T *pop_back();
	bool empty() const;
	size_t size() const;
	void clear();
	void remove(T *);

	iterator find(T *);
	const_iterator find(T *) const;
	bool contain(T *) const;
	void unique();

	void operator+=(T *);
	void operator+=(const Vector<T> &);
	void operator-=(T *);
	void operator-=(const Vector<T> &);

	std::vector<T*> vec;
};

template<class T>
Vector<T>::Vector()
{
}

template<class T>
typename Vector<T>::iterator
Vector<T>::begin()
{
	return vec.begin();
}

template<class T>
typename Vector<T>::const_iterator
Vector<T>::begin() const
{
	return vec.begin();
}

template<class T>
typename Vector<T>::iterator
Vector<T>::end()
{
	return vec.end();
}

template<class T>
typename Vector<T>::const_iterator
Vector<T>::end() const
{
	return vec.end();
}

template<class T>
T *
Vector<T>::front()
{
	if (!vec.empty()) {
		return vec.front();
	} else {
		return NULL;
	}
}

template<class T>
T *
Vector<T>::back()
{
	if (!vec.empty()) {
		return vec.back();
	} else {
		return NULL;
	}
}

template<class T>
T *
Vector<T>::front() const
{
	if (!vec.empty()) {
		return vec.front();
	} else {
		return NULL;
	}
}

template<class T>
T *
Vector<T>::back() const
{
	if (!vec.empty()) {
		return vec.back();
	} else {
		return NULL;
	}
}

template<class T>
T *
Vector<T>::at(int idx)
{
	return vec.at(idx);
}

template<class T>
T *
Vector<T>::at(int idx) const
{
	return vec.at(idx);
}


template<class T>
void
Vector<T>::push_back(T *p)
{
	vec.push_back(p);
}

template<class T>
void
Vector<T>::push_all(const Vector<T> &other)
{
	const_iterator it = other.begin();
	while (it != other.end()) {
		push_back(*it);
		++it;
	}
}

template<class T>
T *
Vector<T>::pop_back()
{
	if (!vec.empty()) {
		T *p = vec.back();
		vec.pop_back();
		return p;
	} else {
		return NULL;
	}
}

template<class T>
bool
Vector<T>::empty() const
{
	return vec.empty();
}

template<class T>
size_t
Vector<T>::size() const
{
	return vec.size();
}

template<class T>
void
Vector<T>::clear()
{
	return vec.clear();
}

template<class T>
void 
Vector<T>::remove(T *n)
{
	iterator i = find(n);
	if (i != end()) {
		vec.erase(i);
	}
}

template<class T>
typename Vector<T>::const_iterator
Vector<T>::find(T *n) const
{
	return std::find(vec.begin(), vec.end(), n);
}

template<class T>
typename Vector<T>::iterator
Vector<T>::find(T *n)
{
	return std::find(vec.begin(), vec.end(), n);
}

template<class T>
bool
Vector<T>::contain(T *n) const
{
	return std::find(vec.begin(), vec.end(), n) != vec.end();
}

template<class T>
void
Vector<T>::unique()
{
	std::sort(begin(), end());
	vec.erase(std::unique(begin(), end()), end());
}

template<class T>
void 
Vector<T>::operator+=(T *n)
{
	push_back(n);
}

template<class T>
void 
Vector<T>::operator+=(const Vector<T> &other)
{
	push_all(other);
}

template<class T>
void 
Vector<T>::operator-=(T *n)
{
	remove(n);
}

template<class T>
void 
Vector<T>::operator-=(const Vector<T> &other)
{
	const_iterator it = other.begin();
	while (it != other.end()) {
		remove(*it);
		++it;
	}
}

#endif //VECTOR_H
