#ifndef TREE_MATCHER_H
#define TREE_MATCHER_H

#include "Tree.h"
namespace tree {

class TreeMatcher
{
 public:
	TreeMatcher(Tree **tr);
	virtual ~TreeMatcher() {}
	virtual bool operator==(Tree *t) const = 0;
 protected:
	Tree **tr;
};

class TreeMatcher0 : public TreeMatcher
{
 public:
	TreeMatcher0(Tree **tr);
	bool operator==(Tree *t) const;
};

class TreeMatcher1 : public TreeMatcher
{
 public:
	TreeMatcher1(Tree **tr, const TreeMatcher &node1);
	bool operator==(Tree *t) const;
 private:
	const TreeMatcher &node1;
};

class TreeMatcher2 : public TreeMatcher
{
 public:
	TreeMatcher2(Tree **tr, const TreeMatcher &node1, const TreeMatcher &node2);
	bool operator==(Tree *t) const;
 private:
	const TreeMatcher &node1;
	const TreeMatcher &node2;
};

#define _M0(tr) tree::TreeMatcher0((tree::Tree**)&tr)
#define _M1(tr, n1) tree::TreeMatcher1((tree::Tree**)&tr, n1)
#define _M2(tr, n1, n2) tree::TreeMatcher2((tree::Tree**)&tr, n1, n2)

}//namespace tree
#endif //TREE_MATCHER_H
