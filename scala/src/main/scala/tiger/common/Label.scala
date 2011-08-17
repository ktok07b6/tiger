package tiger.common

object Label {
	var count:Int = 0
}

class Label(sym:Symbol) {
	def this() = {
		this(Symbol("L" + Label.count))
		Label.count += 1
	}
	override def toString():String = sym.name
}

