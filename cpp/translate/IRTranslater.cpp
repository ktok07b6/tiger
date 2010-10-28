#include <cassert>
#include "IRTranslater.h"
#include "Absyn.h"
#include "Level.h"
#include "debug.h"
#include "Type.h"
#include "NameEntry.h"
#include "Table.h"
#include "Ex.h"
#include "Nx.h"
#include "Cx.h"
#include "HeapManager.h"
#include "TreePrinter.h"

extern Scopes<Type> typeScopes;
extern Scopes<NameEntry> nameScopes;


IRTranslater::IRTranslater(Frame *f)
	: texp(NULL)
	, currentLevel(NULL)
	, topLevel(NULL)
	, currentFuncName(NULL)
	, currentLoopExit(NULL)
{
	FUNCLOG;
	currentLevel = topLevel = Level::newLevel(f);
}

void
IRTranslater::visit(SimpleVar *var)
{
	FUNCLOG;
	//
	DBG("var symbol.name = %s", var->sym->name.c_str());
	DBG("var->symInfo = %p", var->symInfo);
	assert(var->symInfo->access);
	texp = gcnew(translate::Ex, (var->symInfo->access->simpleVar(currentLevel)));
}

void
IRTranslater::visit(FieldVar *var)
{
	FUNCLOG;
	var->var->accept(this);
	DBG("##### %s", var->symInfo->type->toString().c_str());
	RecordT *rec_t = NULL;
	if (var->symInfo->type->isRecordT()) {
		rec_t = (RecordT*)var->symInfo->type;
	} else if (var->symInfo->type->isArrayT()) {
		Type *elem_t = ((ArrayT*)var->symInfo->type)->element;
		assert(elem_t->isRecordT());
		rec_t = (RecordT*)elem_t;
	}
	int i = 0;
	while (rec_t) {
		if (rec_t->fieldName == var->sym) {
			break;
		}
		rec_t = rec_t->tail;
		++i;
	}
	if (rec_t == NULL) {
		assert(0);
		return;
	}
	tree::Exp *head = texp->unEx();
	//var->sym->name;
	tree::CONST *offset = _CONST(i*currentLevel->getFrame()->wordSize());
	tree::BINOP *memberAddr = _(head) + _(offset);
	tree::MEM *member = _MEM(memberAddr);
	texp = gcnew(translate::Ex, (member));
}

void
IRTranslater::visit(SubscriptVar *var)
{
	FUNCLOG;
	var->var->accept(this);

	tree::Exp *head = texp->unEx();

	var->exp->accept(this);

	tree::Exp *index = texp->unEx();

	int wordSize = currentLevel->getFrame()->wordSize();
	tree::CONST *ws = _CONST(wordSize);
	//tree::BINOP *offset = gcnew(tree::BINOP, (tree::BINOP::oMUL, index, ws));
	//tree::BINOP *addr = gcnew(tree::BINOP, (tree::BINOP::oPLUS, head, offset));
	tree::BINOP *offset = _(index) * _(ws);
	tree::BINOP *addr = _(head) + _(offset);
	tree::MEM *mem = _MEM(addr);
	texp = gcnew(translate::Ex, (mem));
}

void
IRTranslater::visit(VarExp *exp)
{
	FUNCLOG;
	exp->var->accept(this);
}

void
IRTranslater::visit(NilExp *exp)
{
	FUNCLOG;
	//TODO:
	tree::CONST *i = _CONST(0);
	texp = gcnew(translate::Ex, (i));
}

void
IRTranslater::visit(IntExp *exp)
{
	FUNCLOG;
	tree::CONST *i = _CONST(exp->n);
	texp = gcnew(translate::Ex, (i));
}

void
IRTranslater::visit(StringExp *exp)
{
	FUNCLOG;

	Label *lab = gcnew(Label, ());
	tree::NAME *name = _NAME(lab);
	Frame *f = currentLevel->getFrame();
	std::string str_asm = f->string(lab, exp->str);
	fragments.push_back(gcnew(DataFragment, (str_asm)));

	texp = gcnew(translate::Ex, (name));
}

