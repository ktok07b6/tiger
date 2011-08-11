package tiger.tree

import tiger.common._

abstract class Tree
abstract class TreeExp extends Tree
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
//case class TreeJump(exp:TreeExp, t:Label) extends TreeStm
case class TreeJump(t:Label) extends TreeStm
case class TreeLabel(label:Label) extends TreeStm
case class TreeMove(dst:TreeExp, src:TreeExp) extends TreeStm
case class TreeSeq(l:TreeStm, r:TreeStm) extends TreeStm
case class TreeNil() extends TreeStm

object TreeSeq {
	def makeSeq(l:List[TreeStm]):TreeStm = l match {
		case x::xs => TreeSeq(x, makeSeq(xs))
		case Nil => TreeNil()
	}
}


