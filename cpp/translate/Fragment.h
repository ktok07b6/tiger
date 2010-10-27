#ifndef FRAGMENT_H
#define FRAGMENT_H

#include "Object.h"
#include <list>
#include <string>
namespace tree {
	class Stm;
}
class Frame;

class Fragment : public Object
{
};

class ProcFragment : public Fragment
{
 public:
	ProcFragment(tree::Stm *stm, Frame *frame);
 private:
	tree::Stm *stm;
	Frame *frame;
};

class DataFragment : public Fragment
{
 public:
	DataFragment(const std::string &s);
 private:
	std::string data;
};

typedef std::list<Fragment*> FragmentList;

#endif //FRAGMENT_H
