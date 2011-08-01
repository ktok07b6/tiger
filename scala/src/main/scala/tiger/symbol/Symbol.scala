package tiger.symbol;

class xSymbol(n:String) {
	val name:String = n
	override def toString = name
}

object xSymbolTable {
	var table:Map[String, Symbol] = Map.empty;
	def symbol(name:String):Symbol = {
		if (table.contains(name) == false) {
			table += (name -> 'name);
		}
		table(name)
	}
}
