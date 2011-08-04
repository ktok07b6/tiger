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

case class ArrayT(element:Type) extends Type {
	override def toString = "ArrayT[" + element + "]"
}

case class RecordElem(fieldName:Symbol, fieldType:Type)

case class RecordT(elems:List[RecordElem]) extends Type {
	def elemString(e:RecordElem) = {
		require(e.fieldName != null);
		require(e.fieldType != null);
		e.fieldName.name + ":" + e.fieldType.toString
	}
	override def toString = {
		"RecordT{" + elems.map(elemString).mkString(",") + "}"
	}

	def getType(s:Symbol):Option[Type] = {
		elems.find(_.fieldName==s) match {
			case Some(elem) => Some(elem.fieldType)
			case None => None
		}
	}
}

object Type {
	def coerceTo(lhs:Option[Type], rhs:Option[Type]):Boolean = {
		if (lhs.isDefined && rhs.isDefined ) {
			val l = lhs.get
			val r = rhs.get
			println("coerceTo " + l + "," + r)
			l match {
				case IntT()    => r.actual().isInstanceOf[IntT]
				case StrT()    => r.actual().isInstanceOf[StrT]
				case NameT(_)  => coerceTo(Some(l.actual), rhs)
				case NilT()    => r.actual().isInstanceOf[RecordT] || r.actual().isInstanceOf[NilT] 
				case VoidT()   => r.actual().isInstanceOf[VoidT]
				case ArrayT(_) => l == r.actual()
				case RecordT(_) => r.actual.isInstanceOf[NilT] || l == r.actual()
				case _        => false
			}
		} else {
			false
		}
	}
}
