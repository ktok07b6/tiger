package tiger.ast

import tiger.typ._
import tiger.common.Oper

object ASTPrinter {
	def print(ast:AST) {
		println(toString(ast))
	}

	def toString(ast:AST):String = ast match {
		case v:SimpleVar => {
			v.sym.name + " "
		}
		case v:FieldVar => {
			toString(v.va) + "." + v.field.name + " "
		}
		case v:SubscriptVar => {
			toString(v.va) + 
			"[" + 
			toString(v.exp) + 
			"] "
		}
		case e:VarExp => {
			toString(e.va)
		}
		case e:NilExp => {
			"nil "
		}
		case e:IntExp => {
			e.n + ""
		}
		case e:StringExp => {
			e.s + ""
		}
		case e:CallExp => {
			e.func.name + "(" + 
			e.exps.map(toString).mkString(",") + ")"
		}
		case e:OpExp => {
			toString(e.l) + " " + Oper.toString(e.op) + " " + toString(e.r)
		}
		case e:RecordExp => {
			e.typ.name + "{" + 
			e.fields.map(toString).mkString(",") + "}"
		}
		case e:SeqExp => {
			e.seq.map(toString).mkString(";")
		}
		case e:AssignExp => {
			toString(e.va) + " := " + toString(e.exp)
		}
		case e:IfExp => {
			val elsestr = e.elseexp match {
				case Some(e) => toString(e) 
				case None => ""
			}
			"if (" + toString(e.test) + ") {" + 
			toString(e.thenexp) + "} else {" + elsestr + "} "
		}
		case e:WhileExp => {
			"while (" + 
			toString(e.test) + ") {" + 
			toString(e.body) +  
			"} "
		}
		case e:ForExp => {
			"for (" + e.va.name + " := " + 
			toString(e.lo) + " to " + 
			toString(e.hi) + " do " + 
			toString(e.body)
		}
		case e:BreakExp => {
			"break "
		}
		case e:LetExp => {
			"let " +
			e.decs.map(toString).mkString(" ") + " in " +
			toString(e.body) + " end "
		}
		case e:ArrayExp => {
			e.typ.name + " " + 
			"[" + toString(e.size) + "]" + " of " + 
			toString(e.init) + " "
		}
		case d:FunDec => {
			"function " + 
			d.name.name + "(" + 
			d.params.map(toString).mkString(",") + "):" + 
			d.result.name + " = " + 
			toString(d.body)
		}
		case d:VarDec => {
			"var " + 
			d.name.name + ":" + 
			d.typ.name + " := " + 
			toString(d.init)
		}
		case d:TypeDec => {
			"type " + 
			d.name.name + " = " + 
			toString(d.typ)
		}
		case t:NameTy => {
			t.name.name
		}
		case t:RecordTy => {
			"{" + 
			t.fields.map(toString).mkString(",") + 
			"}"
		}
		case t:ArrayTy => {
			"array of " + t.name.name
		}
		case f:TypeField => {
			f.name.name + ":" + 
			f.typ.name
		}
		case f:RecordField => {
			f.name.name + ":" +
			//(f.varEntry ? f.varEntry.typ:NilT()) + "," + 
			toString(f.init)
		}
		case _ => {
			"???"
		}
	}
}
