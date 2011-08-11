package tiger.parser;

import tiger.ast._
import tiger.typ._
import tiger.common.Oper
import scala.util.parsing.combinator._
import scala.util.parsing.combinator.syntactical._
import scala.util.parsing.combinator.lexical._

class Parser extends StdTokenParsers with ImplicitConversions with PackratParsers{
	type Tokens = Lexer
	val lexical = new Tokens
	lexical.reserved ++= List("if", "then", "else", 
							  "while", "do", "for",
							  "to", "let", "in", "end",
							  "function", "var", "array",
							  "of", "break", "type", "nil"
							  /*":=", "+", "-", "*", "/",
							  "=", "<>", "<", ">", "<=", ">=", "&", "|"*/)

	lexical.delimiters ++= List("[", "]", "(", ")", "{", "}", ";", ":", ",")

	def parse(input: String) = parseRaw(input)

	def parseRaw(input : String) : Option[ASTExp] = 
		phrase(program)(new lexical.Scanner(input)) match {
			case Success(result, _) => Some(result)
			case NoSuccess(err, next) => {
				println("failed to parse " + 
					  "(line " + next.pos.line + ", column " + next.pos.column + ")\n" + 
					  err + "\n" + next.pos.longString)
				None
			}
		}

	def program:Parser[ASTExp] = expr
	def expr:Parser[ASTExp] = 
		let_expr |
		if_expr |
		while_expr |
		for_expr |
		break_expr |
		assignment_expr | 
		op_expr |
		primary_expr

	lazy val primary_expr:PackratParser[ASTExp] = 
		function_call |
		record_creation_expr |
		array_creation_expr |
		lvalue_expr | 
		string_expr | 
		neg_expr | 
		integer_expr | 
		nil_expr |
		seq_expr

	def string_expr:Parser[ASTExp] = stringVal ^^ { s => StringExp(s) }
	def integer_expr:Parser[ASTExp] = number ^^ { n => IntExp(n) }
	def nil_expr:Parser[ASTExp] = "nil" ^^ { n => NilExp() }
	def lvalue_expr:Parser[ASTExp] = lvalue ^^ {va => VarExp(va)}

	def lvalue:Parser[ASTVar] = field_var | subscript_var | simple_var

	def simple_var:Parser[ASTVar] = id ^^ { id => SimpleVar(id) }
	lazy val field_var:PackratParser[ASTVar] = lvalue ~ "." ~ id ^^ {
		case lv~"."~i => FieldVar(lv, i)
	}
	lazy val subscript_var:PackratParser[ASTVar] = lvalue ~ "[" ~ expr ~ "]" ^^ {
		case lv~"["~e~"]" => SubscriptVar(lv, e)
	}

	lazy val neg_expr:PackratParser[ASTExp] = "-" ~> expr ^^ { 
		e => OpExp(Oper.Minus, IntExp(0), e)
	}

	lazy val op_expr:PackratParser[ASTExp] = or_expr
	lazy val or_expr:PackratParser[ASTExp] = 
		and_expr ~ "|" ~ and_expr ^^ {case l~"|"~r  => OpExp(Oper.Or, l, r)} |
		and_expr

	lazy val and_expr:PackratParser[ASTExp] = 
		rel_expr ~ "&" ~ rel_expr ^^ {case l~"&"~r  => OpExp(Oper.And, l, r)} |
		rel_expr

	lazy val rel_expr:PackratParser[ASTExp] = 
		add_expr ~ "=" ~  add_expr ^^ {case l~"="~r  => OpExp(Oper.Eq, l, r)} |
		add_expr ~ "<>" ~ add_expr ^^ {case l~"<>"~r => OpExp(Oper.Ne, l, r)} |
		add_expr ~ "<=" ~ add_expr ^^ {case l~"<="~r => OpExp(Oper.Le, l, r)} |
		add_expr ~ ">=" ~ add_expr ^^ {case l~">="~r => OpExp(Oper.Ge, l, r)} |
		add_expr ~ "<" ~  add_expr ^^ {case l~"<"~r  => OpExp(Oper.Lt, l, r)} |
		add_expr ~ ">" ~  add_expr ^^ {case l~">"~r  => OpExp(Oper.Gt, l, r)} |
		add_expr

	lazy val add_expr:PackratParser[ASTExp] = 
		mul_expr ~ "+" ~ mul_expr ^^ {case l~"+"~r  => OpExp(Oper.Plus, l, r)} |
		mul_expr ~ "-" ~ mul_expr ^^ {case l~"-"~r  => OpExp(Oper.Minus, l, r)} |
		mul_expr 

