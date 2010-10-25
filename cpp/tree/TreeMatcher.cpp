#include "TreeMatcher.h"
#include "Tree.h"

namespace tree{


TreeMatcher::TreeMatcher(Tree::TypeID tid, Tree **tr)
	: tid(tid)
	, tr(tr)
{}

bool 
TreeMatcher::operator==(Tree *t) const
{
	if (tid != t->id) {
		return false;
	}
	*tr = t;
	return true;
}

TreeMatcher0::TreeMatcher0(Tree::TypeID tid, Tree **tr)
	: TreeMatcher(tid, tr)
{}

bool 
TreeMatcher0::operator==(Tree *t) const
{
	return TreeMatcher::operator==(t);
}

TreeMatcher1::TreeMatcher1(Tree::TypeID tid, Tree **tr, const TreeMatcher &node1)
	: TreeMatcher(tid, tr)
	, node1(node1)
{}


bool 
TreeMatcher1::operator==(Tree *t) const
{
	if (!TreeMatcher::operator==(t)) {
		return false;
	}

	switch(tid) {
	case Tree::MEM_T:
		return (node1) == ((MEM*)t)->exp;
	case Tree::CALL_T:
		return (node1) == ((CALL*)t)->func;
	case Tree::EXPR_T:
		return (node1) == ((EXPR*)t)->exp;

	//following tree has 2 nodes,
	//but only one of them is tested here.
	case Tree::BINOP_T:
		return (node1) == ((BINOP*)t)->l;
	case Tree::ESEQ_T:
		return (node1) == ((ESEQ*)t)->stm;
	case Tree::CJUMP_T:
		return (node1) == ((CJUMP*)t)->l;
	case Tree::MOVE_T:
		return (node1) == ((MOVE*)t)->dst;
	case Tree::SEQ_T:
		return (node1) == ((SEQ*)t)->l;
	default:
		assert(0);
		return false;
	}
}


TreeMatcher2::TreeMatcher2(Tree::TypeID tid, Tree **tr, const TreeMatcher &node1, const TreeMatcher &node2)
	: TreeMatcher(tid, tr)
	, node1(node1)
	, node2(node2)
{}

bool 
TreeMatcher2::operator==(Tree *t) const
{
	if (!TreeMatcher::operator==(t)) {
		return false;
	}

	switch(tid) {
	//a tree is tested that has 2 nodes
	case Tree::BINOP_T:
		return (node1) == ((BINOP*)t)->l &&
			(node2) == ((BINOP*)t)->r;
	case Tree::ESEQ_T:
		return (node1) == ((ESEQ*)t)->stm &&
			(node2) == ((ESEQ*)t)->exp;
	case Tree::CJUMP_T:
		return (node1) == ((CJUMP*)t)->l &&
			(node2) == ((CJUMP*)t)->r;
	case Tree::MOVE_T:
		return (node1) == ((MOVE*)t)->dst &&
			(node2) == ((MOVE*)t)->src;
	case Tree::SEQ_T:
		return (node1) == ((SEQ*)t)->l &&
			(node2) == ((SEQ*)t)->r;
	default:
		assert(0);
		return false;
	}

	return true;
}


} //namespace tree

