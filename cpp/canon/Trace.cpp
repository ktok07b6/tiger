#include "Trace.h"

using namespace std;

Trace::Trace(const list<tree::StmList> &blocks)
	: blocks(blocks)
{
	doTrace();
}

void
Trace::doTrace()
{
	list< tree::StmList >::iterator it = blocks.begin();
	while (it != blocks.end()) {
		int index = std::distance(blocks.begin(), it);

		tree::Stm *s = it->li.front();
		assert(s->isLABEL_T());
		tree::LABEL *l = (tree::LABEL*)s;

		table.insert(make_pair(l->label, index));
		++it;
	}
	it = blocks.begin();
	while (it != blocks.end()) {
		tree::StmList &block = *it;

		tree::LABEL *l = (tree::LABEL*)it->li.front();
		TraceTable::iterator i2 = table.find(l->label);
		if (i2 != table.end()) {
			traceBlock(block);
		}
		++it;
	}
}

void
Trace::traceBlock(const tree::StmList &block)
{
	tree::LABEL *l = (tree::LABEL*)(block.front());
	table.erase(l->label);
	traced.push_all(block);
	
	tree::Stm *s = block.back();
	if (s->isJUMP_T()) {
		tree::JUMP *j = (tree::JUMP*)s;
		traceJUMP(j);
	} else 	if (s->isCJUMP_T()) {
		tree::CJUMP *cj = (tree::CJUMP*)s;
		traceCJUMP(cj);
	} else {
		assert(0);
	}
}

void
Trace::traceJUMP(tree::JUMP *j)
{
	if (j->targets.size() == 1) {
		Label *label = j->targets.front();
		TraceTable::iterator it = table.find(label);
		if (it != table.end()) {
			const tree::StmList &block = getBlock(it->second);
			//remove the JUMP statement
			assert(traced.back()->isJUMP_T());
			traced.pop_back();

			traceBlock(block);
		}
	}
}

void
Trace::traceCJUMP(tree::CJUMP *cj)
{
	TraceTable::iterator it_f;
	TraceTable::iterator it_t;
	it_f = table.find(cj->falselab);
	it_t = table.find(cj->truelab);
	if (it_f != table.end()) {
		const tree::StmList &block = getBlock(it_f->second);
		traceBlock(block);
	}
	else if (it_t != table.end()) {
		const tree::StmList &block = getBlock(it_t->second);
		//TODO:
		//replace the CJUMP
		assert(traced.back()->isCJUMP_T());
		traced.pop_back();
		tree::CJUMP *new_cj = _CJUMP(tree::CJUMP::notRel(cj->relop), 
									 cj->l, 
									 cj->r,
									 cj->falselab,
									 cj->truelab);
		traced.push_back(new_cj);
		traceBlock(block);
	}
	else {
		Label *falselab = gcnew(Label, ());
		traced.pop_back();
		tree::CJUMP *new_cj = _CJUMP(cj->relop, cj->l, cj->r, cj->truelab, falselab);
		traced.push_back(new_cj);
		traced.push_back(_LABEL(falselab));
		traced.push_back(_JUMP(cj->falselab));
	}
}

const tree::StmList &
Trace::getBlock(int index)
{
	list< tree::StmList >::iterator it = blocks.begin();
	std::advance(it, index);
	return *it;
}
