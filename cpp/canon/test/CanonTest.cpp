#include "Canon.h"
#include "Tree.h"
#include "TreePrinter.h"
#include "Temp.h"
#include "TreeMatcher.h"

#define NEWTEMP gcnew(Temp, ())
void test1();

void test_move1();
void test_move2();
void test_move3();
void test_move4();
int main(int argc, char **argv)
{
	test_move1();
	test_move2();
	test_move3();
	test_move4();
	
	//test1();
	return 0;
}

void test_move1()
{
	DBG("test_move1 ... (e1 := e2)");
	Canon canon;
	tree::StmList slist;
	{
		tree::TEMP *e1 = _TEMP(NEWTEMP);
		tree::TEMP *e2 = _TEMP(NEWTEMP);
		tree::MOVE *mv = _MOVE(e1, e2);
		
		tree::TreePrinter::printTree(mv);
		
		slist = canon.linearize(mv);
		DBG("::::::::::::::::::::::::::::::");
		tree::TreePrinter::printStmList(slist);
	}
	tree::Stm *result_stm;
	result_stm = slist.pop_front();
	tree::MOVE *mv;
	tree::MOVE *e1, *e2;
	assert(_M2(MOVE_T, mv, _M0(TEMP_T, e1), _M0(TEMP_T, e2)) == result_stm);
}

void test_move2()
{
	DBG("test_move2 ... (e1 := e2_1 + e2_2)");
	Canon canon;
	tree::StmList slist;
	{
		tree::TEMP *e1 = _TEMP(NEWTEMP);
		tree::BINOP *e2 = _BINOP(tree::BINOP::oPLUS, _TEMP(NEWTEMP), _TEMP(NEWTEMP) );
		tree::MOVE *mv = _MOVE(e1, e2);
		
		tree::TreePrinter::printTree(mv);
		
		slist = canon.linearize(mv);
		DBG("::::::::::::::::::::::::::::::");
		tree::TreePrinter::printStmList(slist);
	}
	tree::Stm *result_stm;
	result_stm = slist.pop_front();
	tree::MOVE *mv;
	tree::MOVE *e1, *e2, *e2_1, *e2_2;
	assert(_M2(MOVE_T, mv, 
			   _M0(TEMP_T, e1), 
			   _M2(BINOP_T, e2, 
				   _M0(TEMP_T, e2_1), 
				   _M0(TEMP_T, e2_2))) == result_stm);
}

void test_move3()
{
	DBG("test_move3 ... (e1 := (e2_1; e2_2))");
	Canon canon;
	tree::StmList slist;
	{
		tree::TEMP *e1 = _TEMP(NEWTEMP);
		tree::EXPR *e2_1 = _EXPR(_TEMP(NEWTEMP));
		tree::ESEQ *e2 = _ESEQ(e2_1, _TEMP(NEWTEMP));
		tree::MOVE *mv = _MOVE(e1, e2);
		
		tree::TreePrinter::printTree(mv);
		
		slist = canon.linearize(mv);
		DBG("::::::::::::::::::::::::::::::");
		tree::TreePrinter::printStmList(slist);
	}
	tree::Stm *result_stm;
	result_stm = slist.pop_front();
	tree::EXPR *e2_1;
	tree::TEMP *t;
	assert(_M1(EXPR_T, e2_1, _M0(TEMP_T, t)) == result_stm);

	result_stm = slist.pop_front();
	tree::MOVE *mv;
	assert(_M2(MOVE_T, mv, _M0(TEMP_T, t), _M0(TEMP_T, t)) == result_stm);
}

void test_move4()
{
	DBG("test_move4 ... (e1 := (e2 := e2+1; e2))");
	Canon canon;
	tree::StmList slist;
	{
		tree::TEMP *e1 = _TEMP(NEWTEMP);
		tree::TEMP *e2 = _TEMP(NEWTEMP);
		tree::MOVE *mv2 = _MOVE(e2, _BINOP(tree::BINOP::oPLUS, e2, _CONST(1)));
		tree::ESEQ *eseq = _ESEQ(mv2, e2);
		tree::MOVE *mv = _MOVE(e1, eseq);
		
		tree::TreePrinter::printTree(mv);
		
		slist = canon.linearize(mv);
		DBG("::::::::::::::::::::::::::::::");
		tree::TreePrinter::printStmList(slist);
	}
	tree::Stm *result_stm;
	result_stm = slist.pop_front();
	tree::MOVE *mv2;
	tree::TEMP *t;
	tree::BINOP *binop;
	assert(_M2(MOVE_T, mv2, _M0(TEMP_T, t), _M0(BINOP_T, binop)) == result_stm);

	result_stm = slist.pop_front();
	tree::MOVE *mv;
	assert(_M2(MOVE_T, mv, _M0(TEMP_T, t), _M0(TEMP_T, t)) == result_stm);
}

void test1()
{
	DBG("test1 ===== (r1 := e1 + e2)");
	Canon canon;
	
	tree::TEMP *t1 = _TEMP(gcnew(Temp, ()));
	tree::TEMP *t2 = _TEMP(gcnew(Temp, ()));
	tree::TEMP *t3 = _TEMP(gcnew(Temp, ()));
	tree::MOVE *s2 = _MOVE(t1, t2);
	tree::MOVE *s1 = _MOVE(t1, t2);
	
	tree::ESEQ *eseq2 = _ESEQ(s2, t1);
	tree::ESEQ *eseq1 = _ESEQ(s1, eseq2);

	tree::Stm *stm = _EXPR(eseq1);
	tree::TreePrinter::printTree(stm);

	tree::StmList slist;
	slist = canon.linearize(stm);
	tree::TreePrinter::printStmList(slist);
}
