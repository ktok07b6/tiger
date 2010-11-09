#ifndef LIST_H
#define LIST_H

#include <list>
#include <algorithm>

template <class T>
class List
{
 public:
	typedef typename std::list<T*>::iterator iterator;
	typedef typename std::list<T*>::const_iterator const_iterator;

	List();

	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;

	T *front();
	T *back();
	T *front() const;
	T *back() const;

	void push_front(T *);
	void push_back(T *);
	void push_all(const List<T> &other);
	T *pop_front();
	T *pop_back();
	bool empty() const;
	size_t size() const;
	void clear();
	void remove(T *);

	iterator find(T *) const;
	bool contain(T *) const;
	void unique();
	List<T> intersection(const List<T> &);

	void operator+=(T *);
	void operator+=(const List<T> &);
	void operator-=(T *);
	void operator-=(const List<T> &);

	std::list<T*> li;
};

template<class T>
List<T>::List()
{
}

template<class T>
typename List<T>::iterator
List<T>::begin()
{
	return li.begin();
}

template<class T>
typename List<T>::const_iterator
List<T>::begin() const
{
	return li.begin();
}

template<class T>
typename List<T>::iterator
List<T>::end()
{
	return li.end();
}

template<class T>
typename List<T>::const_iterator
List<T>::end() const
{
	return li.end();
}

template<class T>
T *
List<T>::front()
{
	if (!li.empty()) {
		return li.front();
	} else {
		return NULL;
	}
}

template<class T>
T *
List<T>::back()
{
	if (!li.empty()) {
		return li.back();
	} else {
		return NULL;
	}
}

template<class T>
T *
List<T>::front() const
{
	if (!li.empty()) {
		return li.front();
	} else {
		return NULL;
	}
}

template<class T>
T *
List<T>::back() const
{
	if (!li.empty()) {
		return li.back();
	} else {
		return NULL;
	}
}

template<class T>
void
List<T>::push_front(T *p)
{
	li.push_front(p);
}

template<class T>
void
List<T>::push_back(T *p)
{
	li.push_back(p);
}

template<class T>
void
List<T>::push_all(const List<T> &other)
{
	const_iterator it = other.begin();
	while (it != other.end()) {
		push_back(*it);
		++it;
	}
}

template<class T>
T *
List<T>::pop_front()
{
	if (!li.empty()) {
		T *p = li.front();
		li.pop_front();
		return p;
	} else {
		return NULL;
	}
}

template<class T>
T *
List<T>::pop_back()
{
	if (!li.empty()) {
		T *p = li.back();
		li.pop_back();
		return p;
	} else {
		return NULL;
	}
}

template<class T>
bool
List<T>::empty() const
{
	return li.empty();
}

template<class T>
size_t
List<T>::size() const
{
	return li.size();
}

template<class T>
void
List<T>::clear()
{
	return li.clear();
}

template<class T>
void 
List<T>::remove(T *n)
{
	li.remove(n);
}

template<class T>
typename List<T>::iterator
List<T>::find(T *n) const
{
	return std::find(li.begin(), li.end(), n);
}

template<class T>
bool
List<T>::contain(T *n) const
{
	return std::find(li.begin(), li.end(), n) != li.end();
}

template<class T>
void
List<T>::unique()
{
	li.unique();
}

template<class T>
List<T> 
List<T>::intersection(const List<T> &other)
{
	List<T> result;
	const_iterator it = other.begin();
	while (it != other.end()) {
		if (contain(*it)) {
			result.push_back(*it);
		}
		++it;
	}
	return result;
}

template<class T>
void 
List<T>::operator+=(T *n)
{
	push_back(n);
}

template<class T>
void 
List<T>::operator+=(const List<T> &other)
{
	push_all(other);
}

template<class T>
void 
List<T>::operator-=(T *n)
{
	remove(n);
}

template<class T>
void 
List<T>::operator-=(const List<T> &other)
{
	const_iterator it = other.begin();
	while (it != other.end()) {
		remove(*it);
		++it;
	}
}

#endif //LIST_H