void
IRTranslater::visit(CallExp *exp)
{
	FUNCLOG;

	assert(exp->fnInfo);
	/*
	assert(exp->fnInfo->level);
	Level *lev = exp->fnInfo->level;
	lev->formals;
	*/
	tree::ExpList args;
	if (currentFuncName == exp->func) {
		//In case of the recusive call, to pass the static link as first argument.
		tree::TEMP *fp = _TEMP(currentLevel->getFrame()->fp());
		tree::Exp *sl = currentLevel->getFrame()->staticChain(fp);
		args.push_back(sl);
	} else {
		//To pass the frame pointer as first argument.
		tree::TEMP *fp = _TEMP(currentLevel->getFrame()->fp());
		args.push_back(fp);
	}

	ExpList::iterator it;
	it = exp->explist->begin();
	while (it != exp->explist->end()) {
		Exp *e = *it;
		e->accept(this);
		args.push_back(texp->unEx());
		++it;
	}

	Label *namedLabel = gcnew(Label, (exp->func->name));
	tree::NAME *func = _NAME(namedLabel);
	tree::CALL *call = _CALL(func, args);
	texp = gcnew(translate::Ex, (call));
}

void
IRTranslater::visit(OpExp *exp)
{
	FUNCLOG;

	exp->left->accept(this);
	tree::Exp *el = texp->unEx();
	exp->right->accept(this);
	tree::Exp *er = texp->unEx();

	if (PlusOp <= exp->op && exp->op <= DivideOp) {
		tree::BINOP::Oper op;

		switch (exp->op) {
		case PlusOp:
			op = tree::BINOP::oPLUS; break;
		case MinusOp:
			op = tree::BINOP::oMINUS; break;
		case TimesOp:
			op = tree::BINOP::oMUL; break;
		case DivideOp:
			op = tree::BINOP::oDIV; break;
		default:
			assert(0);
		}
		tree::BINOP *binop = _BINOP(op, el, er);
		texp = gcnew(translate::Ex, (binop));
		return;
	} else if (EqOp <= exp->op && exp->op <= GeOp) {
		tree::CJUMP::RelOp op;
		switch (exp->op) {
		case EqOp:
			op = tree::CJUMP::oEQ; break;
		case NeOp:
			op = tree::CJUMP::oNE; break;
		case LtOp:
			op = tree::CJUMP::oLT; break;
		case LeOp:
			op = tree::CJUMP::oLE; break;
		case GtOp:
			op = tree::CJUMP::oGT; break;
		case GeOp:
			op = tree::CJUMP::oGE; break;
		default:
			assert(0);
		}
		texp = gcnew(translate::Cx, (op, el, er));
		return;
	} else {
		if (exp->op == AndOp) {
			tree::Exp *e = convertAndOp(el, er);
			texp = gcnew(translate::Ex, (e));
			return;
		} else if (exp->op == OrOp) {
			tree::Exp *e = convertOrOp(el, er);
			texp = gcnew(translate::Ex, (e));
			return;
		}
	}
	assert(0);
}

tree::Exp *
IRTranslater::convertAndOp(tree::Exp *el, tree::Exp *er)
{
	/*
	  r = 0
	  if el != 0 then L1 else done
	  L1:
	  if er != 0 then L2 else done
	  L2:
	  r = 1
	  done:
	  return r
	*/			
	tree::SEQMaker sm;
	Temp *tmp = gcnew(Temp, ());
	tree::TEMP *ret = _TEMP(tmp);
	tree::CONST *const0 = _CONST(0);
	tree::CONST *const1 = _CONST(1);
	tree::MOVE *ret_init = _MOVE(ret, const0);
	
	Label *l1 = gcnew(Label, ());
	Label *l2 = gcnew(Label, ());
	Label *done = gcnew(Label, ());
	tree::LABEL *L1 = _LABEL(l1);
	tree::LABEL *L2 = _LABEL(l2);
	tree::LABEL *DONE = _LABEL(done);

	tree::CJUMP *cj1 = _CJUMP(tree::CJUMP::oNE, el, const0, l1, done);
	tree::CJUMP *cj2 = _CJUMP(tree::CJUMP::oNE, er, const0, l2, done);
	tree::MOVE *ret_update = _MOVE(ret, const1);

	sm.add(ret_init);
	sm.add(cj1);
	sm.add(L1);
	sm.add(cj2);
	sm.add(L2);
	sm.add(ret_update);
	sm.add(DONE);

	tree::SEQ *seq = sm.make();
	return _ESEQ(seq, ret);
}


