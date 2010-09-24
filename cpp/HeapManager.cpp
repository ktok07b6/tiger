#include "HeapManager.h"
#include <map>
#include "debug.h"
#include "Object.h"

using namespace std;
struct HeapInfo
{
	const char *exp;
	const char *file;
	int line;
	bool protect;
};

typedef map<Object *, HeapInfo> HeapMap;
HeapMap heapMap;

HeapManager::HeapManager()
{
}

HeapManager*
HeapManager::instance()
{
	if (!ptr) {
		ptr = new HeapManager();
	}
	return ptr;
}
HeapManager*
HeapManager::ptr;

Object *
HeapManager::regist(Object *p, const char *exp, const char *file, int line)
{
	HeapInfo hi;
	hi.exp = exp;
	hi.file = file;
	hi.line = line;
	hi.protect = false;
	heapMap.insert(make_pair(p, hi));
	return p;
}

void
HeapManager::unregist(Object *p)
{
	HeapMap::iterator i = heapMap.find(p);
	if (i != heapMap.end()) {
		heapMap.erase(p);
	} else {
		ERROR("failed to unregist %p", p);
	}
}

void
HeapManager::protect(Object *p)
{
}

void
HeapManager::unprotect(Object *p)
{
}

void
HeapManager::clean()
{
	//copy container for remove in iteration
	HeapMap cleanMap = heapMap;
	HeapMap::iterator i = cleanMap.begin();
	while (i != cleanMap.end()) {
		const pair<Object*, HeapInfo> &elem = *i;
		Object *p  = elem.first;
		bool protect = elem.second.protect;
		if (!protect) {
			unregist(p);
			delete p;
		}
		++i;
	}
}

void
HeapManager::dump()
{
	DBG("============ HeepManager dump =============");
	HeapMap::iterator i = heapMap.begin();
	while (i != heapMap.end()) {
		pair<Object*, HeapInfo> elem = *i;
		Object *p  = elem.first;
		const char *exp = elem.second.exp;
		const char *file = elem.second.file;
		int line = elem.second.line;
		DBG("-------------------");
		DBG("%s[%d]", file, line);
		DBG("%s[%p]", exp, p);
		++i;
	}
	DBG("===========================================");
}

void
HeapManager::addRoot(Object *p)
{
	roots.push_back(p);
}

void
HeapManager::mark()
{
	std::vector<Object*>::iterator i = roots.begin();
	while (i != roots.end()) {
		Object *o = *i;
		o->mark();
		++i;
	}
}

void 
HeapManager::resetMark()
{
	HeapMap::iterator i = heapMap.begin();
	while (i != heapMap.end()) {
		i->second.protect = false;;
		++i;
	}
}

void
HeapManager::sweep()
{
	clean();
}
