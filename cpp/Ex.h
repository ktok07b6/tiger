#ifndef EX_H
#define EX_H

#include "Exp.h"

namespace translate{
class Ex : public Exp
{
 public:
	Ex(tree::Exp *e);
	virtual tree::Exp *unEx();
	virtual tree::Stm *unNx();
	virtual tree::CJUMP *unCx(Label *t, Label *f);
	tree::Exp *exp;
};

}//namespace translate

#endif //EX_H
