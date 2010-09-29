#ifndef CX_H
#define CX_H

#include "Exp.h"
#include "Tree.h"

namespace translate{
class Cx : public Exp
{
 public:
	Cx(tree::CJUMP::RelOp o, tree::Exp *l, tree::Exp *r);
	virtual tree::Exp *unEx();
	virtual tree::Stm *unNx();
	virtual tree::CJUMP *unCx(Label *t, Label *f);

 private:
	tree::CJUMP::RelOp op;
	tree::Exp *left;
	tree::Exp *right;
};

}//namespace translate

#endif //CX_H

