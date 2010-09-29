#ifndef LEVEL_H
#define LEVEL_H

#include "Object.h"
#include <vector>
#include <string>
#include "Frame.h"
#include "debug.h"

struct Symbol;
class LavelAccess;

namespace tree {
	class Exp;
}

class Level : public Object
{
 public:
	static Level *newLevel(Level *parent, Symbol *name, bool isLeaf, std::vector<int> escapes);
	static Level *newLevel(Frame *f);
	~Level() {FUNCLOG;}
	class Access : public Object
	{
	public:
		Access(Level *h, Frame::Access *a);
		std::string toString();
		tree::Exp *simpleVar(Level *current);
	private:
		Level *home;
		Frame::Access *acc;
	};

	Level::Access *allocLocal(bool escape);
	Frame *getFrame() { return frame;}

	std::vector<Level::Access*> formals;
	Level *parent;
 private:
	Level(Level *p, Symbol *name, std::vector<int> escapes);
	Level(Frame *f);

	Frame *frame;
	Symbol *name;
};

#endif //LEVEL_H
