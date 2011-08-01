package tiger.ast

object ASTPrinter {
	def print(ast:AST) {
		println(toString(ast))
	}

	def toString(ast:AST):String = ast match {
		case v:SimpleVar => {
			"SimpleVar"
		}
		case v:FieldVar => {
			"FieldVar"
		}
		case v:SubscriptVar => {
			"SubscriptVar"
		}
		case e:VarExp => {
			"VarExp"
		}
		case e:NilExp => {
			"NilExp"
		}
		case e:IntExp => {
			"IntExp"
		}
		case e:StringExp => {
			"StringExp"
		}
		case e:CallExp => {
			"CallExp"
		}
		case e:OpExp => {
			"OpExp"
		}

		case e:RecordExp => {
			"RecordExp"
		}
		case e:SeqExp => {
			"SeqExp"
		}
		case e:AssignExp => {
			"AssignExp"
		}
		case e:IfExp => {
			"IfExp"
		}
		case e:WhileExp => {
			"WhileExp"
		}
		case e:ForExp => {
			"ForExp"
		}
		case e:BreakExp => {
			"BreakExp"
		}
		case e:LetExp => {
			"LetExp"
		}
		case e:ArrayExp => {
			"ArrayExp"
		}
		case d:FunDec => {
			"FunDec"
		}
		case d:VarDec => {
			"VarDec"
		}
		case d:TypeDec => {
			"TypeDec"
		}
		case t:NameTy => {
			"NameTy"
		}
		case t:RecordTy => {
			"RecordTy"
		}
		case t:ArrayTy => {
			"ArrayTy"
		}
		case f:TypeField => {
			"TypeField"
		}
		case f:RecordField => {
			"RecordField"
		}
		case _ => {
			"???"
		}
	}
}
