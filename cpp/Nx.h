#ifndef NX_H
#define NX_H

#include "Exp.h"

namespace translate{
class Nx : public Exp
{
 public:
	Nx(tree::Stm *s);
	virtual tree::Exp *unEx();
	virtual tree::Stm *unNx();
	virtual tree::CJUMP *unCx(Label *t, Label *f);
	tree::Stm *stm;
};

}//namespace translate

#endif //NX_H
