import tiger.ast._
import tiger.symbol._
import tiger.parser._

object Tiger {
	def main(args: Array[String]) {
		testAST
	}

	def testAST() {
		ASTPrinter.print(SimpleVar(Symbol("a")))
		ASTPrinter.print(FieldVar(SimpleVar(Symbol("x")),Symbol("y")))
		ASTPrinter.print(SubscriptVar(SimpleVar(Symbol("x")), IntExp(99)))
	}

	def testParser() {
		val parser = new Parser
		val result = parser.parse("1")
	}

	def testLexer() {
		val lexer = new Lexer
		lexer.reserved ++= List("if", "then", "else", 
							  "while", "do", "for",
							  "to", "let", "in", "end",
							  "function", "var", "array",
							  "of", "break", "type", "nil",
							  ":=", "+", "-", "*", "/",
							  "=", "<>", "<", ">", "<=", ">=", "&", "|")

		lexer.delimiters ++= List("[", "]", ":", ",")
		val scanner = new lexer.Scanner("var a:int := 0")
		val result = Stream.iterate(scanner)(_.rest).takeWhile(!_.atEnd).map(_.first).toList
		print(result)
	}

	def testSymbol() {
		val sa1 = 'a
		val sb1 = 'b
	   	val sa2 = 'a
		println(sa1 + " " + sa1.name)
		println(sb1 + " " + sb1.name)
		println(sa2 + " " + sa2.name)
		if (sa1 == sa2) println("ok")
		if (sa1.equals(sa2)) println("ok")
	}

	def test2() {
		val xs = List(1,2,3)
		val ys = List(4,5,6)
		for((x,y) <- xs zip ys) println(x+y)  
	}
}
