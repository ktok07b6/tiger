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
protected:
	enum TypeID { PROC_T, DATA_T };
	const TypeID typeID;

public:
 Fragment(TypeID id) 
	 : typeID(id) {}
	virtual std::string toString() = 0;

	bool isProc() const;
	bool isData() const;
};

inline bool
Fragment::isProc() const
{
	return typeID == PROC_T;
}
inline bool
Fragment::isData() const
{
	return typeID == DATA_T;
}

class ProcFragment : public Fragment
{
 public:
	ProcFragment(tree::Stm *stm, Frame *frame);
	virtual std::string toString();
	tree::Stm *getStm();
	Frame *getFrame();
 private:
	tree::Stm *stm;
	Frame *frame;
};

class DataFragment : public Fragment
{
 public:
	DataFragment(const std::string &s);
	virtual std::string toString();
 private:
	std::string data;
};

typedef std::list<Fragment*> FragmentList;

#endif //FRAGMENT_H
