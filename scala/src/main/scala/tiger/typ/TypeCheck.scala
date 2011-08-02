package tiger.typ;

import tiger.ast._
import tiger.symbol._

object TypeCheck {
	def typeCheck(ast:AST):Type = ast match {
		case v:SimpleVar => {
			val ve:VarEntry = Table.getVarEntry(v.sym).get
			v.varEntry = ve
			ve.typ
		}
		case v:FieldVar => {
			val ve:VarEntry = Table.getVarEntry(v.sym).get
			val t:Type = typeCheck(v.va)
			val r:RecordT = t.actual().asInstanceOf[RecordT]
			//TODO:find v.sym in r
			v.varEntry = ve
			r.fieldType
		}
		case v:SubscriptVar => {
			val ve:VarEntry = Table.getVarEntry(v.getSymbol).get
			val expt:Type = typeCheck(v.exp)
			require(expt.actual().isInstanceOf[IntT])
			val vat:Type = typeCheck(v.va)
			val arrayT:ArrayT = vat.actual().asInstanceOf[ArrayT]
			v.varEntry = ve
			arrayT.element
		}
		case e:VarExp => {
			typeCheck(e.va)
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
			val fe:FuncEntry = Table.getFuncEntry(e.func).get
			require(e.exps.length == fe.params.length)
			for ((a,t) <- e.exps zip fe.params) {
				require(Type.coerceTo(typeCheck(a), t))
			}
			e.funcEntry = fe
 			fe.result
		}
		case e:OpExp => {
			val lt = typeCheck(e.l)
			val rt = typeCheck(e.r)
			require(Type.coerceTo(lt, rt))
			lt match {
				case i:IntT => i
				case s:StrT => s
				case _   => error("invalid operand type")
			}
		}

		case e:RecordExp => {
			val t = Table.getType(e.typ).get
			val rt = t.actual().asInstanceOf[RecordT]
			//TODO
			null
		}
		case e:SeqExp => {
			val results = for (exp <- e.seq) yield {
				typeCheck(exp)
			}
			results.last
		}
		case e:AssignExp => {
			val dst = typeCheck(e.va)
			val src = typeCheck(e.exp)
			require(Type.coerceTo(dst, src))
			VoidT()
		}
		case e:IfExp => {
			val testt = typeCheck(e.test)
			require(testt.isInstanceOf[IntT])
			val thent = typeCheck(e.thenexp)
			//TODO
			val elset = typeCheck(e.elseexp)
			VoidT()
		}
		case e:WhileExp => {
			val testt = typeCheck(e.test)
			require(testt.isInstanceOf[IntT])
			typeCheck(e.body)
			//TODO
			VoidT()
		}
		case e:ForExp => {
			Table.beginScope
			val vart = NameT(e.va)
			vart.bind(Table.getType('int).get)
			val ve = VarEntry(vart)
			Table.putVarEntry(e.va, ve)
			e.varEntry = ve

			require(typeCheck(e.lo).actual.isInstanceOf[IntT])
			require(typeCheck(e.hi).actual.isInstanceOf[IntT])

			typeCheck(e.body)
			Table.endScope
			VoidT()
		}
		case e:BreakExp => {
			VoidT()
		}
		case e:LetExp => {
			Table.beginScope

			for(dec <- e.decs) {
				require(typeCheck(dec).actual.isInstanceOf[VoidT])
			}

			val bodyt = e.body.map(typeCheck).last
			Table.endScope
			bodyt.actual
		}
		case e:ArrayExp => {
			val t = Table.getType(e.typ).get
			val arrt = t.actual.asInstanceOf[ArrayT]

			require(typeCheck(e.size).actual.isInstanceOf[IntT])
			val initt = typeCheck(e.init)
			require(Type.coerceTo(initt, arrt.element))
			arrt
		}
		case d:FunDec => {
			val params = for (p <- d.params) yield {
				typeCheck(p)
			}
			val result = Table.getType(d.result).get
			Table.putFuncEntry(d.name, FuncEntry(result, params))
			VoidT()
		}
		case d:VarDec => {
			val opt = Table.getType(d.typ)
			opt match {
				case Some(t) => {
					val ve = VarEntry(t)
					Table.putVarEntry(d.name, ve)
					val initt = typeCheck(d.init)
					if (!Type.coerceTo(t, initt)) {
						error("var initialize exp type mismatch")
					}
					d.varEntry = ve
				}
				case None => {
					val initt = typeCheck(d.init)
					val ve = VarEntry(initt.actual)
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
			val t = typeCheck(d.typ)
			t match {
				case RecordT(na, ty, tail) => ()//TODO 
				case _ => ()
			} 
			Table.putType(d.name, t)
			VoidT()
		}
		case t:NameTy => {
			val namet = NameT(t.name)
			val opt = Table.getType(t.name)
			opt match {
				case Some(t) => namet.bind(t)
				case None => error("unknown type name")
			}
			namet
		}
		case t:RecordTy => {
			null
		}
		case t:ArrayTy => {
			null
		}
		case f:TypeField => {
			val opt = Table.getType(f.typ)
			opt match {
				case Some(t) => t
				case None    => NameT(f.typ)
			}
		}
		case f:RecordField => {
			val initt = typeCheck(f.init)
			val namet = Table.getType(f.name).get
			require(Type.coerceTo(initt, namet))
			VoidT()
		}
		case _ => null
	}
}
