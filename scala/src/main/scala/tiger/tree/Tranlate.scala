package tiger.tree

import tiger.common._
import tiger.tree._

abstract class Translate {
	def unEx():Option[TreeExp]
	def unNx():Option[TreeStm]
	def unCx(t:Label, f:Label):Option[TreeCjump]
}

case class Ex(exp:TreeExp) extends Translate {
	override def unEx():Option[TreeExp] = Some(exp)
	override def unNx():Option[TreeStm] = Some(TreeExpr(exp))
	override def unCx(t:Label, f:Label):Option[TreeCjump] = None
}

case class Nx(stm:TreeStm) extends Translate {
	override def unEx():Option[TreeExp] = None
	override def unNx():Option[TreeStm] = Some(stm)
	override def unCx(t:Label, f:Label):Option[TreeCjump] = None
}

case class Cx(op:Oper.Value, l:TreeExp, r:TreeExp) extends Translate {
	override def unEx():Option[TreeExp] = {
		val r = TreeTemp(new Temp())
		val r_1 = TreeMove(r, TreeConst(1))
		val r_0 = TreeMove(r, TreeConst(0))
		val t = new Label()
		val f = new Label()
		val label_t = TreeLabel(t)
		val label_f = TreeLabel(f)
		val cmp = unCx(t, f).get
		val seq = TreeSeq.makeSeq(List(r_1, cmp, label_f, r_0, label_t))
		Some(TreeEseq(seq, r))
	}
	override def unNx():Option[TreeStm] = {
		val l = new Label()
		Some(TreeSeq(unCx(l,l).get, TreeLabel(l)))
	}
	override def unCx(t:Label, f:Label):Option[TreeCjump] = {
		Some(TreeCjump(op, l, r, t, f))
	}
}
