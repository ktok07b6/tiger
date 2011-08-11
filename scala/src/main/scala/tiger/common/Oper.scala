package tiger.common

object Oper extends Enumeration {
	val Plus, Minus, Times, Divide, Eq, Ne, Lt, Gt, Le, Ge, And, Or = Value
	def toString(v:Value):String = v match {
		case Plus => "+"
		case Minus => "-"
		case Times => "*"
		case Divide => "/"
		case Eq => "="
		case Ne => "<>"
		case Lt => "<"
		case Gt => ">"
		case Le => "<="
		case Ge => ">="
		case And => "&"
		case Or => "|"
	}
}
