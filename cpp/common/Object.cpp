#include "Object.h"
#include "HeapManager.h"

Object::Object()
{
}

Object::~Object()
{
}

void
Object::child(Object *o)
{
	children.push_back(o);
}

void
Object::mark()
{
	std::vector<Object*>::iterator i = children.begin();
	while (i != children.end()) {
		Object *o = *i;
		o->mark();
		++i;
	}
	HeapManager::instance()->protect(this);
}
