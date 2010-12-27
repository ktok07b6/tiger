package tiger.symbol

import tiger.typ._
import tiger.ast._

object Table {
	def getVarEntry(key:Symbol):Option[VarEntry] = 
		if (nameTable.contains(key))
			Some(nameTable(key).asInstanceOf[VarEntry])
		else
			None
	
	def putVarEntry(key:Symbol, va:VarEntry) {
		nameTable += (key->va);
	}

	def getFuncEntry(key:Symbol):Option[FuncEntry] = 
		if (nameTable.contains(key))
			Some(nameTable(key).asInstanceOf[FuncEntry])
		else
			None
	
	def putFuncEntry(key:Symbol, fe:FuncEntry) {
		nameTable += (key->fe);
	}

	var nameTable:Map[Symbol, NameEntry] = Map.empty

	def getType(key:Symbol):Option[Type] = 
		if (typeTable.contains(key))
			Some(typeTable(key))
		else
			None
	
	def putType(key:Symbol, t:Type) {
		typeTable += (key->t);
	}
	var typeTable:Map[Symbol, Type] = Map.empty

	def beginScope():Unit = {}
	def endScope():Unit = {}
}