tree::Exp *
IRTranslater::convertOrOp(tree::Exp *el, tree::Exp *er)
{
	/*
	  t1 = 1
	  if el = 0 then L1 else done
	  L1:
	  if er = 0 then L2 else done
	  L2:
	  t1 = 0
	  done:
	  return t1
	*/			
	tree::SEQMaker sm;
	Temp *tmp = gcnew(Temp, ());
	tree::TEMP *ret = _TEMP(tmp);
	tree::CONST *const0 = _CONST(0);
	tree::CONST *const1 = _CONST(1);
	tree::MOVE *ret_init = _MOVE(ret, const1);
	
	Label *l1 = gcnew(Label, ());
	Label *l2 = gcnew(Label, ());
	Label *done = gcnew(Label, ());
	tree::LABEL *L1 = _LABEL(l1);
	tree::LABEL *L2 = _LABEL(l2);
	tree::LABEL *DONE = _LABEL(done);

	tree::CJUMP *cj1 = _CJUMP(tree::CJUMP::oEQ, el, const0, l1, done);
	tree::CJUMP *cj2 = _CJUMP(tree::CJUMP::oEQ, er, const0, l2, done);
	tree::MOVE *ret_update = _MOVE(ret, const0);

	sm.add(ret_init);
	sm.add(cj1);
	sm.add(L1);
	sm.add(cj2);
	sm.add(L2);
	sm.add(ret_update);
	sm.add(DONE);

	tree::SEQ *seq = sm.make();
	return _ESEQ(seq, ret);
}

void
IRTranslater::visit(RecordField *field)
{
	FUNCLOG;
	//field->name->name;
	field->init->accept(this);
}

void
IRTranslater::visit(RecordExp *exp)
{
	FUNCLOG;
	//exp->type->name;
	tree::SEQMaker seq;
	//base = malloc fieldsize * wordsize
	Temp *t = gcnew(Temp, ());
	tree::TEMP *base = _TEMP(t);
	int ws = currentLevel->getFrame()->wordSize();
	tree::Exp *addr = callMalloc(exp->fields->size() * ws);
	tree::MOVE *mv = _MOVE(base, addr);

	seq.add(mv);

	RecordFieldList::iterator it;
	int i = 0;
	it = exp->fields->begin();
	while (it != exp->fields->end()) {
		RecordField *f = *it;
		f->accept(this);
		if (texp) {
			tree::Exp *e = texp->unEx();
			tree::CONST *offset = _CONST(i*ws);
			tree::BINOP *p = _(base) + _(offset);
			tree::MEM *m = _MEM(p);
			tree::MOVE *mv = _MOVE(m, e);
			seq.add(mv);
		}
		++i;
		++it;
	}
	tree::ESEQ *eseq = _ESEQ(seq.make(), base);
	texp = gcnew(translate::Ex, (eseq));
}

void
IRTranslater::visit(SeqExp *exp)
{
	FUNCLOG;
	assert(exp->seq->size() >= 2);

	tree::Exp *ret = NULL;
	tree::SEQMaker sm;
	Seq::iterator it;
	it = exp->seq->begin();
	do {
		Exp *e = *it;
		e->accept(this);		
		++it;
		
		if (it == exp->seq->end()) {
			ret = texp ? texp->unEx() : NULL;
			break;
			
		} else {
			texp ? sm.add(texp->unNx()) : (void)(0);
		}
   } while (true);

	if (ret) {
		tree::SEQ *seq = sm.make();
		tree::ESEQ *eseq = _ESEQ(seq, ret);
		texp = gcnew(translate::Ex, (eseq));
	} else {
		sm.add(texp->unNx());
		tree::SEQ *seq = sm.make();
		texp = gcnew(translate::Nx, (seq));
	}
}

