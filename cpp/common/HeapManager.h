#ifndef __HEAP_MANAGER_H__
#define __HEAP_MANAGER_H__

#include <vector>

class Object;

class HeapManager
{
 public:
	static HeapManager *instance();

	Object *regist(Object *, const char *, const char *, int);
	void unregist(Object *);
	void protect(Object *);
	void unprotect(Object *);
	void clean();
	void dump();

	void addRoot(Object *);
	void mark();
	void resetMark();
	void sweep();
 private:
	HeapManager();
	static HeapManager *ptr;
	std::vector<Object*> roots;
};


#define gcnew(type, exp) (type *)HeapManager::instance()->regist(new type exp, #type#exp, __FILE__, __LINE__)


#endif //__HEAP_MANAGER_H__
