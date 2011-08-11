package tiger.typ;

import tiger.symbol._

abstract class Type {
	def actual():Type = this
}

case class UndefinedT() extends Type {
	override def toString = "UndefinedT"
}

case class IntT() extends Type {
	override def toString = "IntT"
}

case class StrT() extends Type {
	override def toString = "StrT"
}

case class NameT(name:Symbol) extends Type {
	var binding:Option[Type] = None
	override def actual():Type = {
		binding.getOrElse(UndefinedT()).actual
	}
	override def toString = "NameT(" + name + ")->" + actual()
	def bind(t:Type) {binding = Some(t)}
}

case class NilT() extends Type {
	override def toString = "NilT"
}

case class VoidT() extends Type {
	override def toString = "VoidT"
}

case class ArrayT(id:Symbol, element:Type) extends Type {
	override def toString = "ArrayT(" + id + ")[" + element + "]"
	def == (rhs:Type):Boolean = {
		rhs match {
			case t:ArrayT => id == t.id
			case _ => false
		}
	}
}

case class RecordElem(fieldName:Symbol, fieldType:Type)

case class RecordT(id:Symbol, elems:List[RecordElem]) extends Type {
	def elemString(e:RecordElem) = {
		require(e.fieldName != null);
		require(e.fieldType != null);
		e.fieldName.name + ":" + e.fieldType.toString
	}
	override def toString = {
		"RecordT(" + id + "){" + elems.map(elemString).mkString(",") + "}"
	}

	def findFieldType(s:Symbol):Option[Type] = {
		elems.find(_.fieldName==s) match {
			case Some(elem) => Some(elem.fieldType)
			case None => None
		}
	}
	def == (rhs:Type):Boolean = {
		rhs match {
			case t:RecordT => println(id + "==" + t.id); id == t.id
			case _ => false
		}
	}
}

object Type {
	def coerceTo(l:Type, r:Type):Boolean = {
		//println("coerceTo " + l + "," + r)
		l match {
			case IntT()     => r.actual().isInstanceOf[IntT]
			case StrT ()    => r.actual().isInstanceOf[StrT]
			case NameT(_)   => coerceTo(l.actual, r)
			case NilT()     => r.actual().isInstanceOf[RecordT] || r.actual().isInstanceOf[NilT] 
			case VoidT()    => r.actual().isInstanceOf[VoidT]
			case ll:ArrayT  => ll == r.actual
			case ll:RecordT => r.actual.isInstanceOf[NilT] || ll == r.actual
			case _        => false
		}
	}
	
	def createArrayT(element:Type):ArrayT = {
		arrayID+=1
		ArrayT(Symbol("array"+arrayID), element)
	} 
	def createRecordT(elems:List[RecordElem]):RecordT = {
		recordID+=1
		RecordT(Symbol("record"+recordID), elems)
	} 
	var arrayID:Int = 0
	var recordID:Int = 0
}