void
IRTranslater::visit(AssignExp *exp)
{
	FUNCLOG;
	exp->var->accept(this);
	assert(texp);
	tree::Exp *v = texp->unEx();

	exp->exp->accept(this);
	tree::Exp *e = texp ? texp->unEx() : NULL;

	tree::MOVE *move = _MOVE(v, e);
	texp = gcnew(translate::Nx, (move));
}

void
IRTranslater::visit(IfExp *exp)
{
	FUNCLOG;

	Label *labelT = gcnew(Label, ());
	Label *labelF = gcnew(Label, ());
	tree::LABEL *l_T = _LABEL(labelT);
	tree::LABEL *l_F = _LABEL(labelF);

	exp->test->accept(this);
	tree::Exp *boolean = texp->unEx();
	tree::CONST *const_0 = _CONST(0);
	tree::CJUMP *cjump = _CJUMP(tree::CJUMP::oNE, boolean, const_0, labelT, labelF);
	if (exp->elseexp) {
		Temp *tmp = gcnew(Temp, ());
		tree::TEMP *r = _TEMP(tmp);

		exp->thenexp->accept(this);
		tree::MOVE *r_T = _MOVE(r, texp->unEx());

		exp->elseexp->accept(this);
		tree::MOVE *r_F = _MOVE(r, texp->unEx());

		Label *labelJoin = gcnew(Label, ());
		tree::LABEL *l_Join = _LABEL(labelJoin);
		tree::JUMP *jmp_to_Join = _JUMP(labelJoin); 

		tree::SEQMaker sm;
		sm.add(cjump);
		sm.add(l_T);
		sm.add(r_T);
		sm.add(jmp_to_Join);
		sm.add(l_F);
		sm.add(r_F);
		sm.add(l_Join);
		tree::SEQ *seq = sm.make();//makeSEQ(cjump, l_T, r_T, jmp_to_Join, l_F, r_F, l_Join);
		//TODO
		tree::ESEQ *eseq = _ESEQ(seq, r);
		texp = gcnew(translate::Ex, (eseq));

	} else {
		exp->thenexp->accept(this);
		tree::Stm *stm_T = texp->unNx();
		tree::SEQMaker sm;
		sm.add(cjump);
		sm.add(l_T);
		sm.add(stm_T);
		sm.add(l_F);
		tree::SEQ *seq = sm.make();//makeSEQ(cjump, l_T, stm_T, l_F);

		texp = gcnew(translate::Nx, (seq));
	}
}

void
IRTranslater::visit(WhileExp *exp)
{
	FUNCLOG;
	
	//TODO:
	exp->test->accept(this);
	tree::Exp *test = texp->unEx();

	exp->body->accept(this);
	tree::Stm *body = texp->unNx();

	/*
	  if test then jmp start
              else jmp end
start:
    body
    if test then jmp start
            else jmp end
end:
	 */
	Label *labelS = gcnew(Label, ());
	Label *labelE = gcnew(Label, ());
	tree::LABEL *l_S = _LABEL(labelS);
	tree::LABEL *l_E = _LABEL(labelE);
	tree::CONST *const0 = _CONST(0);
	tree::CJUMP *cmp = _CJUMP(tree::CJUMP::oNE, test, const0, labelS, labelE); 
	tree::SEQMaker sm;
	sm.add(cmp);
	sm.add(l_S);
	sm.add(body);
	sm.add(cmp);
	sm.add(l_E);
	tree::SEQ *seq = sm.make();//makeSEQ(cmp, l_S, body, cmp, l_E);

	texp = gcnew(translate::Nx, (seq));
}

