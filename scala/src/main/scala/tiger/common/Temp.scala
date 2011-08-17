package tiger.common

object Temp {
	var count:Int = 0
}

class Temp(name:Symbol) {
	def this() = {
		this(Symbol("T" + Temp.count))
		Temp.count += 1
	}
	override def toString():String = name.name
}
