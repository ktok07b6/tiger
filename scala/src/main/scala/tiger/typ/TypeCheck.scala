package tiger.typ;

import tiger.ast._
import tiger.symbol._
import tiger.common._

object TypeCheck {
	def typeCheck(ast:ASTExp):Boolean = {
		Table.init
		Table.beginScope
		Table.putType('int, IntT())
		Table.putType('string, StrT())
		
		Table.putFuncEntry('print, FuncEntry(VoidT(), List(StrT())))
		Table.putFuncEntry('flush, FuncEntry(VoidT(), List()))
		Table.putFuncEntry('ord, FuncEntry(IntT(), List(StrT())))
		Table.putFuncEntry('chr, FuncEntry(StrT(), List(IntT())))
		Table.putFuncEntry('stringEqual, FuncEntry(IntT(), List(StrT(), StrT())))
		Table.putFuncEntry('stringLen, FuncEntry(IntT(), List(StrT())))
		Table.putFuncEntry('substring, FuncEntry(StrT(), List(StrT(), IntT(), IntT())))
		Table.putFuncEntry('stringConcat, FuncEntry(StrT(), List(StrT(), StrT())))
		Table.putFuncEntry('not, FuncEntry(IntT(), List(IntT())))
		Table.putFuncEntry('getch, FuncEntry(StrT(), List()))
		Table.putFuncEntry('test, FuncEntry(VoidT(), List(IntT())))

		var ret:Boolean = false;
		try {
			typeCheckExp(ast);
			ret = true
		} catch {
			case e => println(e); println(e.getStackTraceString)
			ret = false
		} finally {
			Table.endScope
		}
		ret
	}	
	def typeCheckVar(ast:ASTVar):Type = {
		val ve_ = Table.getVarEntry(ast.getSymbol)
		if (ve_.isEmpty) {
			error("undefined symbol:" + ast.getSymbol)
		}
		val ve = ve_.get
		println("typeCheckVar " + ast.getSymbol + " : " + ve)
		ast match {
			case v:SimpleVar => {
				val ve = ve_.get
				v.varEntry = ve
				ve.typ
			}

			case v:FieldVar => {
				//FIXME: nested too deep
				val ty = typeCheckVar(v.va).actual
				if (!ty.isInstanceOf[RecordT]) {
					error("Record type is expected")
				}

				val r = ty.asInstanceOf[RecordT]
				v.varEntry = ve
				r.findFieldType(v.field) match {
					case Some(t) => t
					case None => error("undefined field name:" + v.getSymbol)
				}
			}

			case v:SubscriptVar => {
				val expt = typeCheckExp(v.exp).actual
				if (!expt.isInstanceOf[IntT]) {
					error("Integer type is expected")
				}
				val vat = typeCheckVar(v.va).actual
				if (!vat.isInstanceOf[ArrayT]) {
					error("Array type is expected")
				}
				val arrayT = vat.asInstanceOf[ArrayT]
				v.varEntry = ve
				arrayT.element
			}
		}
	}

	def typeCheckExp(ast:ASTExp):Type = ast match {
		case e:VarExp => {
			typeCheckVar(e.va)
		}
		case e:NilExp => {
			NilT()
		}
		case e:IntExp => {
			IntT()
		}
		case e:StringExp => {
			StrT()
		}
		case e:CallExp => {
			val fe_ = Table.getFuncEntry(e.func)
			if (fe_.isEmpty) {
				error("undefined function:" + e.func)
			}
			val fe = fe_.get
			if (e.exps.length != fe.params.length) {
				error("number of function parameter is wrong")
			}
			for ((a,t) <- e.exps zip fe.params) {
				if (!Type.coerceTo(typeCheckExp(a), t)) {
					error("function parameter type is missmatch")
				}
			}
			e.funcEntry = fe
 			fe.result
		}
		case e:OpExp => {
			e.lt = typeCheckExp(e.l)
			e.rt = typeCheckExp(e.r)
			println("OpExp: " + e.op + "," + 
					e.l + ":" + e.lt + "," + 
					e.r + ":" + e.rt)
			if (Type.coerceTo(e.lt, e.rt)) {
				(e.lt.actual, e.op) match {
					case (IntT(), _) => IntT()
					case (StrT(), Oper.Plus) => StrT()
					case (_, Oper.Eq) | (_, Oper.Ne) => IntT()
					case _ => error("invalid operand type") 
				}
			} else {
				error("binary exp operand type is missmatched")
			}
		}

		case e:RecordExp => {
			def checkRecordField(r:RecordField) = {
			}
			val t_ = Table.getType(e.typ)
			t_ match {
				case None => error("unkown type:" + e.typ)
				case Some(t) => {
					val rt = t.actual.asInstanceOf[RecordT]
					e.fields.foreach(checkRecordField)
					rt
				}
			}
		}
		case e:SeqExp => {
			val results = e.seq.map(typeCheckExp)
			//println("SeqExp:" + results)
			//println("results.last:" + results.last)
			if (!results.isEmpty) {
				results.last
			} else {
				VoidT()
			}
		}
		case e:AssignExp => {
			val dst = typeCheckVar(e.va)
			val src = typeCheckExp(e.exp)
			if (!Type.coerceTo(dst, src)) {
				error("assignment type is missmatch")
			}
			VoidT()
		}
		case e:IfExp => {
			val testt = typeCheckExp(e.test)
			if (!testt.actual.isInstanceOf[IntT]) {
				error("condition exp must be Integer")
			}
			val thent = typeCheckExp(e.thenexp)
			e.elseexp match {
				case Some(elsee) => {
					val elset = typeCheckExp(elsee)
					if (!Type.coerceTo(thent, elset)) {
						error("then exp and else exp type is missmatch");
					}
					thent
				} 
				case None => {
					thent match {
						case t:VoidT => VoidT()
						case _ => error("if-then returns non unit")
					}
				}
			}
		}
		case e:WhileExp => {
			val testt = typeCheckExp(e.test)
			if (!testt.actual.isInstanceOf[IntT]) {
				error("condition exp must be Integer")
			}
			val bodyt = typeCheckExp(e.body)
			bodyt match {
				case t:VoidT => VoidT()
				case _ => error("body of while is not unit")
			}
		}
		case e:ForExp => {
			Table.beginScope
			val vart = NameT(e.va)
			vart.bind(Table.getType('int).get)
			val ve = VarEntry(vart)
			Table.putVarEntry(e.va, ve)
			e.varEntry = ve

			if (!typeCheckExp(e.lo).actual.isInstanceOf[IntT]) {
				error("exp must be Integer")
			}
			if (!typeCheckExp(e.hi).actual.isInstanceOf[IntT]) {
				error("exp must be Integer")
			}

			typeCheckExp(e.body)
			Table.endScope
			VoidT()
		}
		case e:BreakExp => {
			VoidT()
		}
		case e:LetExp => {
			Table.beginScope

			for(dec <- e.decs) {
				typeCheckDec(dec)
			}
			for(dec <- e.decs) {
				dec match {
					case d:FunDec => typeCheckFunDec2(d)
					case _ => ()
				}
			}
			typeCheckExp(e.body)
		}
		case e:ArrayExp => {
			Table.getType(e.typ) match {
				case Some(t) => {
					val arrt = t.actual.asInstanceOf[ArrayT]
					require(typeCheckExp(e.size).actual.isInstanceOf[IntT])
					e.init_t = typeCheckExp(e.init)
					require(Type.coerceTo(e.init_t, arrt.element))
					arrt
				}
				case _ => error("unknown type:" + e.typ)
			}
		}
	}

	def typeCheckFunDec2(d:FunDec) = {
		val fe_ = Table.getFuncEntry(d.name)
		require(fe_.isDefined)
		val fe = fe_.get

		Table.beginScope
		//Table.dump
		for ((f,t) <- d.params zip fe.params) {
			f.varEntry = VarEntry(t)
			Table.putVarEntry(f.name, f.varEntry) 
			println("Table.putVarEntry " + f.name + " -> " + f.varEntry)
		}

		//result type check
		val actual_result = typeCheckExp(d.body)
		if (!Type.coerceTo(fe.result, actual_result)) {
			error("function return type is missmatch")
		}

		d.funcEntry = fe
		Table.endScope		
	}

	def typeCheckDec(ast:ASTDec):Type = ast match {
		case d:FunDec => {
			val params = d.params.map(typeCheckTypeField)
			val result_ = Table.getType(d.result)
			result_ match {
				case Some(result) => {
					Table.putFuncEntry(d.name, FuncEntry(result, params))
				}
				case None => {
					//result type is Void
					Table.putFuncEntry(d.name, FuncEntry(VoidT(), params))
				}
			}
			VoidT()
		}

		case d:VarDec => {
			val vart_ = Table.getType(d.typ)
			vart_ match {
				case Some(vart) => {
					val ve = VarEntry(vart)
					Table.putVarEntry(d.name, ve)
					val initt = typeCheckExp(d.init)
					println(vart + " :=  " + initt)
					if (!Type.coerceTo(vart, initt)) {
						error("var initialize exp type mismatch")
					}
					d.varEntry = ve
				}
				case None => {
					val initt = typeCheckExp(d.init)
					val ve = VarEntry(initt)
					if (initt.actual.isInstanceOf[NilT]) {
						error("initializing nil expressions not constrained by record type");
					}
					Table.putVarEntry(d.name, ve)
					d.varEntry = ve
				}
			}
			VoidT()
		}
		case d:TypeDec => {
			val t = typeCheckType(d.typ)
			t match {
				case r:RecordT => Table.putType(d.name, r)//TODO 
				case t:Type => Table.putType(d.name, t)
			}
			VoidT()
		}
	}

	def typeCheckType(ast:ASTType):Type = ast match {
		case t:NameTy => {
			val namet = NameT(t.name)
			val t_ = Table.getType(t.name)
			t_ match {
				case Some(t) => namet.bind(t)
				case None => error("unknown type name")
			}
			namet
		}
		case t:RecordTy => {
			def recFieldTypeCheck(f:TypeField):RecordElem = {
				val ty = typeCheckTypeField(f)
				RecordElem(f.name, ty)
			}
			Type.createRecordT(t.fields.map(recFieldTypeCheck))
		}

		case t:ArrayTy => {
			val elemt_ = Table.getType(t.name)
			elemt_ match {
				case Some(t) => Type.createArrayT(t)
				case None => error("unknown type name:" + t.name)
			}
		}
	}

	def typeCheckTypeField(f:TypeField):Type = {
		val opt = Table.getType(f.typ)
		opt match {
			case Some(t) => t
			case None    => NameT(f.typ)
		}
	}

	def typeCheckRecordField(f:RecordField):Type = {
		val initt = typeCheckExp(f.init)
		val namet_ = Table.getType(f.name)
		if (namet_.isEmpty) error("undefined field name")
		if (!Type.coerceTo(initt, namet_.get)) error("field type missmatch")
		VoidT()
	}
}