void
IRTranslater::visit(ForExp *exp)
{
	FUNCLOG;
	//TODO:
	//LetExpへの変換
	Level::Access *access = currentLevel->allocLocal(true); //TODO: escape
	DBG("exp->symInfo = %p", exp->symInfo);
	exp->symInfo->access = access;
	tree::Exp *var = access->simpleVar(currentLevel);

	//exp->var->name;
	//exp->escape;
	exp->lo->accept(this);
	tree::Exp *lo = texp->unEx();
	exp->hi->accept(this);
	tree::Exp *hi = texp->unEx();
	exp->body->accept(this);
	tree::Stm *body = texp->unNx();
	/*
	i = lo
    if i < hi then jmp start
              else jmp end
start:
    body
    i = i+1
    if i < hi then jmp start 
               else jmp end
end:
	 */
		 /*
	Temp *tmp = gcnew(Temp, ());
	tree::TEMP *i = gcnew(tree::TEMP, (tmp));
		 */
	Label *labelS = gcnew(Label, ());
	Label *labelE = gcnew(Label, ());
	tree::LABEL *l_S = _LABEL(labelS);
	tree::LABEL *l_E = _LABEL(labelE);

	tree::MOVE *var_init = _MOVE(var, lo);
	tree::CONST *const_1 = _CONST(1);
	tree::BINOP *var_inc = _BINOP(tree::BINOP::oPLUS, var, const_1);
	tree::MOVE *var_update = _MOVE(var, var_inc);
	tree::CJUMP *cmp = _CJUMP(tree::CJUMP::oLT, var, hi, labelS, labelE); 
	tree::SEQMaker sm;
	sm.add(var_init);
	sm.add(cmp);
	sm.add(l_S);
	sm.add(body);
	sm.add(var_update);
	sm.add(cmp);
	sm.add(l_E);
	tree::SEQ *seq = sm.make();//makeSEQ(var_init, cmp, l_S, body, var_update, cmp, l_E);

	texp = gcnew(translate::Nx, (seq));
}

void
IRTranslater::visit(BreakExp *exp)
{
	FUNCLOG;
}

void
IRTranslater::visit(LetExp *exp)
{
	FUNCLOG;
	
	tree::SEQMaker sm;
	DBG("exp->decs->size() = %u", exp->decs->size());
	if (!exp->decs->empty()) {
		DecList::iterator it;
		it = exp->decs->begin();

		while (it != exp->decs->end()) {
			Dec *d = *it;
			DBG("accept dec");
			d->accept(this);

			//If d is function decl, it is not added.
			texp && !d->isFundec() ?
				sm.add(texp->unNx())
			:
				(void)(0);//DBG("texp is Nil");
			
			++it;
		}
	}
 
	DBG("accept body");
	exp->body->accept(this);

	//letも値を返す
	if (texp) {
		tree::Exp *ebody = texp->unEx();
		if (ebody) {
			tree::SEQ *seq = sm.make();
			if (seq) {
				tree::ESEQ *eseq = _ESEQ(seq, ebody);
				texp = gcnew(translate::Ex, (eseq));
			} else {
				texp = gcnew(translate::Ex, (ebody));
			}

			return;
		} else {
			sm.add(texp->unNx());
		}
	} 
	tree::SEQ *seq = sm.make();
	texp = gcnew(translate::Nx, (seq));
}

void
IRTranslater::visit(ArrayExp *exp)
{
	FUNCLOG;
	//TODO

	//exp->type->name;
	exp->size->accept(this);
	tree::Exp *size = texp->unEx();

	exp->init->accept(this);
	tree::Exp *init = texp->unEx();

	tree::SEQMaker seq;
	Temp *t = gcnew(Temp, ());
	tree::TEMP *base = _TEMP(t);

	tree::ExpList arg;
	arg.push_back(size);
	arg.push_back(init);
	tree::Exp *addr = currentLevel->getFrame()->externalCall("initArray", arg);
	tree::MOVE *mv = _MOVE(base, addr);
	seq.add(mv);
	tree::ESEQ *eseq = _ESEQ(seq.make(), base);
	texp = gcnew(translate::Ex, (eseq));
}

