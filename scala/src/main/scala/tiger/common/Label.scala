package tiger.common

import tiger.tree._

object Label {
	var count:Int = 0
/*
	def ::(rhs:Label):Pair[Label] = {
		(this, rhs)
	}
*/
}

class Label(sym:Symbol) {
	def this() = {
		this(Symbol("L" + Label.count))
		Label.count += 1
	}
	override def toString():String = sym.name
}

