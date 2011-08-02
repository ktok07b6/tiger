package tiger.util

object Error extends Enumeration {
	val SyntaxError,
	TypeMissmatch = Value
}
object ErrorReport  {
	def print(e:Error) {
		print(e)
	}
}
