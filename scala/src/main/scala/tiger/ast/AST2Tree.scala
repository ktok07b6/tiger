package tiger.ast

import tiger.tree._
import tiger.common._
import tiger.frame.Frame
import tiger.typ._

object AST2Tree {
	var currentFuncName:Symbol = 'nil
	var currentLoopExit:Label = null
	var fragments:List[Fragment] = List.empty

	def ast2tree(ast:ASTExp, frame:Frame):List[Fragment] = {
		fragments = List.empty
		Level.newTopLevel(frame)
		val tr = translateExp(ast)
		val stm = tr.unNx.get
		val global_body = frame.procEntryExit1(stm)
		procEntryExit(Nx(global_body))
		require(Level.levels.isEmpty)
		fragments
	}

	private def translateVar(ast:ASTVar):Translate = ast match {
		case v:SimpleVar => {
			require(v.varEntry.access != null)
			Ex(v.varEntry.access.simpleVar(Level.current))
		}

		case v:FieldVar => {
			val tr = translateVar(v.va)
			val rect = v.varEntry.typ match {
				case r:RecordT => r
				case a:ArrayT => a.element.asInstanceOf[RecordT]
			}
			val index = rect.elems.map(_.fieldName).indexOf(v.field)
			val head = tr.unEx.get
			val offset = index * wordSize
			val member = TreeMem(head + offset)
			Ex(member)
		}

		case v:SubscriptVar => {
			val head = translateVar(v.va).unEx.get
			val index = translateExp(v.exp).unEx.get

			val offset:TreeExp = index match {
				case TreeConst(i) => {
					i * wordSize
				}
				case _ => {
					index * wordSize
				}
			}
			Ex(TreeMem(head + offset))
		}
	}

