package tiger.symbol

import tiger.typ._

object Table {
	def getVarEntry(key:Symbol):Option[VarEntry] = 
		if (nameTable.contains(key))
			Some(nameTable(key))
		else
			None
	
	def setVarEntry(key:Symbol, va:VarEntry) {
		nameTable += (key->va);
	}

	var nameTable:Map[Symbol, VarEntry] = Map.empty
}
