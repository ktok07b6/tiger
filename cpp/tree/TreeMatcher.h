#ifndef TREE_MATCHER_H
#define TREE_MATCHER_H

#include "Tree.h"
namespace tree {

class TreeMatcher
{
 public:
	TreeMatcher(Tree::TypeID tid, Tree **tr);
	virtual ~TreeMatcher() {}
	virtual bool operator==(Tree *t) const;
 protected:
	Tree::TypeID tid;
	Tree **tr;
};

class TreeMatcher0 : public TreeMatcher
{
 public:
	TreeMatcher0(Tree::TypeID tid, Tree **tr);
	bool operator==(Tree *t) const;
};

class TreeMatcher1 : public TreeMatcher
{
 public:
	TreeMatcher1(Tree::TypeID tid, Tree **tr, const TreeMatcher &node1);
	bool operator==(Tree *t) const;
 private:
	const TreeMatcher &node1;
};

class TreeMatcher2 : public TreeMatcher
{
 public:
	TreeMatcher2(Tree::TypeID tid, Tree **tr, const TreeMatcher &node1, const TreeMatcher &node2);
	bool operator==(Tree *t) const;
 private:
	const TreeMatcher &node1;
	const TreeMatcher &node2;
};

#define _M0(t, tr) tree::TreeMatcher0(tree::Tree::t, (tree::Tree**)&tr)
#define _M1(t, tr, n1) tree::TreeMatcher1(tree::Tree::t, (tree::Tree**)&tr, n1)
#define _M2(t, tr, n1, n2) tree::TreeMatcher2(tree::Tree::t, (tree::Tree**)&tr, n1, n2)

#define _MMEM0(tr) tree::TreeMatcher0(tree::Tree::MEM_T, (tree::Tree**)&tr)
#define _MMEM(tr, n1) tree::TreeMatcher1(tree::Tree::MEM_T, (tree::Tree**)&tr, n1)

#define _MCONST(tr) tree::TreeMatcher0(tree::Tree::CONST_T, (tree::Tree**)&tr)

#define _MMOVE0(tr) tree::TreeMatcher0(tree::Tree::MOVE_T, (tree::Tree**)&tr)
#define _MMOVE(tr, n1, n2) tree::TreeMatcher2(tree::Tree::MOVE_T, (tree::Tree**)&tr, n1, n2)

#define _MCALL0(tr) tree::TreeMatcher0(tree::Tree::CALL_T, (tree::Tree**)&tr)

}//namespace tree
#endif //TREE_MATCHER_H
