#include "Level.h"
#include <cassert>
#include "Frame.h"
#include "Symbol.h"
#include "debug.h"

using namespace std;

Level::Level(Level *p, Symbol *name, std::vector<int> escapes)
	: parent(p)
	, name(name)
{
	assert(parent);
	frame = parent->frame->newFrame(name, escapes);

	vector< Frame::Access* >::iterator it;
	it = frame->formals.begin();
	while (it != frame->formals.end()) {
		Level::Access *acc = gcnew(Level::Access, (this,(*it)));
		this->formals.push_back(acc);
		++it;
	}
}

Level::Level(Frame *f)
	: parent(NULL)
	, name(NULL)
{
	frame = f;
}


Level *
Level::newLevel(Level *p, Symbol *name, bool isLeaf, std::vector<int> escapes)
{
	DBG("newLevel");
	return gcnew(Level, (p, name, escapes));
}

Level *
Level::newLevel(Frame *f)
{
	DBG("newLevel");
	return gcnew(Level, (f));
}


Level::Access *
Level::allocLocal(bool escape)
{
	DBG("allocLocal");
	return gcnew(Level::Access, (this, frame->allocLocal(escape)));
}


Level::Access::Access(Level *h, Frame::Access *a)
	: home(h)
	, acc(a)
{
}


std::string 
Level::Access::toString()
{
	char buf[64];
	sprintf(buf, "access[level = %p, acc = %p]", home, acc);
	return buf;
}


tree::Exp *
Level::Access::simpleVar(Level *lev)
{
	tree::Exp *e;
	tree::Exp *fp = gcnew(tree::TEMP, (lev->frame->fp()));
	while (home != lev) {
		fp = lev->frame->staticChain(fp);
		assert(lev->parent);
		lev = lev->parent;
	}
	e = acc->exp(fp);
	
	return e;
}