	lazy val mul_expr:PackratParser[ASTExp] = 
		primary_expr ~ "*" ~ primary_expr ^^ {case l~"*"~r  => OpExp(Oper.Times, l, r)} |
		primary_expr ~ "/" ~ primary_expr ^^ {case l~"/"~r  => OpExp(Oper.Divide, l, r)} |
		primary_expr 
	def assignment_expr:Parser[ASTExp] = lvalue ~ ":=" ~ expr ^^ {
		case lv~":="~e => AssignExp(lv, e)
	}

	def function_call:Parser[ASTExp] = id ~ "(" ~ repsep(expr, ",") ~ ")" ^^ {
		case name~"("~args~")" => CallExp(name, args)
	}
	
	def seq_expr:Parser[ASTExp] = "(" ~> repsep(expr, ";") <~ ")" ^^ {
		seq => SeqExp(seq)
	}

	def record_creation_expr:Parser[ASTExp] = id ~ "{" ~ repsep(record_field, ",") ~ "}" ^^ {
		case id~"{"~recs~"}" => RecordExp(id, recs)
	}
	def record_field:Parser[RecordField] = id ~ "=" ~ expr ^^ {
		case id~"="~e => RecordField(id, e)
	}
	def array_creation_expr:Parser[ASTExp] = id ~ "[" ~ expr ~ "]" ~ "of" ~ expr ^^ {
		case id~"["~size~"]"~"of"~init => ArrayExp(id, size, init)
	}

	def if_expr:Parser[ASTExp] = 
		"if" ~ expr ~ "then" ~ expr ~ "else" ~ expr ^^ {
			case "if"~test~"then"~trueexp~"else"~elseexp => IfExp(test, trueexp, Some(elseexp))
		} |
		"if" ~ expr ~ "then" ~ expr ^^ {
			case "if"~test~"then"~trueexp => IfExp(test, trueexp, None)
		}

	def while_expr:Parser[ASTExp] = "while" ~ expr ~ "do" ~ expr ^^ {
		case "while"~test~"do"~body => WhileExp(test, body)
	}

	def for_expr:Parser[ASTExp] = "for" ~ id ~ ":=" ~ expr ~ "to" ~ expr ~ "do" ~ expr ^^ {
		case "for"~id~":="~lo~"to"~hi~"do"~body => ForExp(id, lo, hi, body)
	}

	def break_expr:Parser[ASTExp] = "break" ^^ {n => BreakExp() }

	def let_expr:Parser[ASTExp] = "let" ~ repsep(dec, rep(" ")) ~ "in" ~ repsep(expr, ";") ~ "end" ^^ {
		case "let"~decs~"in"~body~"end" => LetExp(decs, SeqExp(body))
	}
	
	def dec:Parser[ASTDec] = typedec | vardec | fundec

	def typedec:Parser[ASTDec] = "type" ~ id ~ "=" ~ typ ^^ {
		case "type"~id~"="~ty => TypeDec(id, ty)
	}
	def typ:Parser[ASTType] = id ^^ { n => NameTy(n) } | 
							typefields ^^ { fields => RecordTy(fields) } | 
							"array" ~ "of" ~> id ^^ { n => ArrayTy(n) }

	def typefields:Parser[List[TypeField]] = "{" ~> repsep(typefield, ",") <~ "}"
	def typefield:Parser[TypeField] = id ~ ":" ~ id ^^ { case name~":"~typ => TypeField(name, typ) }

	def vardec:Parser[ASTDec] = 
		"var" ~ id ~ ":" ~ id ~ ":=" ~ expr ^^ {
			case "var"~name~":"~typ~":="~init => VarDec(name, typ, init)
		} | 
		"var" ~ id ~ ":=" ~ expr ^^ {
			case "var"~name~":="~init => VarDec(name, 'nil, init)
		}

	def fundec:Parser[ASTDec] = 
		"function" ~ id ~ funparamdec ~ ":" ~ id ~ "=" ~ expr ^^ {
			case "function"~name~params~":"~result~"="~body => FunDec(name, params, result, body)
		} |
		"function" ~ id ~ funparamdec ~ "=" ~ expr ^^ {
			case "function"~name~params~"="~body => FunDec(name, params, 'nil, body)
		}
	
	def funparamdec:Parser[List[TypeField]] = "(" ~> repsep(typefield, ",") <~ ")" 

	def id:Parser[Symbol] = accept("id", { case lexical.Identifier(n) => Symbol(n)} )

	def stringVal = accept("string", { case lexical.StringLit(n) => n} )
	def number    = accept("number", { case lexical.NumericLit(n) => n.toInt} )
}
