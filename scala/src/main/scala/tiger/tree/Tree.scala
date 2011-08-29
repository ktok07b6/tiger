package tiger.tree

import tiger.common._

object Tree {
	implicit def exp2stm(e:TreeExp):TreeStm = TreeExpr(e)
	implicit def int2tree(i:Int):TreeConst = TreeConst(i)
	implicit def L2TreeLabel(l:L):TreeLabel = TreeLabel(l.l)

	def kids(tree:Tree):List[TreeExp] = tree match {
		case TreeBinOp(op, l, r) => List(l,r)
		case TreeCall(func ,args) => func::args
		case TreeConst(n) => List()
		case TreeEseq(seq, exp) => error("do not reach here"); List()
		case TreeMem(exp) => List(exp)
		case TreeName(label) => List()
		case TreeTemp(temp) => List()
		case TreeExpr(exp) => List(exp)
		case TreeCjump(op, l, r, t, f) => List(l, r)
		case TreeJump(e, targets) => List(e)
		case TreeLabel(label) => List()
		case TreeMove(TreeMem(exp), src) => List(exp, src)
		case TreeMove(dst, src) => List(src)
		case TreeSeq(l, r) => error("do not reach here"); List()
		case TreeNil() => List()

		case MoveCall(tmp, call) => kids(call)
		case MoveBinOp(tmp, binop) => kids(binop)
		case ExpCall(call) => kids(call)

		case _ => error("do not reach here")
	}

	def buildExp(tree:TreeExp, exps:List[TreeExp]):TreeExp = (tree, exps) match {
		case (TreeBinOp(op, l,r), l2::r2::tail) => TreeBinOp(op, l2, r2)
		case (TreeCall(func, args), f2::args2) => TreeCall(f2.asInstanceOf[TreeName], args2)
		case (TreeConst(n), _) => tree
		case (TreeEseq(seq, exp), _) => error("do not reach here"); tree
		case (TreeMem(exp), exp2::tail) => TreeMem(exp2)
		case (TreeName(label), _) => tree
		case (TreeTemp(temp), _) => tree
		case _ => error("do not reach here")
	}

	def buildStm(tree:TreeStm, exps:List[TreeExp]):TreeStm = (tree, exps) match {
		case (TreeExpr(exp), exp2::tail) => TreeExpr(exp2)
		case (TreeCjump(op, l, r, t, f), l2::r2::tail) => TreeCjump(op, l2, r2, t, f)
		case (TreeJump(e, targets), e2::tail) => TreeJump(e2, targets)
		case (TreeLabel(label), _) => tree
		case (TreeMove(TreeMem(exp), src), d2::s2::tail) => TreeMove(TreeMem(d2), s2) 
		case (TreeMove(dst, src), s2::tail) => TreeMove(dst, s2)
		case (TreeSeq(l, r), _) => error("do not reach here"); tree
		case (TreeNil(), _) => tree

		case (MoveCall(tmp, call), exps) => TreeMove(tmp, buildExp(call, exps))
		case (MoveBinOp(tmp, binop), exps) => TreeMove(tmp, buildExp(binop, exps))
		case (ExpCall(call), exps) => TreeExpr(buildExp(call, exps))

		case _ => error("do not reach here")
	}
}

abstract class Tree
abstract class TreeExp extends Tree {
	def +(rhs:TreeExp):TreeExp = {
		TreeBinOp(Oper.Plus, this, rhs)
	} 

	def *(rhs:TreeExp):TreeExp = {
		TreeBinOp(Oper.Times, this, rhs)
	}

	def :=(rhs:TreeExp):TreeMove = {
		TreeMove(this, rhs)
	} 

	def ==(rhs:TreeExp):CjumpC = {
		CjumpC(Oper.Eq, this, rhs)
	}
	def !=(rhs:TreeExp):CjumpC = {
		CjumpC(Oper.Ne, this, rhs)
	}
	def <(rhs:TreeExp):CjumpC = {
		CjumpC(Oper.Lt, this, rhs)
	}
	def >(rhs:TreeExp):CjumpC = {
		CjumpC(Oper.Gt, this, rhs)
	}
	def <=(rhs:TreeExp):CjumpC = {
		CjumpC(Oper.Le, this, rhs)
	}
	def >=(rhs:TreeExp):CjumpC = {
		CjumpC(Oper.Ge, this, rhs)
	}
}
abstract class TreeStm extends Tree
case class TreeBinOp(op:Oper.Value, l:TreeExp, r:TreeExp) extends TreeExp
case class TreeCall(func:TreeName, args:List[TreeExp]) extends TreeExp
case class TreeConst(value:Int) extends TreeExp
case class TreeEseq(stm:TreeStm, exp:TreeExp) extends TreeExp
case class TreeMem(exp:TreeExp) extends TreeExp
case class TreeName(label:Label) extends TreeExp
case class TreeTemp(temp:Temp) extends TreeExp
case class TreeExpr(exp:TreeExp) extends TreeStm
case class TreeCjump(op:Oper.Value, l:TreeExp, r:TreeExp, t:Label, f:Label) extends TreeStm
case class TreeJump(exp:TreeExp, targets:List[Label]) extends TreeStm
case class TreeLabel(label:Label) extends TreeStm
case class TreeMove(dst:TreeExp, src:TreeExp) extends TreeStm
case class TreeSeq(l:TreeStm, r:TreeStm) extends TreeStm
case class TreeNil() extends TreeStm

case class MoveCall(dst:TreeTemp, src:TreeCall) extends TreeStm
case class MoveBinOp(dst:TreeTemp, src:TreeBinOp) extends TreeStm
case class ExpCall(call:TreeCall) extends TreeStm

object TreeSeq {
	def makeSeq(stms:TreeStm*):TreeStm = {
		def makeSeqSub(l:List[TreeStm]):TreeStm = l match {
			case x::xs => TreeSeq(x, makeSeqSub(xs))
			case Nil => TreeNil()
		}
		val slist = stms.toList
		if (slist.length == 1)
			slist.head
		else
			makeSeqSub(slist)
	}
}


case class CjumpC(op:Oper.Value, l:TreeExp, r:TreeExp) {
	def ? (l1:Label, l2:Label):TreeCjump = {
		TreeCjump(op, l, r, l1, l2)
	}

}

case class L(l:Label)

