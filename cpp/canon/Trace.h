#ifndef TRACE_H
#define TRACE_H

#include "Tree.h"
#include <map>

class Trace
{
 public:
	Trace(const std::list<tree::StmList> &blocks);
	tree::StmList traced;
 private:
	void doTrace();
	void traceBlock(const tree::StmList &block);
	void traceJUMP(tree::JUMP *j);
	void traceCJUMP(tree::CJUMP *cj);
	const tree::StmList &getBlock(int index);

	std::list<tree::StmList> blocks;


	typedef std::map<Label *, int> TraceTable;
	TraceTable table;
};
#endif //TRACE_H
