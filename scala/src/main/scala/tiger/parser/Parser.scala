package tiger.parser;

import tiger.ast._
import scala.util.parsing.combinator._
import scala.util.parsing.combinator.syntactical._
import scala.util.parsing.combinator.lexical._

class Parser extends StdTokenParsers with ImplicitConversions {
	type Tokens = Lexer
	val lexical = new Tokens
	lexical.reserved ++= List("if", "then", "else", 
							  "while", "do", "for",
							  "to", "let", "in", "end",
							  "function", "var", "array",
							  "of", "break", "type", "nil",
							  ":=", "+", "-", "*", "/",
							  "=", "<>", "<", ">", "<=", ">=", "&", "|")

	lexical.delimiters ++= List("[", "]", ":", ",")

	def parse(input: String): Option[AST] = parseRaw(input)
	def parseRaw(input : String) : Option[AST] = 
		phrase(program)(new lexical.Scanner(input)) match {
			case Success(result, _) => Some(result)
			case _ => None
		}

	def program:Parser[AST] = expr
	def expr:Parser[AST] = string_expr | integer_expr | nil_expr
	def string_expr:Parser[AST] = stringVal ^^ { case s:String => StringExp(s) }
	def integer_expr:Parser[AST] = number ^^ { case n:Int => IntExp(n) }
	def nil_expr:Parser[AST] = "nil" ^^ { case _ => NilExp() }
	def stringVal = accept("string", { case lexical.StringLit(n) => n} )
	def number    = accept("number", { case lexical.NumericLit(n) => n.toInt} )

}
