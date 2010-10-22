#include "TreeMatcher.h"
#include "Tree.h"

namespace tree{


TreeMatcher::TreeMatcher(Tree **tr)
	: tr(tr)
{}


TreeMatcher0::TreeMatcher0(Tree **tr)
	: TreeMatcher(tr)
{}

bool 
TreeMatcher0::operator==(Tree *t) const
{
	if ((*tr)->id != t->id) {
		return false;
	}
	*tr = t;
	return true;
}

TreeMatcher1::TreeMatcher1(Tree **tr, const TreeMatcher &node1)
	: TreeMatcher(tr)
	, node1(node1)
{}


bool 
TreeMatcher1::operator==(Tree *t) const
{
	if ((*tr)->id != t->id) {
		return false;
	}
	*tr = t;

	switch((*tr)->id) {
	case Tree::MEM_T:
		return (node1) == ((MEM*)t)->exp;
	case Tree::CALL_T:
		return (node1) == ((CALL*)t)->func;
	case Tree::EXPR_T:
		return (node1) == ((EXPR*)t)->exp;

		//following tree has 2 nodes
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


TreeMatcher2::TreeMatcher2(Tree **tr, const TreeMatcher &node1, const TreeMatcher &node2)
	: TreeMatcher(tr)
	, node1(node1)
	, node2(node2)
{}

bool 
TreeMatcher2::operator==(Tree *t) const
{
	if ((*tr)->id != t->id) {
		return false;
	}
	*tr = t;

	switch((*tr)->id) {
		//following tree has 2 nodes
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

