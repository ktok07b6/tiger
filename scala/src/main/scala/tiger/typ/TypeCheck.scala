package tiger.typ;

import tiger.ast._
import tiger.symbol._

object TypeCheck {
	Table.putType('int, IntT())
	Table.putType('string, StrT())

	def typeCheck(ast:AST):Boolean = {
		try {
			ast match {
				case exp:ASTExp =>	typeCheckExp(exp) isDefined
				case _ => error("ASTExp is expected"); false
			}
		} catch {
			case e => println(e); println(e.getStackTraceString)
			false
		}
	}	
	def typeCheckVar(ast:ASTVar):Option[Type] = ast match {
		case v:SimpleVar => {
			val ve_ = Table.getVarEntry(v.getSymbol)
			if (ve_.isEmpty) {
				error("undefined symbol:" + v.getSymbol)
			}
			val ve = ve_.get
			v.varEntry = ve
			Some(ve.typ)
		}

		case v:FieldVar => {
			//FIXME: nested too deep
			val ve_ = Table.getVarEntry(v.getSymbol())
			ve_ match {
				case None => {
					error("undefined symbol:" + v.getSymbol)
					None
				}
				case Some(ve) => {
					val ty_ = typeCheckVar(v.va)
					ty_ match {
						case None => None
						case Some(ty) => {
							ty.actual() match {
								case r:RecordT => {
									v.varEntry = ve
									r.getType(v.sym)
								}
								case _ => error("Record type is expected"); None
							}
						}
					}
				}
			}
		}

		case v:SubscriptVar => {
			val ve_ = Table.getVarEntry(v.getSymbol())
			ve_ match {
				case None => error("undefined symbol:" + v.getSymbol()); None
				case Some(ve) => {
					val expt_ = typeCheckExp(v.exp)
					expt_ match {
						case None => None
						case Some(expt) => {
							require(expt.actual().isInstanceOf[IntT])
							val vat_ = typeCheckVar(v.va)
							vat_ match {
								case None => None
								case Some(vat) => {
									val arrayT:ArrayT = vat.actual().asInstanceOf[ArrayT]
									v.varEntry = ve
									Some(arrayT.element)
								}
							}
						}
					}
				}
			}
		}
	}

	def typeCheckExp(ast:ASTExp):Option[Type] = ast match {
		case e:VarExp => {
			typeCheckVar(e.va)
		}
		case e:NilExp => {
			Some(NilT())
		}
		case e:IntExp => {
			Some(IntT())
		}
		case e:StringExp => {
			Some(StrT())
		}
		case e:CallExp => {
			val fe_ = Table.getFuncEntry(e.func)
			if (fe_.isEmpty) {
				error("undefined function:" + e.func)
			}
			val fe = fe_.get
			require(e.exps.length == fe.params.length)
			for ((a,t) <- e.exps zip fe.params) {
				require(Type.coerceTo(typeCheckExp(a), Some(t)))
			}
			e.funcEntry = fe
 			Some(fe.result)
		}
		case e:OpExp => {
			println("op, lt, rt = " + e.op + "," + e.l + "," + e.r)
			val lt_ = typeCheckExp(e.l)
			val rt_ = typeCheckExp(e.r)
			if (Type.coerceTo(lt_, rt_)) {
				lt_ match {
					case Some(i:IntT) => Some(i)
					case Some(s:StrT) => Some(s)
					case _ => error("invalid operand type"); None
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
				case None => error("unkown type:" + e.typ); None
				case Some(t) => {
					val rt = t.actual().asInstanceOf[RecordT]
					e.fields.foreach(checkRecordField)
					Some(t)
				}
			}
		}
		case e:SeqExp => {
			val results = for (exp <- e.seq) yield {
				typeCheckExp(exp)
			}
			println("SeqExp:" + results)
			println("results.last:" + results.last)
			results.last
		}
		case e:AssignExp => {
			val dst = typeCheckVar(e.va)
			val src = typeCheckExp(e.exp)
			require(Type.coerceTo(dst, src))
			Some(VoidT())
		}
		case e:IfExp => {
			val testt = typeCheckExp(e.test)
			require(testt.isInstanceOf[IntT])
			val thent = typeCheckExp(e.thenexp)
			//TODO
			val elset = typeCheckExp(e.elseexp)
			Some(VoidT())
		}
		case e:WhileExp => {
			val testt = typeCheckExp(e.test)
			require(testt.isInstanceOf[IntT])
			typeCheckExp(e.body)
			//TODO
			Some(VoidT())
		}
		case e:ForExp => {
			Table.beginScope
			val vart = NameT(e.va)
			vart.bind(Table.getType('int).get)
			val ve = VarEntry(vart)
			Table.putVarEntry(e.va, ve)
			e.varEntry = ve

			require(typeCheckExp(e.lo).get.actual.isInstanceOf[IntT])
			require(typeCheckExp(e.hi).get.actual.isInstanceOf[IntT])

			typeCheckExp(e.body)
			Table.endScope
			Some(VoidT())
		}
		case e:BreakExp => {
			Some(VoidT())
		}
		case e:LetExp => {
			Table.beginScope

			for(dec <- e.decs) {
				require(typeCheckDec(dec).get.actual.isInstanceOf[VoidT])
			}

			if (!e.body.isEmpty) {
				val bodyt_ = e.body.map(typeCheckExp).last
				Table.endScope
				bodyt_ match { 
					case Some(bodyt) => Some(bodyt.actual)
					case None => None
				}
			} else {
				Some(VoidT())
			}
		}
		case e:ArrayExp => {
			Table.getType(e.typ) match {
				case Some(t) => {
					println(t)
					val arrt = t.actual.asInstanceOf[ArrayT]
					require(typeCheckExp(e.size).get.actual.isInstanceOf[IntT])
					val initt_ = typeCheckExp(e.init)
					require(Type.coerceTo(initt_, Some(arrt.element)))
					Some(arrt)
				}
				case _ => error("unknown type:" + e.typ)
			}
		}
	}

	def typeCheckDec(ast:ASTDec):Option[Type] = ast match {
		case d:FunDec => {
			val params = d.params.map({p => typeCheckTypeField(p).get})
			val result_ = Table.getType(d.result)
			result_ match {
				case Some(result) => {
					Table.putFuncEntry(d.name, FuncEntry(result, params))
					Some(VoidT())
				}
				case None => None
			}
			//TODO: check body
		}

		case d:VarDec => {
			val vart_ = Table.getType(d.typ)
			vart_ match {
				case Some(vart) => {
					val ve = VarEntry(vart)
					Table.putVarEntry(d.name, ve)
					val initt_ = typeCheckExp(d.init)
					if (!Type.coerceTo(vart_, initt_)) {
						error("var initialize exp type mismatch")
					}
					d.varEntry = ve
				}
				case None => {
					val initt_ = typeCheckExp(d.init)
					initt_ match {
						case Some(initt) => {
							val ve = VarEntry(initt.actual)
							if (initt.actual.isInstanceOf[NilT]) {
								error("initializing nil expressions not constrained by record type");
							}
							Table.putVarEntry(d.name, ve)
							d.varEntry = ve
						}
						case None => None
					}
				}
			}
			Some(VoidT())
		}
		case d:TypeDec => {
			val t_ = typeCheckType(d.typ)
			t_ match {
				case Some(t) => {
					t match {
						case r:RecordT => Table.putType(d.name, r)//TODO 
						case t:Type => Table.putType(d.name, t)
					}
					Some(VoidT())
				}
				case None => None
			} 
		}
	}

	def typeCheckType(ast:ASTType):Option[Type] = ast match {
		case t:NameTy => {
			val namet = NameT(t.name)
			val t_ = Table.getType(t.name)
			t_ match {
				case Some(t) => namet.bind(t)
				case None => error("unknown type name")
			}
			Some(namet)
		}
		case t:RecordTy => {
			def recFieldTypeCheck(f:TypeField):RecordElem = {
				val ty_ = typeCheckTypeField(f)
				ty_ match {
					case None => error("unknown type name")
					case Some(ty) => {
						RecordElem(f.name, ty)
					}
				}
			}
			Some(RecordT(t.fields.map(recFieldTypeCheck)))
		}

		case t:ArrayTy => {
			val elemt_ = Table.getType(t.name)
			elemt_ match {
				case Some(t) => Some(ArrayT(t))
				case None => error("unknown type name:" + t.name); None
			}
		}
	}

	def typeCheckTypeField(f:TypeField):Option[Type] = {
		val opt = Table.getType(f.typ)
		opt match {
			case Some(t) => Some(t)
			case None    => Some(NameT(f.typ))
		}
	}

	def typeCheckRecordField(f:RecordField):Option[Type] = {
		val initt = typeCheckExp(f.init)
		val namet = Table.getType(f.name)
		require(Type.coerceTo(initt, namet))
		Some(VoidT())
	}
}
