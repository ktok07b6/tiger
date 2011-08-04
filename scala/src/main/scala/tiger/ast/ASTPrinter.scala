package tiger.ast

import tiger.typ._

object ASTPrinter {
	def print(ast:AST) {
		println(toString(ast))
	}

	def toString(ast:AST):String = ast match {
		case v:SimpleVar => {
			"SimpleVar(" + v.sym.name + ")" 
		}
		case v:FieldVar => {
			"FieldVar(" + toString(v.va) + "." + v.sym.name + ")"
		}
		case v:SubscriptVar => {
			"SubscriptVar(" + 
			toString(v.va) + 
			"[" + 
			toString(v.exp) + 
			"])"
		}
		case e:VarExp => {
			"VarExp(" + toString(e.va) + ")"
		}
		case e:NilExp => {
			"NilExp()"
		}
		case e:IntExp => {
			"IntExp(" + e.n + ")"
		}
		case e:StringExp => {
			"StringExp(" + e.s + ")"
		}
		case e:CallExp => {
			"CallExp(" + 
			e.func.name + "," + 
			e.exps.map(toString).mkString(",") + 
			")"
		}
		case e:OpExp => {
			"OpExp(" + 
			e.op + "," + 
			toString(e.l) + "," + 
			toString(e.r) + 
			")"
		}
		case e:RecordExp => {
			"RecordExp(" + 
			e.typ.name + ", {" + 
			e.fields.map(toString).mkString(",") + "}" +
			")"
		}
		case e:SeqExp => {
			"SeqExp(" + 
			e.seq.map(toString).mkString(",") + 
			")"
		}
		case e:AssignExp => {
			"AssignExp(" + 
			toString(e.va) + "," + 
			toString(e.exp) + 
			")"
		}
		case e:IfExp => {
			"IfExp(" +
			toString(e.test) + "," + 
			toString(e.thenexp) + "," + 
			toString(e.elseexp) + 
			")"
		}
		case e:WhileExp => {
			"WhileExp(" + 
			toString(e.test) + "," + 
			toString(e.body) +  
			")"
		}
		case e:ForExp => {
			"ForExp(" + 
			e.va.name + "," + 
			toString(e.lo) + "," + 
			toString(e.hi) + "," + 
			toString(e.body) + 
			")"
		}
		case e:BreakExp => {
			"BreakExp"
		}
		case e:LetExp => {
			"LetExp(" +
			e.decs.map(toString).mkString(",") + "," +
			e.body.map(toString).mkString(";") + 
			")"
		}
		case e:ArrayExp => {
			"ArrayExp(" + 
			e.typ.name + "," + 
			toString(e.size) + "," + 
			toString(e.init) + 
			")"
		}
		case d:FunDec => {
			"FunDec(" + 
			d.name.name + "," + 
			d.params.map(toString) + "," + 
			d.result.name + "," + 
			toString(d.body) + 
			")"
		}
		case d:VarDec => {
			"VarDec(" + 
			d.name.name + "," + 
			d.typ.name + "," + 
			toString(d.init) + 
			")"
		}
		case d:TypeDec => {
			"TypeDec(" + 
			d.name.name + "," + 
			toString(d.typ) + 
			")"
		}
		case t:NameTy => {
			"NameTy(" + t.name.name + ")"
		}
		case t:RecordTy => {
			"RecordTy(" + 
			t.fields.map(toString).mkString(",") + 
			")"
		}
		case t:ArrayTy => {
			"ArrayTy(" + t.name.name + ")"
		}
		case f:TypeField => {
			"TypeField(" + 
			f.name.name + ":" + 
			f.typ.name + 
			")"
		}
		case f:RecordField => {
			"RecordField(" + 
			f.name.name + ":" +
			//(f.varEntry ? f.varEntry.typ:NilT()) + "," + 
			toString(f.init) + 
			")"
		}
		case _ => {
			"???"
		}
	}
}
