package tiger.canon

import tiger.tree._
import tiger.common._

object Canon {
	def linearize(s:TreeStm):List[TreeStm] = {
		val stm = do_stm(s)
		//tree::TreePrinter::printTree(stm);
		def linear(s:TreeStm, stms:List[TreeStm]):List[TreeStm] = s match {
			case TreeSeq(l, r) => linear(l, linear(r, stms))
			case _ => s :: stms
		}
		linear(stm, List())
	}

	private def isNop(s:TreeStm):Boolean = {
		//it has no side effects
		s match {
			case expr:TreeExpr => expr.exp.isInstanceOf[TreeConst]
			case _ => false
		}
	}

	private def seq(s1:TreeStm, s2:TreeStm):TreeStm = {
		//remove a no side effect statement
		if (isNop(s1)) return s2
		if (isNop(s2)) return s1
		TreeSeq(s1, s2)
	}

	private def commute(s:TreeStm, e:TreeExp):Boolean = {
		return isNop(s) || 
			e.isInstanceOf[TreeName] || 
			e.isInstanceOf[TreeConst]
	}

	private def do_stm(s:TreeSeq):TreeStm = {
		seq(do_stm(s.l), do_stm(s.r))
	}

	private def do_stm(mov:TreeMove):TreeStm = (mov.dst, mov.src) match {
		case (temp:TreeTemp, call:TreeCall) => {
			reorder_stm(MoveCall(temp, call))
		}
		case (temp:TreeTemp, binop:TreeBinOp) => {
			reorder_stm(MoveBinOp(temp, binop))
		}
		case (eseq:TreeEseq, _) => {
			//remove ESEQ
			val mov2 = TreeMove(eseq.exp, mov.src)
			val seq = TreeSeq(eseq.stm, mov2)
				do_stm(seq)
		}
		case _ => reorder_stm(mov)
	}

	private def do_stm(expr:TreeExpr):TreeStm = expr.exp match {
		case call:TreeCall => reorder_stm(ExpCall(call))
		case _ => reorder_stm(expr)
	}

	private def do_stm(stm:TreeStm):TreeStm = stm match {
		case seq:TreeSeq => do_stm(seq) 
		case mov:TreeMove => do_stm(mov)
		case expr:TreeExpr => do_stm(expr)
		case _ => reorder_stm(stm)
	}

	private def reorder_stm(stm:TreeStm):TreeStm = {
		println("reorder_stm: " + stm)
		val (stm2, exps) = reorder(Tree.kids(stm))
		seq(stm2, Tree.buildStm(stm, exps))
	}

	private def do_exp(eseq:TreeEseq):TreeEseq = {
		val stm = do_stm(eseq.stm)
		val b = do_exp(eseq.exp)
		return TreeEseq(seq(stm, b.stm), b.exp)
	}

	private def do_exp(exp:TreeExp):TreeEseq = exp match {
		case eseq:TreeEseq => do_exp(eseq)
		case _ => reorder_exp(exp)
	}

	private def reorder_exp(exp:TreeExp):TreeEseq = {
		val (stm, exps) = reorder(Tree.kids(exp))
		TreeEseq(stm, Tree.buildExp(exp, exps))
	}

	private def reorder(kids:List[TreeExp]):Pair[TreeStm, List[TreeExp]] = {
		if (kids.isEmpty) {
			return (TreeExpr(0), List())
		}

		//DBG("reorder kids ------------------ begin");
		//tree::TreePrinter::printExpList(kids);
		//DBG("reorder kids ------------------ end");

		//FIXME:
		//Because div BINOP becomes a function call in ARM, 
		//BINOP is put out ahead of the expression. 
		val a = kids.head match {
			case TreeCall(_,_) | TreeBinOp(_,_,_) => {
				//replace CALL
				val tmp = TreeTemp(new Temp())
				val mv = TreeMove(tmp, kids.head)
				TreeEseq(mv, tmp)
			}
			case _ => kids.head
		}

		{
			val a_eseq = do_exp(a)
			val (stm, exps) = reorder(kids.tail)
			if (commute(stm, a_eseq.exp)) {
				(seq(a_eseq.stm, stm), a_eseq.exp::exps)
			} else {
				val tmp = TreeTemp(new Temp())
				val mv = TreeMove(tmp, a_eseq.exp)
				(seq(a_eseq.stm, seq(a_eseq, stm)), tmp::exps)
			}
		}
	}


}
