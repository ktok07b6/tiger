#ifndef EXP_H
#define EXP_H

#include "Object.h"

class Label;
namespace tree {
	class Exp;
	class Stm;
	class CJUMP;
}

namespace translate {
class Exp : public Object
{
 public:
	virtual ~Exp() {}
	virtual tree::Exp *unEx() = 0;
	virtual tree::Stm *unNx() = 0;
	virtual tree::CJUMP *unCx(Label *t, Label *f) = 0;
};

}//namespace translate

#endif //EXP_H

