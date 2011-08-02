import tiger.ast._
import tiger.symbol._
import tiger.parser._
import tiger.typ._

object Tiger {
	def main(args: Array[String]) {
		testParser
		testLexer
	}

	def testAST() {
		ASTPrinter.print(SimpleVar('a))
		ASTPrinter.print(FieldVar(SimpleVar('x),'y))
		ASTPrinter.print(SubscriptVar(SimpleVar('x), IntExp(99)))
		ASTPrinter.print(VarExp(SimpleVar('x)))
		ASTPrinter.print(NilExp())
		ASTPrinter.print(IntExp(100))
		ASTPrinter.print(StringExp("string"))
		ASTPrinter.print(CallExp('func, List(IntExp(1),IntExp(2),IntExp(3))))
		ASTPrinter.print(OpExp(Oper.Plus, IntExp(10), IntExp(20)))

		ASTPrinter.print(RecordField('rec, IntExp(10)))
	}

	def testParser_(source:String) {
		val parser = new Parser
		val result = parser.parse(source)
		result match {
			case Some(ast) => ASTPrinter.print(ast)
			case _ => println("!!!!! PARSE FAILED !!!!!:" + source)
		}
	}
	def testParser() {
		testParser_("1")
		testParser_("a")
		testParser_("x.y")
		testParser_("x[1]")
		testParser_("-1")
		testParser_("1+1")
		testParser_("1--1")
		testParser_("\"a\"=\"b\"")
		testParser_("a<=1")
		testParser_("a & b")
		testParser_("a := b")
		testParser_("func(100, 200, 300)")
		testParser_("func()")
		testParser_("func(  )")
		testParser_("(1; 2)")
		testParser_("rec {a := 1, b := 2}")
		testParser_("a[10] of 0")
		testParser_("if a < 0 then fun1")
		testParser_("if a < 0 then fun1 else fun2")
		testParser_("while a = 0 do (fun1;fun2)")
		testParser_("for i := 0 to 10 do (fun1;break)")
		testParser_("let var a:=0 in a end")
		testParser_("let var a:int:=0 in a end")
		testParser_("let type i = int in end")
		testParser_("let function f():int = 10 in end")
	}

	def testLexer_(source:String) {
		val lexer = new Lexer
		lexer.reserved ++= List("if", "then", "else", 
							  "while", "do", "for",
							  "to", "let", "in", "end",
							  "function", "var", "array",
							  "of", "break", "type", "nil",
							  ":=", ":", "+", "-", "*", "/",
							  "=", "<>", "<", ">", "<=", ">=", "&", "|")

		lexer.delimiters ++= List("[", "]", "(", ")", "{", "}", ";", ",")
		val scanner = new lexer.Scanner(source)
		val result = Stream.iterate(scanner)(_.rest).takeWhile(!_.atEnd).map(_.first).toList
		println(result)

	}
	def testLexer() {
		testLexer_("a+1")
		testLexer_("a=1")
		testLexer_("a<=1")
		testLexer_("a:=1")
		testLexer_("func(100, 200, 300)")
		testLexer_("(1; 2)")
		testLexer_("rec {a := 1, b := 2}")
		testLexer_("if a < 0 then fun1")
		testLexer_("if a < 0 then fun1 else fun2")
		testLexer_("let var a:int:=0 in a end")
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
