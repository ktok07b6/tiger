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
			val r:Record = t.actual().asInstanceOf[Record]
			//TODO:find v.sym in r
			v.varEntry = ve
			r.fieldType
		}
		case v:SubscriptVar => {
			val ve:VarEntry = Table.getVarEntry(v.getSymbol).get
			val expt:Type = typeCheck(v.exp)
			require(expt.actual().isInstanceOf[Int])
			val vat:Type = typeCheck(v.va)
			val arrayT:Array = vat.actual().asInstanceOf[Array]
			v.varEntry = ve
			arrayT.element
		}
		case e:VarExp => {
			typeCheck(e.va)
		}
		case e:NilExp => {
			Nil()
		}
		case e:IntExp => {
			Int()
		}
		case e:StringExp => {
			Str()
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
				case i:Int => i
				case s:Str => s
				case _   => error("invalid operand type")
			}
		}

		case e:RecordExp => {
			val t = Table.getType(e.typ).get
			val rt = t.actual().asInstanceOf[Record]
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
			Void()
		}
		case e:IfExp => {
			val testt = typeCheck(e.test)
			require(testt.isInstanceOf[Int])
			val thent = typeCheck(e.thenexp)
			//TODO
			val elset = typeCheck(e.elseexp)
			Void()
		}
		case e:WhileExp => {
			val testt = typeCheck(e.test)
			require(testt.isInstanceOf[Int])
			typeCheck(e.body)
			//TODO
			Void()
		}
		case e:ForExp => {
			Table.beginScope
			val vart = Name(e.va)
			vart.bind(Table.getType(SymbolTable.symbol("int")).get)
			val ve = new VarEntry(vart)
			Table.putVarEntry(e.va, ve)
			e.varEntry = ve

			require(typeCheck(e.lo).actual.isInstanceOf[Int])
			require(typeCheck(e.hi).actual.isInstanceOf[Int])

			typeCheck(e.body)
			Table.endScope
			Void()
		}
		case e:BreakExp => {
			Void()
		}
		case e:LetExp => {
			Table.beginScope

			for(dec <- e.decs) {
				require(typeCheck(dec).actual.isInstanceOf[Void])
			}

			val bodyt = typeCheck(e.body)
			Table.endScope
			bodyt.actual
		}
		case e:ArrayExp => {
			val t = Table.getType(e.typ).get
			val arrt = t.actual.asInstanceOf[Array]

			require(typeCheck(e.size).actual.isInstanceOf[Int])
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
			Void()
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
					if (initt.actual.isInstanceOf[Nil]) {
						error("initializing nil expressions not constrained by record type");
					}
					Table.putVarEntry(d.name, ve)
					d.varEntry = ve
				}
			}
			Void()
		}
		case d:TypeDec => {
			val t = typeCheck(d.typ)
			t match {
				case Record(na, ty, tail) => ()//TODO 
				case _ => ()
			} 
			Table.putType(d.name, t)
			Void()
		}
		case t:NameTy => {
			val namet = Name(t.name)
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
				case None    => Name(f.typ)
			}
		}
		case f:RecordField => {
			val initt = typeCheck(f.init)
			val namet = Table.getType(f.name).get
			require(Type.coerceTo(initt, namet))
			Void()
		}
		case _ => null
	}
}
