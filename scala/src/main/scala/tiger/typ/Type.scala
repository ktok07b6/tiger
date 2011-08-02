package tiger.typ;

import tiger.symbol._

abstract class Type {
	def actual():Type = this
}

case class IntT() extends Type {
	override def toString = "IntT"
}

case class StrT() extends Type {
	override def toString = "StrT"
}

case class NameT(name:Symbol) extends Type {
	var binding:Option[Type] = None
	override def actual():Type = binding.get.actual
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

case class RecordT(fieldName:Symbol, fieldType:Type, tail:RecordT) extends Type {
	override def toString = toStringSub(this)
	private def toStringSub(head:RecordT):String = {
		require(fieldName != null);
		require(fieldType != null);

		if (fieldType == head) {
			"RecordT(" + fieldName +	":(this))"
		} else {
			var s0:String = fieldType match {
				case r:RecordT => r.toStringSub(head) + ")"
				case _         => fieldType + ")"
			}
			s0 = "RecordT(" + fieldName + ":" + s0;
			if (tail != null) {
				s0 +=  "->" + tail.toStringSub(head);
			}
			s0
		}
	}
}

object Type {
	def coerceTo(lhs:Type, rhs:Type):Boolean = lhs match {
		case IntT()    => rhs.actual().isInstanceOf[IntT]
		case StrT()    => rhs.actual().isInstanceOf[StrT]
		case NameT(_)  => coerceTo(lhs.actual(), rhs)
		case NilT()    => rhs.actual().isInstanceOf[RecordT] || rhs.actual().isInstanceOf[NilT] 
		case VoidT()   => rhs.actual().isInstanceOf[VoidT]
		case ArrayT(_) => lhs == rhs.actual()
		case RecordT(_, _, _) => rhs.actual.isInstanceOf[NilT] || lhs == rhs.actual()
		case _        => false
	}
}
