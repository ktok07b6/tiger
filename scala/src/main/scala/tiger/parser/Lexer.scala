package tiger.parser;
import scala.util.parsing.combinator._
import scala.util.parsing.combinator.syntactical._
import scala.util.parsing.combinator.lexical._
import scala.util.parsing.input.CharArrayReader.EofCh

class Lexer extends StdLexical with ImplicitConversions {
	override def token: Parser[Token] = 
		( string ^^ StringLit
		 | '-' ~ number ^^ { case num => NumericLit("-" + num) }
		 | number ^^ NumericLit
		 | EofCh ^^^ EOF
		 | delim
		 | '\"' ~> failure("Unterminated string")
		 | id ^^ checkKeyword
		 | operator ^^ { s => Keyword(s) }
		 | failure("Illegal character")
	 )


	def number = intPart
	def intPart = zero | intList
	def intList = nonzero ~ rep(digit) ^^ {case x ~ y => (x :: y) mkString ""}
	def zero: Parser[String] = '0' ^^^ "0"
	def nonzero = elem("nonzero digit", d => d.isDigit && d != '0')

	def id = rep1(identChar) ^^ { _ mkString "" } 
	def operator = ((elem(':')~elem('=')) |
					(elem('<')~elem('>')) |
					(elem('<')~elem('=')) |
					(elem('>')~elem('=')) |
					elem('=') | 
					elem('<') | 
					elem('>') | 
					elem('&') |
					elem('|') |
					elem('+') |
					elem('-') |
					elem('*') |
					elem('/')
				) ^^ { _ toString }

	def checkKeyword(strRep: String) = {
		if (reserved contains strRep) Keyword(strRep) else Identifier(strRep)
	}
/*
	def checkKeyword(xs : List[Any]) = {
		val strRep = xs mkString ""
		if (reserved contains strRep) Keyword(strRep) else ErrorToken("Not a keyword: " + strRep)
	}
*/

	def string = '\"' ~> rep(charSeq | chrExcept('\"', '\n', EofCh)) <~ '\"' ^^ { _ mkString "" }

    def charSeq: Parser[String] =
		('\\' ~ '\"' ^^^ "\""
		 |'\\' ~ '\\' ^^^ "\\"
		 |'\\' ~ '/'  ^^^ "/"
		 |'\\' ~ 'b'  ^^^ "\b"
		 |'\\' ~ 'f'  ^^^ "\f"
		 |'\\' ~ 'n'  ^^^ "\n"
		 |'\\' ~ 'r'  ^^^ "\r"
		 |'\\' ~ 't'  ^^^ "\t"
		 |'\\' ~> 'u' ~> unicodeBlock)

	val hexDigits = Set[Char]() ++ "0123456789abcdefABCDEF".toArray
	def hexDigit = elem("hex digit", hexDigits.contains(_))

	private def unicodeBlock = hexDigit ~ hexDigit ~ hexDigit ~ hexDigit ^^ {
		case a ~ b ~ c ~ d =>
			new String(Array(Integer.parseInt(List(a, b, c, d) mkString "", 16)), 0, 1)
	} 

}
