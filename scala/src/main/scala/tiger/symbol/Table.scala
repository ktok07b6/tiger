package tiger.symbol

import tiger.typ._
import tiger.ast._

object Table {
	class TableScope {
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
			println("putType " + key + "->" + t)
			typeTable += (key->t);
		}
		var typeTable:Map[Symbol, Type] = Map.empty

		def dump(indent:String):Unit = {
			println(indent + this)
			println(indent + "---name table---")
			nameTable.foreach({a => println(indent+a)})
		}
	}

	def getVarEntry(key:Symbol):Option[VarEntry] = {
		for (t <- tables) {
			val ve = t.getVarEntry(key)
			if (ve.isDefined) return ve
		}
		None
	}	
	def putVarEntry(key:Symbol, va:VarEntry) = {
		tables.head.putVarEntry(key, va)
	}
	def getFuncEntry(key:Symbol):Option[FuncEntry] = {
		for (t <- tables) {
			val fe = t.getFuncEntry(key)
			if (fe.isDefined) return fe
		}
		None
	}
	def putFuncEntry(key:Symbol, fe:FuncEntry) = {
		tables.head.putFuncEntry(key, fe)
	}
	def getType(key:Symbol):Option[Type] = {
		for (t <- tables) {
			val ty = t.getType(key)
			if (ty.isDefined) return ty
		}
		None
	}
	def putType(key:Symbol, t:Type) = {
		tables.head.putType(key, t)
	}
	def beginScope():Unit = {
		val t = new TableScope
		tables = t :: tables
	}
	def endScope():Unit = {
		tables = tables.drop(1)
	}
	def init():Unit = {
		tables = List.empty
	}
	def dump():Unit = {
		var indent:String = ""
		tables.reverse.foreach({t => t.dump(indent); indent+="    "})
	}
	var tables:List[TableScope] = List.empty
}
