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
	VDBG("var symbol.name = %s", var->sym->name.c_str());
	VDBG("var->symInfo = %p", var->symInfo);
	assert(var->symInfo->access);
	texp = gcnew(translate::Ex, (var->symInfo->access->simpleVar(currentLevel)));
}

void
IRTranslater::visit(FieldVar *var)
{
	FUNCLOG;
	var->var->accept(this);
	VDBG("##### %s", var->symInfo->type->toString().c_str());
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
	if (index->isCONST_T()) {
		int idx = ((tree::CONST*)index)->value;
		tree::CONST *offset = _CONST(idx*wordSize);
		tree::BINOP *addr = _(head) + _(offset);
		tree::MEM *mem = _MEM(addr);
		texp = gcnew(translate::Ex, (mem));
	} else {
		tree::CONST *ws = _CONST(wordSize);
		tree::BINOP *offset = _(index) * _(ws);
		tree::BINOP *addr = _(head) + _(offset);
		tree::MEM *mem = _MEM(addr);
		texp = gcnew(translate::Ex, (mem));
	}
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
	tree::SEQMaker sm;
	Frame *f = currentLevel->getFrame();
	//allocate memory for "string" structure
	/*
	  struct string 
	  {
	  int length; 
	  unsigned char chars[1];
	  };

	  string *pstr = malloc(wordSize + strSize);
	  pstr->length = strSize;
	*/
	int len_field_size = currentLevel->getFrame()->wordSize();
	int string_field_size = exp->str.size();
	Temp *t = gcnew(Temp, ());
	tree::Exp *alloc = callAlloc(len_field_size + string_field_size);
	tree::TEMP *pstr = _TEMP(t);
	tree::MOVE *mv = _MOVE(pstr, alloc);
	sm.add(mv);

	tree::MEM *mem = _MEM(pstr);
	tree::CONST *len = _CONST(string_field_size);
	tree::MOVE *copy_str_len = _MOVE(mem, len);
	sm.add(copy_str_len);

	/*
	  TODO: move strcpy to Frame impl
	  strcopy(pstr->chars, [address of string], strSize);
	 */
	Label *lab = gcnew(Label, ());
	tree::NAME *name = _NAME(lab);

	tree::CONST *four = _CONST(4);
	tree::BINOP *pstr_plus_4 = _(pstr) + _(four);
	tree::ExpList arg;
	arg.push_back(pstr_plus_4);
	arg.push_back(name);
	tree::Exp *str_copy = f->externalCall("strcpy", arg);
	tree::EXPR *stm_str_copy = _EXPR(str_copy);
	sm.add(stm_str_copy);

	//add DataFragment
	std::string str_asm = f->string(lab, exp->str);
	fragments.push_back(gcnew(DataFragment, (str_asm)));

	tree::ESEQ *eseq = _ESEQ(sm.make(), pstr);
	texp = gcnew(translate::Ex, (eseq));
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
	//FIXME
	if (currentFuncName == exp->func) {
		//In case of the recusive call, to pass the static link as first argument.
		tree::TEMP *fp = _TEMP(currentLevel->getFrame()->fp());
		tree::Exp *sl = currentLevel->getFrame()->staticChain(fp);
		args.push_back(sl);
	} else {
		if (!isBuiltinFunc(exp->func)) {
			//To pass the frame pointer as first argument.
			tree::TEMP *fp = _TEMP(currentLevel->getFrame()->fp());
			args.push_back(fp);
		}
	}
	

	ExpList::iterator it;
	it = exp->explist->begin();
	while (it != exp->explist->end()) {
		Exp *e = *it;
		e->accept(this);
		tree::Exp *arg = texp->unEx();
		assert(arg);
		args.push_back(arg);
		++it;
	}

	Label *namedLabel = gcnew(Label, (exp->func->name));
	tree::NAME *func = _NAME(namedLabel);
	tree::CALL *call = _CALL(func, args);
	if (exp->fnInfo->result->isVoidT()) {
		tree::EXPR *expr = _EXPR(call);
		texp = gcnew(translate::Nx, (expr));
	} else {
		texp = gcnew(translate::Ex, (call));
	}
}

