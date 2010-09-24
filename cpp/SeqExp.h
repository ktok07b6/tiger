#ifndef SEQ_EXP_H
#define SEQ_EXP_H

#include "Exp.h"
#include "Tree.h"

namespace translate{
class SeqExp : public Exp
{
 public:
	SeqExp(tree::SEQ *);
	virtual tree::Exp *unEx();
	virtual tree::Stm *unNx();
	virtual tree::CJUMP *unCx(Label *t, Label *f);

 private:
	tree::Stm *getLastStm();
	tree::SEQ *seq;
};

}//namespace translate

#endif //EX_H
