#include "BasicBlocks.h"
   
BasicBlocks::BasicBlocks(tree::StmList stms) 
{
	done = gcnew(Label, ());
	mkBlocks(stms);
}


//BasicBlockの中の処理
void 
BasicBlocks::doStms(tree::StmList l) 
{
	if (l.empty()) {
		//BasicBlockの最後はdoneラベルへのジャンプ
		//doneは関数のepilogの開始を表す
		block.push_back(_JUMP(done));
		blocks.push_back(block);
		return;
	}
	tree::Stm *s = l.front();
	if (s->isJUMP_T() || s->isCJUMP_T()) {
		//ジャンプがくればBasicBlockを区切る
		l.pop_front();
		block.push_back(s);
		blocks.push_back(block);
		mkBlocks(l);
	} 
	else if (s->isLABEL_T()) {
		//ラベルがくればそのラベルへのジャンプを挿入し再帰処理
		tree::StmList st;
		st = l;
		st.push_front(_JUMP(((tree::LABEL*)s)->label));
		doStms(st);
	}
	else {
		l.pop_front();
		block.push_back(s);
		doStms(l);
	}
}

void 
BasicBlocks::mkBlocks(tree::StmList l) 
{
	if (l.empty()) { 
		return;
	}
	tree::Stm *s = l.front();
	if (s->isLABEL_T()) {
		//ラベルがくればBasicBlockの開始
		l.pop_front();
		block.clear();
		block.push_back(s);
		doStms(l);
	}
	else {
		//ラベルでなければ新規ラベルを挿入し再帰処理
		tree::StmList s;
		s = l;
		s.push_front(_LABEL(gcnew(Label, ())));
		mkBlocks(s);
	}
}
