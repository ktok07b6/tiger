#ifndef TREE_LIST_H
#define TREE_LIST_H

#include <list>

template<class T>
class TreeList
{
 public:
	TreeList();
	T *front();
	T *back();
	T *front() const;
	T *back() const;

	void push_front(T *);
	void push_back(T *);
	void push_all(const TreeList &other);
	T *pop_front();
	T *pop_back();
	bool empty();
	void clear();

	std::list<T*> li;
};

template<class T>
TreeList<T>::TreeList()
{
}

template<class T>
T *
TreeList<T>::front()
{
	if (!li.empty()) {
		return li.front();
	} else {
		return NULL;
	}
}

template<class T>
T *
TreeList<T>::back()
{
	if (!li.empty()) {
		return li.back();
	} else {
		return NULL;
	}
}

template<class T>
T *
TreeList<T>::front() const
{
	if (!li.empty()) {
		return li.front();
	} else {
		return NULL;
	}
}

template<class T>
T *
TreeList<T>::back() const
{
	if (!li.empty()) {
		return li.back();
	} else {
		return NULL;
	}
}

template<class T>
void
TreeList<T>::push_front(T *p)
{
	li.push_front(p);
}

template<class T>
void
TreeList<T>::push_back(T *p)
{
	li.push_back(p);
}

template<class T>
void
TreeList<T>::push_all(const TreeList<T> &other)
{
	typename std::list<T*>::const_iterator it = other.li.begin();
	while (it != other.li.end()) {
		li.push_back(*it);
		++it;
	}
}

template<class T>
T *
TreeList<T>::pop_front()
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
TreeList<T>::pop_back()
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
TreeList<T>::empty()
{
	return li.empty();
}

template<class T>
void
TreeList<T>::clear()
{
	return li.clear();
}

#endif //TREE_LIST_H
