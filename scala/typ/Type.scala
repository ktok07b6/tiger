package tiger.typ;

abstract class Type {
	def actual():Type = this
}

case class Int() extends Type {
	override def toString = "Int"
}

case class Str() extends Type {
	override def toString = "Str"
}

case class Name(name:Symbol) extends Type {
	var binding:Type = null
	override def actual():Type = binding.actual()
	override def toString = "Name(" + name + ")->" + actual()
	def bind(t:Type) {binding = t}
}

case class Nil() extends Type {
	override def toString = "Nil"
}

case class Void() extends Type {
	override def toString = "Void"
}

case class Array(element:Type) extends Type {
	override def toString = "Array[" + element + "]"
}

case class Record(fieldName:Symbol, fieldType:Type, tail:Record) extends Type {
	override def toString = toStringSub(this)
	private def toStringSub(head:Record):String = {
		require(fieldName != null);
		require(fieldType != null);

		if (fieldType == head) {
			"Record(" + fieldName +	":(this))"
		} else {
			var s0:String = fieldType match {
				case r:Record => r.toStringSub(head) + ")"
				case _        => fieldType + ")"
			}
			s0 = "Record(" + fieldName + ":" + s0;
			if (tail != null) {
				s0 +=  "->" + tail.toStringSub(head);
			}
			s0
		}
	}
}

object Type {
	def coerceTo(lhs:Type, rhs:Type):Boolean = lhs match {
		case Int()    => rhs.actual().isInstanceOf[Int]
		case Str()    => rhs.actual().isInstanceOf[Str]
		case Name(_)  => coerceTo(lhs.actual(), rhs)
		case Nil()    => rhs.actual().isInstanceOf[Record] || rhs.actual().isInstanceOf[Nil] 
		case Void()   => rhs.actual().isInstanceOf[Void]
		case Array(_) => lhs == rhs.actual()
		case Record(_, _, _) => rhs.actual.isInstanceOf[Nil] || lhs == rhs.actual()
		case _        => false
	}
}