void
IRTranslater::visit(OpExp *exp)
{
	FUNCLOG;

	exp->left->accept(this);
	tree::Exp *el = texp->unEx();
	exp->right->accept(this);
	tree::Exp *er = texp->unEx();
	if (exp->l_t->actual()->isStrT()) {
		tree::Exp *e = convertStrOp(exp->op, el, er);
		texp = gcnew(translate::Ex, (e));
		return;
	}
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
IRTranslater::convertStrOp(int op, tree::Exp *el, tree::Exp *er)
{
	tree::ExpList args;
	args.push_back(el);
	args.push_back(er);
	if (op == PlusOp) {
		return currentLevel->getFrame()->externalCall("stringConcat", args);
	} else if (op == EqOp) {
		return currentLevel->getFrame()->externalCall("stringEqual", args);
	} else if (op == NeOp) {
		tree::Exp *streq = currentLevel->getFrame()->externalCall("stringEqual", args);
		args.clear();
		args.push_back(streq);
		return currentLevel->getFrame()->externalCall("not", args);
	}
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
	tree::Exp *addr = callAlloc(exp->fields->size() * ws);
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

	tree::CJUMP *cjump;
	if (texp->is(translate::Exp::CX)) {
		cjump = texp->unCx(labelT, labelF);
	} else {
		assert(texp->is(translate::Exp::EX));
		tree::Exp *boolean = texp->unEx();
		tree::CONST *const_0 = _CONST(0);
		cjump = _CJUMP(tree::CJUMP::oNE, boolean, const_0, labelT, labelF);
	}
	if (exp->elseexp) {
		Temp *r = gcnew(Temp, ());
		tree::Stm *r_T;
		tree::Stm *r_F;

		exp->thenexp->accept(this);
		tree::Exp *thenexp = texp->unEx();
	    if (thenexp) {
			r_T = _MOVE(_TEMP(r), thenexp);
		} else {
			r_T = texp->unNx();
		}

		exp->elseexp->accept(this);
		tree::Exp *elseexp = texp->unEx();
	    if (elseexp) {
			r_F = _MOVE(_TEMP(r), elseexp);
		} else {
			r_F = texp->unNx();
		}

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
		tree::ESEQ *eseq = _ESEQ(seq, _TEMP(r));
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
	
	exp->test->accept(this);

	tree::CJUMP *cjump;
	if (texp->is(translate::Exp::CX)) {
		cjump = texp->unCx(labelS, labelE);
	} else {
		assert(texp->is(translate::Exp::EX));
		tree::Exp *boolean = texp->unEx();
		tree::CONST *const_0 = _CONST(0);
		cjump = _CJUMP(tree::CJUMP::oNE, boolean, const_0, labelS, labelE);
	}

	Label *oldLoopExit = NULL;
	oldLoopExit = currentLoopExit;
	currentLoopExit = labelE;
	
	exp->body->accept(this);
	tree::Stm *body = texp->unNx();

	currentLoopExit = oldLoopExit;

	tree::LABEL *l_S = _LABEL(labelS);
	tree::LABEL *l_E = _LABEL(labelE);
	tree::SEQMaker sm;
	sm.add(cjump);
	sm.add(l_S);
	sm.add(body);
	sm.add(cjump);
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

	Level::Access *access = currentLevel->allocLocal(exp->escape);
	VDBG("exp->symInfo = %p", exp->symInfo);
	exp->symInfo->access = access;
	tree::Exp *var = access->simpleVar(currentLevel);
	Label *labelS = gcnew(Label, ());
	Label *labelE = gcnew(Label, ());

	exp->lo->accept(this);
	tree::Exp *lo = texp->unEx();
	exp->hi->accept(this);
	tree::Exp *hi = texp->unEx();

	currentLoopExit = labelE;

	exp->body->accept(this);
	tree::Stm *body = texp->unNx();

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
	tree::JUMP *jmp = _JUMP(currentLoopExit);
	texp = gcnew(translate::Nx, (jmp));
}

#include "TreePrinter.h"
void
IRTranslater::visit(LetExp *exp)
{
	FUNCLOG;
	
	tree::SEQMaker sm;
	VDBG("exp->decs->size() = %u", exp->decs->size());
	if (!exp->decs->empty()) {
		DecList::iterator it;
		it = exp->decs->begin();

		while (it != exp->decs->end()) {
			Dec *d = *it;
			d->accept(this);

			//If d is function decl, it is not added.
			texp && !d->isFundec() ?
				sm.add(texp->unNx())
			:
				(void)(0);//DBG("texp is Nil");
			
			++it;
		}
	}
 
	exp->body->accept(this);
	DBG("$$$");
	tree::TreePrinter::printTree(texp->unNx());

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
/*
void
IRTranslater::initArray
*/
void
IRTranslater::visit(ArrayExp *exp)
{
	FUNCLOG;

	int wordSize = currentLevel->getFrame()->wordSize();

	//exp->type->name;
	exp->size->accept(this);
	tree::Exp *size = texp->unEx();

	if (exp->init_t->actual()->isIntT()) {
		exp->init->accept(this);
		tree::Exp *init = texp->unEx();

		tree::ExpList arg;
		arg.push_back(size);
		arg.push_back(init);
		tree::Exp *addr = currentLevel->getFrame()->externalCall("initArray", arg);
		texp = gcnew(translate::Ex, (addr));
	} else if (exp->init_t->actual()->isStrT() ||
			   exp->init_t->actual()->isArrayT() ||
			   exp->init_t->actual()->isRecordT()) {
		exp->init->accept(this);
		tree::Exp *init = texp->unEx();

		tree::ExpList arg;
		tree::BINOP *memsz = _(size) * _(_CONST(wordSize));
		arg.push_back(memsz);
		tree::Exp *addr = currentLevel->getFrame()->externalCall("alloc", arg);

		tree::TEMP *ptr = _TEMP(gcnew(Temp,()));
		tree::MOVE *ptr_init = _MOVE(ptr, addr);
		tree::TEMP *var = _TEMP(gcnew(Temp,()));
		Label *labelS = gcnew(Label, ());
		Label *labelE = gcnew(Label, ());
		tree::LABEL *l_S = _LABEL(labelS);
		tree::LABEL *l_E = _LABEL(labelE);
		tree::MOVE *var_init = _MOVE(var, _CONST(0));
		tree::BINOP *offset = _(var) * _(_CONST(wordSize));
		tree::MEM *mem = _MEM(_(ptr)+_(offset));
		tree::MOVE *store = _MOVE(mem, init);
		tree::BINOP *var_inc = _(var) + _(_CONST(1));
		tree::MOVE *var_update = _MOVE(var, var_inc);
		tree::CJUMP *cmp = _CJUMP(tree::CJUMP::oLT, var, size, labelS, labelE); 
		tree::SEQMaker sm;
		sm.add(ptr_init);
		sm.add(var_init);
		sm.add(cmp);
		sm.add(l_S);
		sm.add(store);
		sm.add(var_update);
		sm.add(cmp);
		sm.add(l_E);
		tree::SEQ *seq = sm.make();
		tree::ESEQ *eseq = _ESEQ(seq, ptr);
		texp = gcnew(translate::Ex, (eseq));
	} else {
		assert(0);
	}
}

void
IRTranslater::visit(TypeField *field)
{
	FUNCLOG;
	assert(0);
}

void
IRTranslater::visit(FunDec *dec)
{
	FUNCLOG;

	std::vector<int> formals;
	//static chainのため1つ余分に引数を割り当てる
	//FIXME
	formals.push_back(1);//always escape
	
	TypeFieldList::iterator it;
	it = dec->params->begin();
	while (it != dec->params->end()) {
		TypeField *f = *it;
		VDBG("typefield %s escape %d", (const char*)f->name, f->escape);
		formals.push_back(f->escape);
		++it;
	}
	currentLevel = Level::newLevel(currentLevel, dec->name, true, formals);

	dec->fnInfo->level = currentLevel;
	//dec->name->name;
	//dec->result->name;
	tree::SEQMaker sm;
	it = dec->params->begin();
	std::vector<Level::Access*>::iterator it2;
	it2 = currentLevel->formals.begin();
	//FIXME:
	it2++;//skip static chain
	while (it != dec->params->end()) {
		TypeField *f = *it;
		Level::Access *accArg = *it2;
		f->symInfo->access = accArg;
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
			//TODO: To find more simple way!
			Temp *rv = currentLevel->getFrame()->rv();
			if (e->isESEQ_T()) {
				tree::ESEQ *eseq = (tree::ESEQ*)e;
				while (eseq->exp->isESEQ_T()) {
					eseq = (tree::ESEQ*)eseq->exp;
				}
				tree::MOVE *mv_return_value = _MOVE(_TEMP(rv), eseq->exp);
				sm.add(eseq->stm);
				sm.add(mv_return_value);
			} else {
				tree::MOVE *mv_return_value = _MOVE(_TEMP(rv), e); 
				sm.add(mv_return_value);
			}

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
	Level::Access *access = currentLevel->allocLocal(dec->escape);
	VDBG("dec->symInfo = %p", dec->symInfo);
	dec->symInfo->access = access;

	tree::Exp *var = access->simpleVar(currentLevel);

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
	texp = NULL;
}

void
IRTranslater::visit(NameTy *ty)
{
	FUNCLOG;
	assert(0);
}

void
IRTranslater::visit(RecordTy *ty)
{
	FUNCLOG;
	assert(0);
}

void
IRTranslater::visit(ArrayTy *ty)
{
	FUNCLOG;
	assert(0);
}

tree::Exp *
IRTranslater::callAlloc(int size)
{
	tree::CONST *csize = _CONST(size);
	tree::ExpList arg;
	arg.push_back(csize);
	tree::Exp *calloc = currentLevel->getFrame()->externalCall("alloc", arg);
	return calloc;
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
	tree::Stm *stm = texp->unNx();
	tree::Stm *global_body = currentLevel->getFrame()->procEntryExit1(stm);
	texp = gcnew(translate::Nx, (global_body));
	procEntryExit(texp);
	assert(currentLevel == NULL);

	return fragments;
}

bool
IRTranslater::isBuiltinFunc(Symbol *f)
{
	return //(Symbol::symbol("initrray") == f) ||
		//(Symbol::symbol("alloc") == f) ||
		(Symbol::symbol("print") == f) ||
		(Symbol::symbol("flush") == f) ||
		(Symbol::symbol("ord") == f) ||
		(Symbol::symbol("chr") == f) ||
		(Symbol::symbol("stringEqual") == f) ||
		(Symbol::symbol("stringLen") == f) ||
		(Symbol::symbol("substring") == f) ||
		(Symbol::symbol("stringConcat") == f) ||
		(Symbol::symbol("not") == f) ||
		(Symbol::symbol("getch") == f) ||
		(Symbol::symbol("test") == f);
}