void
IRTranslater::visit(TypeField *field)
{
	FUNCLOG;
	//TODO
	//field->name->name;
	//field->escape;
	//field->type->name;

	//tree::Exp *arg = field->symInfo->access->simpleVar(currentLevel);
	texp = NULL;
}

void
IRTranslater::visit(FunDec *dec)
{
	FUNCLOG;
	//dec->name;
	//dec->params;
	//dec->result;

	std::vector<int> formals;
	//static chainのため1つ余分に引数を割り当てる
	for (unsigned int i = 0; i < dec->params->size()+1; ++i) {
		formals.push_back(1);//TODO:
	}

	currentLevel = Level::newLevel(currentLevel, dec->name, true, formals);

	dec->fnInfo->level = currentLevel;
	//dec->name->name;
	//dec->result->name;
	tree::SEQMaker sm;
	TypeFieldList::iterator it;
	it = dec->params->begin();
	std::vector<Level::Access*>::iterator it2;
	it2 = currentLevel->formals.begin();
	it2++;//skip static chain
	while (it != dec->params->end()) {
		TypeField *f = *it;
		f->symInfo->access = *it2;
		//f->accept(this);
		//texp ? sm.add(texp->unNx()) : (void)(0);
		++it;
		++it2;
	}

	Symbol *parentFuncName = currentFuncName;
	currentFuncName = dec->name;

	dec->body->accept(this);

	currentFuncName = parentFuncName;

	if (texp) {
		tree::Exp *e = texp->unEx();
		if (e) {
			//function
			tree::TEMP *rv = _TEMP(currentLevel->getFrame()->rv());
			tree::MOVE *mv_return_value = _MOVE(rv, e); 
			sm.add(mv_return_value);
		} else {
			//procedure
			sm.add(texp->unNx());
		}
	}
	tree::Stm *body = currentLevel->getFrame()->procEntryExit1(sm.make());
	texp = gcnew(translate::Nx, (body));
	procEntryExit(texp);
}

void
IRTranslater::visit(VarDec *dec)
{
	FUNCLOG;
	Level::Access *access = currentLevel->allocLocal(true); //TODO: escape
	DBG("dec->symInfo = %p", dec->symInfo);
	dec->symInfo->access = access;

	tree::Exp *var = access->simpleVar(currentLevel);
	//dec->name->name;
	//dec->escape;
	//dec->type->name;
	dec->init->accept(this);

	tree::Exp *init;
	init = texp ? texp->unEx() : NULL;

	tree::MOVE *move = _MOVE(var, init);
	texp = gcnew(translate::Nx, (move));
}

void
IRTranslater::visit(TypeDec *dec)
{
	FUNCLOG;
	//dec->name->name;
	dec->ty->accept(this);
}

void
IRTranslater::visit(NameTy *ty)
{
	FUNCLOG;
	//ty->name->name;
}

void
IRTranslater::visit(RecordTy *ty)
{
	FUNCLOG;
	TypeFieldList::iterator it;
	it = ty->fields->begin();
	while (it != ty->fields->end()) {
		TypeField *f = *it;
		f->accept(this);
		++it;
	}
}

void
IRTranslater::visit(ArrayTy *ty)
{
	FUNCLOG;
	//ty->name->name;
}

tree::Exp *
IRTranslater::callMalloc(int size)
{
	tree::CONST *csize = _CONST(size);
	tree::ExpList arg;
	arg.push_back(csize);
	tree::Exp *cmalloc = currentLevel->getFrame()->externalCall("malloc", arg);
	return cmalloc;
}

void
IRTranslater::procEntryExit(translate::Exp *exp)
{
	tree::Stm *stm = exp->unNx();
	ProcFragment *fragment = gcnew(ProcFragment, (stm, currentLevel->getFrame()));
	fragments.push_back(fragment);
	currentLevel = currentLevel->parent;	
}

translate::Exp *
IRTranslater::getExp()
{
	return texp;
}
const FragmentList &
IRTranslater::getFragments()
{
	procEntryExit(texp);
	assert(currentLevel == NULL);

	return fragments;
}

