package tiger.symbol;

class Symbol(n:String) {
	val name:String = n
	override def toString = name
}

object SymbolTable {
	var table:Map[String, Symbol] = Map.empty;
	def symbol(name:String):Symbol = {
		if (table.contains(name) == false) {
			table += (name -> new Symbol(name));
		}
		table(name)
	}
}