	private def translateExp(ast:ASTExp):Translate = ast match {
		case e:VarExp => {
			translateVar(e.va)
		}

		case e:NilExp => {
			Ex(0)
		}

		case e:IntExp => {
			Ex(e.n)
		}

		case e:StringExp => {
			val alloc = callAlloc(wordSize + e.s.length)
			val pstr = TreeTemp(new Temp())
			/*
			 TODO: move strcpy to Frame impl
			 strcopy(pstr->chars, [address of string], strSize);
			 */
			val lab = new Label()
			val name = TreeName(lab)
			val str_copy = frame.externalCall("strcpy", 
											  List(pstr + wordSize, name))
			//add DataFragment
			val str_asm = frame.string(lab, e.s);
			fragments = fragments :+ DataFragment(str_asm)
			val seq = TreeSeq.makeSeq(pstr := alloc, 
									  TreeMem(pstr) := e.s.length, 
									  TreeExpr(str_copy))
			Ex(TreeEseq(seq, pstr))
		}

		case e:CallExp => {
			//FIXME
			val args:List[TreeExp] = 
				if (currentFuncName == e.func) {
					//In case of the recusive call, to pass the static link as first argument.
					val fp = TreeTemp(frame.fp);
					val sl = frame.staticChain(fp)
					sl :: e.exps.map(translateExp).map(_.unEx.get)
				} else if (!isBuiltinFunc(e.func)) {
					//To pass the frame pointer as first argument.
					val fp = TreeTemp(frame.fp)
					fp :: e.exps.map(translateExp).map(_.unEx.get)
				} else {
					e.exps.map(translateExp).map(_.unEx.get)
				}
			val func = TreeName(new Label(e.func))
			val call = TreeCall(func, args)
			e.funcEntry.result match {
				case VoidT() => Nx(TreeExpr(call))
				case _       =>	Ex(call)
			}
		}

		case e:OpExp => {
			val el = translateExp(e.l).unEx.get
			val er = translateExp(e.r).unEx.get

			if (e.lt.actual.isInstanceOf[StrT]) {
				return Ex(convertStrOp(e.op, el, er))
			}

			e.op match {
				case Oper.Plus | Oper.Minus | Oper.Times | Oper.Divide
					=> Ex(TreeBinOp(e.op, el, er))
				case Oper.Eq | Oper.Ne | Oper.Lt | Oper.Le | Oper.Gt | Oper.Ge
					=> Cx(e.op, el, er)
				case Oper.And 
					=> Ex(convertAndOp(el, er))
				case Oper.Or  
					=> Ex(convertOrOp(el, er))
			}
		}

		case e:RecordExp => {
			//base = malloc fieldsize * wordsize
			val t = new Temp()
			val base = TreeTemp(t)
			val addr = callAlloc(e.fields.length * wordSize)
			val mv = base := addr

			var i = 0
			def fieldInit(e:TreeExp):TreeStm = {
				val offset = i * wordSize
				i += 1
				TreeMem(base + offset) := e
			}
			val recFields = e.fields.map(translateRecordField).map(_.unEx.get)
			val fieldInits = recFields.map(fieldInit)

			val eseq = TreeEseq(TreeSeq.makeSeq((mv :: fieldInits): _*), base);
			Ex(eseq)
		}

		case e:SeqExp => {
			if (e.seq.length < 1) {
				return Ex(0)
			}
			val trs = e.seq.map(translateExp)
			val tr = trs.last
			tr.unEx match {
				case Some(ret) => {
					val seqs = trs.dropRight(1).map(_.unNx.get)
					val eseq = TreeEseq(TreeSeq.makeSeq(seqs:_*), ret)
					Ex(eseq)
				}
				case None => {
					val seq = TreeSeq.makeSeq(trs.last.unNx.get)
					Nx(seq)
				}
			}
		}

		case e:AssignExp => {
			val va = translateVar(e.va).unEx.get
			val exp = translateExp(e.exp).unEx.get
			Nx(va := exp)
		}

		case e:IfExp => {
		
			val l_t = new Label()
			val l_f = new Label()
			
			val tr = translateExp(e.test)
			
			val cjump = 
				if (tr.isInstanceOf[Cx]) {
					tr.unCx(l_t, l_f).get
				} else {
					val cond = tr.unEx.get
					(cond != TreeConst(0)) ? (l_t, l_f)
				}

			if (e.elseexp.isDefined) {
				val r = TreeTemp(new Temp())
				val thentr = translateExp(e.thenexp)
				val r_t = thentr.unEx match {
					case Some(rt) => r := rt
					case None => tr.unNx.get
				}

				val elsetr = translateExp(e.elseexp.get)
				val r_f = elsetr.unEx match {
					case Some(rt) => r := rt
					case None => tr.unNx.get
				}
				
				val l_j = new Label()
				val jmp_to_Join = TreeJump(0, List(l_j))

				val seq = TreeSeq.makeSeq(cjump,
										  L(l_t),
										  r_t,
										  jmp_to_Join,
										  L(l_f),
										  r_f,
										  L(l_j))
				val eseq = TreeEseq(seq, r)
				Ex(eseq)
			} else {
				val stm_t =	translateExp(e.thenexp).unNx.get
				val seq = TreeSeq.makeSeq(cjump,
										  L(l_t),
										  stm_t,
										  L(l_f))
				Nx(seq)
			}
		}

		case e:WhileExp => {
			/*
			 if test then jmp start
             else jmp end
			 start:
			 body
			 if test then jmp start
             else jmp end
			 end:
			 */

			val l_s = new Label()
			val l_e = new Label()
	
			val tr = translateExp(e.test)

			val cjump = 
				if (tr.isInstanceOf[Cx]) {
					tr.unCx(l_s, l_e).get
				} else {
					val cond = tr.unEx.get
					(cond != TreeConst(0)) ? (l_s, l_e)
				}

			val oldLoopExit = currentLoopExit
			currentLoopExit = l_e
	
			val body = translateExp(e.body).unNx.get
			currentLoopExit = oldLoopExit

			val seq = TreeSeq.makeSeq(cjump, 
									  L(l_s), 
									  body, 
									  cjump, 
									  L(l_e))
			Nx(seq)
		}

		case e:ForExp => {

			//TODO:
			//LetExpへの変換
			/*
			 i = lo
			 if i < hi then jmp start
             else jmp end
			 start:
			 body
			 i = i+1
			 if i < hi then jmp start 
             else jmp end
			 end:
			 */

			val access = Level.current.allocLocal(e.escape)
			e.varEntry.access = access
			val va = access.simpleVar(Level.current)
			val l_s = new Label()
			val l_e = new Label()

			val lo = translateExp(e.lo).unEx.get
			val hi = translateExp(e.hi).unEx.get
			currentLoopExit = l_e

			val body = translateExp(e.body).unNx.get
			val cmp = (va < hi) ? (l_s, l_e)
			val seq = TreeSeq.makeSeq(va := lo, 
									  cmp, 
									  L(l_s), 
									  body, 
									  va := va + 1, 
									  cmp, 
									  L(l_e))
			Nx(seq)
		}

		case e:BreakExp => {
			Nx(TreeJump(0, List(currentLoopExit)))
		}

		case e:LetExp => {
			//TODO:
			val var_decs = e.decs.filter(_.isInstanceOf[VarDec]).map(translateDec).map(_.unNx.get)
			e.decs.filter(_.isInstanceOf[FunDec]).foreach(translateDec)

			val tr = translateExp(e.body)
			tr.unEx match {
				case Some(ebody) => {
					if (!var_decs.isEmpty) {
						Ex(TreeEseq(TreeSeq.makeSeq(var_decs:_*), ebody))
					} else {
						Ex(ebody)
					}
				}
				case None => {
					val nbody = tr.unNx.get
					if (!var_decs.isEmpty) {
						Nx(TreeSeq(TreeSeq.makeSeq(var_decs:_*), nbody))
					} else {
						Nx(nbody)
					}
				}
			}
		}

		case e:ArrayExp => {
			val size = translateExp(e.size).unEx.get

			e.init_t.actual  match {
				case IntT() => {
					val init = translateExp(e.init).unEx.get
					val addr = frame.externalCall("initArray", List(size, init))
					Ex(addr)
				}
				case StrT() | ArrayT(_,_) | RecordT(_,_) => {
					val init = translateExp(e.init).unEx.get						   
					val memsz = size * wordSize
					val addr = frame.externalCall("alloc", List(memsz))
					
					val ptr = TreeTemp(new Temp())
					val va = TreeTemp(new Temp())
					val l_s = new Label()
					val l_e = new Label()
					val offset = va * wordSize
					val mem = TreeMem(ptr + offset)
					val cmp = (va < size) ? (l_s, l_e)
					val seq = TreeSeq.makeSeq(ptr := addr,
											  va := 0,
											  cmp,
											  L(l_s),
											  mem := init,
											  va := va + 1,
											  cmp,
											  L(l_e))
					
					val eseq = TreeEseq(seq, ptr)
					Ex(eseq)
				} 
				case _ => {
					require(false)
					Ex(0)
				}
			}
		}
	}

