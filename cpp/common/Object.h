#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <vector>

class Object
{
 public:
	Object();
	virtual ~Object();
	void child(Object *);
	void mark();

 private:
	std::vector<Object*> children;
};

#endif //__OBJECT_H__
