package tiger.ast

import tiger.tree._
import tiger.common._
import tiger.frame.Frame
import tiger.typ._

object AST2Tree {
	var currentFuncName:Symbol = 'nil
	var currentLoopExit:Label = null

	def ast2tree(ast:ASTExp, frame:Frame):TreeStm = {
		Level.newTopLevel(frame)
		val tr = translateExp(ast)
		tr.unNx.get
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
			val name = TreeName(new Label())
			val str_copy = frame.externalCall("strcpy", 
											  List(pstr + wordSize, name))
			//TODO:add DataFragment
			//val str_asm = frame.string(lab, exp.s);
			//fragments = DataFragment(str_asm)::fragments
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
		
			val labelT = new Label()
			val labelF = new Label()
			val l_t = TreeLabel(labelT)
			val l_f = TreeLabel(labelF)
			
			val tr = translateExp(e.test)
			
			val cjump = 
				if (tr.isInstanceOf[Cx]) {
					tr.unCx(labelT, labelF).get
				} else {
					val boolean = tr.unEx.get
					TreeCjump(Oper.Ne, boolean, 0, labelT, labelF)
				}

			if (e.elseexp.isDefined) {
				val r = TreeTemp(new Temp())
				//tree::Stm *r_T;
				//tree::Stm *r_F;
				
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
				
				val labelJoin = new Label()
				val l_Join = TreeLabel(labelJoin)
				val jmp_to_Join = TreeJump(labelJoin)

				val seq = TreeSeq.makeSeq(cjump,
										  l_t,
										  r_t,
										  jmp_to_Join,
										  l_f,
										  r_f,
										  l_Join)
				val eseq = TreeEseq(seq, r)
				Ex(eseq)
			} else {
				val stm_t =	translateExp(e.thenexp).unNx.get
				val seq = TreeSeq.makeSeq(cjump,
										  l_t,
										  stm_t,
										  l_f)
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

			val labelS = new Label()
			val labelE = new Label()
	
			val tr = translateExp(e.test)

			val cjump = 
				if (tr.isInstanceOf[Cx]) {
					tr.unCx(labelS, labelE).get
				} else {
					val boolean = tr.unEx.get
					TreeCjump(Oper.Ne, boolean, 0, labelS, labelE)
				}

			val oldLoopExit = currentLoopExit
			currentLoopExit = labelE
	
			val body = translateExp(e.body).unNx.get
			currentLoopExit = oldLoopExit

			val l_s = TreeLabel(labelS)
			val l_e = TreeLabel(labelE)
			val seq = TreeSeq.makeSeq(cjump, l_s, body, cjump, l_e)
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
			val labelS = new Label()
			val labelE = new Label()

			val lo = translateExp(e.lo).unEx.get
			val hi = translateExp(e.hi).unEx.get
			currentLoopExit = labelE

			val body = translateExp(e.body).unNx.get
			val cmp = TreeCjump(Oper.Lt, va, hi, labelS, labelE)
			val seq = TreeSeq.makeSeq(va := lo, 
									  cmp, 
									  TreeLabel(labelS), 
									  body, 
									  va := va + 1, 
									  cmp, 
									  TreeLabel(labelE))
			Nx(seq)
		}

		case e:BreakExp => {
			Nx(TreeJump(currentLoopExit))
		}

		case e:LetExp => {
			//TODO:
			val decs = e.decs.map(translateDec).map(_.unNx.get)
			val decs_except_func = decs.filter(!_.isInstanceOf[FunDec])

			val tr = translateExp(e.body)
			tr.unEx match {
				case Some(ebody) => {
					if (!decs_except_func.isEmpty) {
						Ex(TreeEseq(TreeSeq.makeSeq(decs_except_func:_*), ebody))
					} else {
						Ex(ebody)
					}
				}
				case None => {
					val nbody = tr.unNx.get
					Nx(TreeSeq(nbody, TreeSeq.makeSeq(decs_except_func:_*)))
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
					val labelS = new Label()
					val labelE = new Label()
					val offset = va * wordSize
					val mem = TreeMem(ptr + offset)
					val cmp = TreeCjump(Oper.Lt, va, size, labelS, labelE)
					val seq = TreeSeq.makeSeq(ptr := addr,
											  va := 0,
											  cmp,
											  TreeLabel(labelS),
											  mem := init,
											  va := va + 1,
											  cmp,
											  TreeLabel(labelE))
					
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
		val cj1 = TreeCjump(Oper.Ne, el, 0, l1, done)
		val cj2 = TreeCjump(Oper.Ne, er, 0, l2, done)
		val seq = TreeSeq.makeSeq(ret := 0,
								  cj1,
								  TreeLabel(l1),
								  cj2,
								  TreeLabel(l2),
								  ret := 1,
								  TreeLabel(done))
		TreeEseq(seq, ret)
	}


	def convertOrOp(el:TreeExp, er:TreeExp):TreeExp = {
		/*
		 t1 = 1
		 if el = 0 then L1 else done
		 L1:
		 if er = 0 then L2 else done
		 L2:
		 t1 = 0
		 done:
		 return t1
		 */			
		val ret = TreeTemp(new Temp())
		val l1 = new Label()
		val l2 = new Label()
		val done = new Label()
		val cj1 = TreeCjump(Oper.Eq, el, 0, l1, done)
		val cj2 = TreeCjump(Oper.Eq, er, 0, l2, done)
		val seq = TreeSeq.makeSeq(ret := 1,
								  cj1,
								  TreeLabel(l1),
								  cj2,
								  TreeLabel(l2),
								  ret := 0,
								  TreeLabel(done))
		TreeEseq(seq, ret)
	}

	private def procEntryExit(tr:Translate) = {
		//TODO:
/*
		val stm = tr.unNx
		val frag = new ProcFragment(stm, frame)
		fragments = fragments ++ frag
		currentLevel = currentLevel->parent;	
*/
	}
	
	private def frame():Frame = Level.current.frame
	private def wordSize() = frame.wordSize
}