	private def translateDec(ast:ASTDec):Translate = ast match {
		case d:FunDec => {
			//static chainのため1つ余分に引数を割り当てる
			//FIXME
			val paramEscapes = d.params.map(_.escape)
			Level.newLevel(d.name, true :: paramEscapes)
			
			for ((field, acc) <- d.params zip Level.current.formals) {
				println("acc = " + acc)
				field.varEntry.access = acc
			}

			val parentFuncName = currentFuncName
			currentFuncName = d.name

			val tr = translateExp(d.body)

			currentFuncName = parentFuncName;

			val rv = TreeTemp(frame.rv)
			val seq = tr.unEx match {
				case Some(eseq:TreeEseq) => {
					//TODO: To find more simple way!
/*
					while (eseq->exp->isESEQ_T()) {
						eseq = (tree::ESEQ*)eseq->exp;
					}
*/
					val mv_return_value = rv := eseq.exp
					TreeSeq.makeSeq(eseq.stm, mv_return_value)
				}
				case Some(e) => {
					val mv_return_value = rv := e
					TreeSeq.makeSeq(mv_return_value)
				}
				case None => {
					//procedure
					TreeSeq.makeSeq(tr.unNx.get)
				}
			}
			val body = frame.procEntryExit1(seq)
			val nx = Nx(body)
			procEntryExit(nx)
			nx
		}

		case d:VarDec => {
			val acc = Level.current.allocLocal(d.escape)
			d.varEntry.access = acc
			val va = acc.simpleVar(Level.current)
			val init = translateExp(d.init).unEx.get
			Nx(va := init)
		}
		case d:TypeDec => {
			//TODO:
			Ex(0)
		}
	}

	private def translateRecordField(ast:RecordField):Translate = {
		translateExp(ast.init)
	}

	private def callAlloc(size:Int):TreeExp = {
		frame.externalCall("alloc", List(TreeConst(size)))
	}

	private def isBuiltinFunc(f:Symbol):Boolean = List('print, 'flush, 'ord, 'chr, 
											   'stringEqual, 'stringLen, 'substring, 'stringConcat, 
											   'not, 'getch, 'test).contains(f)



	private def convertStrOp(op:Oper.Value, el:TreeExp, er:TreeExp):TreeExp = {
		val args = List(el, er)
		println(op)
		op match {
			case Oper.Plus => frame.externalCall("stringConcat", args)
			case Oper.Eq => frame.externalCall("stringEqual", args)
			case Oper.Ne => {
				val streq = frame.externalCall("stringEqual", args)
				frame.externalCall("not", List(streq))
			}
		}
	}

	private def convertAndOp(el:TreeExp, er:TreeExp):TreeExp = {
		/*
		 r = 0
		 if el != 0 then L1 else done
		 L1:
		 if er != 0 then L2 else done
		 L2:
		 r = 1
		 done:
		 return r
		 */			
		val ret = TreeTemp(new Temp())
		val l1 = new Label()
		val l2 = new Label()
		val done = new Label()
		val const0 = TreeConst(0)
		val seq = TreeSeq.makeSeq(ret := 0,
								  (el != const0) ? (l1, done),
								  L(l1),
								  (er != const0) ? (l2, done),
								  L(l2),
								  ret := 1,
								  L(done))
		TreeEseq(seq, ret)
	}


	def convertOrOp(el:TreeExp, er:TreeExp):TreeExp = {
		val ret = TreeTemp(new Temp())
		val l1 = new Label()
		val l2 = new Label()
		val done = new Label()
		val const0 = TreeConst(0)
		val seq = TreeSeq.makeSeq(ret := 1,
								  (el == const0) ? (l1, done),
								  L(l1),
								  (er == const0) ? (l2, done),
								  L(l2),
								  ret := 0,
								  L(done))
		TreeEseq(seq, ret)
	}

	private def procEntryExit(tr:Translate) = {
		val stm = tr.unNx.get
		fragments = fragments :+ ProcFragment(stm, frame)
		Level.deleteLevel()
	}
	
	private def frame():Frame = Level.current.frame
	private def wordSize() = frame.wordSize
}
