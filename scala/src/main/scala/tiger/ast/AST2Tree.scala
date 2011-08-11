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
		tr.unNx().get
	}

	private def translateVar(ast:ASTVar):Translate = ast match {
		case v:SimpleVar => {
			require(v.varEntry.access != null)
			Ex(v.varEntry.access.simpleVar(Level.current()))
		}

		case v:FieldVar => {
			val tr = translateVar(v.va)
			val rect = v.varEntry.typ match {
				case r:RecordT => r
				case a:ArrayT => a.element.asInstanceOf[RecordT]
			}
			val index = rect.elems.map(_.fieldName).indexOf(v.field)
			val head = tr.unEx().get
			val offset = TreeConst(index*wordSize())
			val memberAddr = TreeBinOp(Oper.Plus, head, offset)
			val member = TreeMem(memberAddr)
			Ex(member)
		}

		case v:SubscriptVar => {
			val e1_ = translateVar(v.va).unEx()
			val head = e1_.get
			val e2_ = translateExp(v.exp).unEx()
			val index = e2_.get

			val offset = index match {
				case TreeConst(i) => {
					TreeConst(i*wordSize())
				}
				case _ => {
					TreeBinOp(Oper.Times, index, TreeConst(wordSize()))
				}
			}
			val addr = TreeBinOp(Oper.Plus, head, offset)
			val mem = TreeMem(addr)
			Ex(mem)
		}
	}

	private def translateExp(ast:ASTExp):Translate = ast match {
		case e:VarExp => {
			translateVar(e.va)
		}

		case e:NilExp => {
			Ex(TreeConst(0))
		}

		case e:IntExp => {
			Ex(TreeConst(e.n))
		}

		case e:StringExp => {
			val t = new Temp()
			val alloc = callAlloc(wordSize() + e.s.length())
			val pstr = TreeTemp(t)
			val mv = TreeMove(pstr, alloc)

			val mem = TreeMem(pstr)
			val len = TreeConst(e.s.length())
			val copy_str_len = TreeMove(mem, len);

			/*
			 TODO: move strcpy to Frame impl
			 strcopy(pstr->chars, [address of string], strSize);
			 */
			val lab = new Label()
			val name = TreeName(lab)

			val four = TreeConst(4)
			val pstr_plus_4 = TreeBinOp(Oper.Plus, pstr, four)
			val str_copy = frame().externalCall("strcpy", List(pstr_plus_4, name))
			val stm_str_copy = TreeExpr(str_copy)

			//TODO:add DataFragment
			//val str_asm = frame().string(lab, exp.s);
			//fragments = DataFragment(str_asm)::fragments
			val seq = TreeSeq.makeSeq(List[TreeStm](mv, copy_str_len, stm_str_copy))
			val eseq = TreeEseq(seq, pstr)
			Ex(eseq)
		}

		case e:CallExp => {
			//FIXME
			val args:List[TreeExp] = 
			if (currentFuncName == e.func) {
				//In case of the recusive call, to pass the static link as first argument.
				val fp = TreeTemp(frame().fp());
				val sl = frame().staticChain(fp)
				sl :: e.exps.map(translateExp).map(_.unEx().get)
			} else if (!isBuiltinFunc(e.func)) {
				//To pass the frame pointer as first argument.
				val fp = TreeTemp(frame().fp())
				fp :: e.exps.map(translateExp).map(_.unEx().get)
			} else {
				e.exps.map(translateExp).map(_.unEx().get)
			}
			val func = TreeName(new Label(e.func))
			val call = TreeCall(func, args)
			e.funcEntry.result match {
				case VoidT() => Nx(TreeExpr(call))
				case _       =>	Ex(call)
			}
		}

		case e:OpExp => {
			val el = translateExp(e.l).unEx().get
			val er = translateExp(e.r).unEx().get

			if (e.lt.actual().isInstanceOf[StrT]) {
				return Ex(convertStrOp(e.op, el, er))
			}

			e.op match {
				case Oper.Plus | Oper.Minus | Oper.Times | Oper.Divide
					=> Ex(TreeBinOp(e.op, el, er))
				case Oper.Eq | Oper.Ne | Oper.Lt | Oper.Le | Oper.Gt | Oper.Ge
					=> Cx(e.op, el, er)
				case Oper.And => Ex(convertAndOp(el, er))
				case Oper.Or  => Ex(convertOrOp(el, er))
			}
		}

		case e:RecordExp => {
			//base = malloc fieldsize * wordsize
			val t = new Temp()
			val base = TreeTemp(t)
			val ws = wordSize()
			val addr = callAlloc(e.fields.length * ws)
			val mv = TreeMove(base, addr)

			var i = 0
			def fieldInit(e:TreeExp):TreeStm = {
				val offset = TreeConst(i*ws)
				i += 1
				val p = TreeBinOp(Oper.Plus, base, offset)
				val m = TreeMem(p)
				TreeMove(m, e)
			}
			val recFields = e.fields.map(translateRecordField).map(_.unEx().get)
			val fieldInits = recFields.map(fieldInit)

			val eseq = TreeEseq(TreeSeq.makeSeq(mv :: fieldInits), base);
			Ex(eseq)
		}

		case e:SeqExp => {
			require(e.seq.length >= 2)

			val trs = e.seq.map(translateExp)
			trs.last.unEx() match {
				case Some(ret) => {
					val seqs = trs.dropRight(1).map(_.unNx().get)
					val eseq = TreeEseq(TreeSeq.makeSeq(seqs), ret)
					Ex(eseq)
				}
				case None => {
					val seq = TreeSeq.makeSeq(List(trs.last.unNx().get))
					Nx(seq)
				}
			}
		}

		case e:AssignExp => {
			val va = translateVar(e.va).unEx().get
			val exp = translateExp(e.exp).unEx().get
			Nx(TreeMove(va, exp))
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
					val boolean = tr.unEx().get
					val const_0 = TreeConst(0)
					TreeCjump(Oper.Ne, boolean, const_0, labelT, labelF)
				}

			if (e.elseexp.isDefined) {
				val r = new Temp()
				//tree::Stm *r_T;
				//tree::Stm *r_F;
				
				val thentr = translateExp(e.thenexp)
				val r_t = thentr.unEx() match {
					case Some(rt) => TreeMove(TreeTemp(r), rt)
					case None => tr.unNx().get
				}

				val elsetr = translateExp(e.elseexp.get)
				val r_f = elsetr.unEx() match {
					case Some(rt) => TreeMove(TreeTemp(r), rt)
					case None => tr.unNx().get
				}
				
				val labelJoin = new Label()
				val l_Join = TreeLabel(labelJoin)
				val jmp_to_Join = TreeJump(labelJoin)

				val seq = TreeSeq.makeSeq(List[TreeStm](cjump,
														l_t,
														r_t,
														jmp_to_Join,
														l_f,
														r_f,
														l_Join))
				val eseq = TreeEseq(seq, TreeTemp(r))
				Ex(eseq)
			} else {
				val stm_t =	translateExp(e.thenexp).unNx().get
				val seq = TreeSeq.makeSeq(List[TreeStm](cjump,
														l_t,
														stm_t,
														l_f))
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
					val boolean = tr.unEx().get
					val const_0 = TreeConst(0)
					TreeCjump(Oper.Ne, boolean, const_0, labelS, labelE)
				}

			val oldLoopExit = currentLoopExit
			currentLoopExit = labelE
	
			val body = translateExp(e.body).unNx().get
			currentLoopExit = oldLoopExit

			val l_s = TreeLabel(labelS)
			val l_e = TreeLabel(labelE)
			val seq = TreeSeq.makeSeq(List[TreeStm](cjump, l_s, body, cjump, l_e))
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

			val access = Level.current().allocLocal(e.escape)
			e.varEntry.access = access
			val va = access.simpleVar(Level.current())
			val labelS = new Label()
			val labelE = new Label()

			val lo = translateExp(e.lo).unEx().get
			val hi = translateExp(e.hi).unEx().get
			currentLoopExit = labelE

			val body = translateExp(e.body).unNx().get

			val l_S = TreeLabel(labelS)
			val l_E = TreeLabel(labelE)
			val var_init = TreeMove(va, lo)
			val const_1 = TreeConst(1)
			val var_inc = TreeBinOp(Oper.Plus, va, const_1)
			val var_update = TreeMove(va, var_inc)
			val cmp = TreeCjump(Oper.Lt, va, hi, labelS, labelE)
			val seq = TreeSeq.makeSeq(List[TreeStm](var_init, cmp, l_S, body, var_update, cmp, l_E))
			Nx(seq)
		}

		case e:BreakExp => {
			Nx(TreeJump(currentLoopExit))
		}

		case e:LetExp => {
			//TODO:
			val decs = e.decs.map(translateDec).map(_.unNx().get)
			val decs_except_func = decs.filter(!_.isInstanceOf[FunDec])
 			val ebody = translateExp(e.body).unEx().get
			if (!decs_except_func.isEmpty) {
				Ex(TreeEseq(TreeSeq.makeSeq(decs_except_func), ebody))
			} else {
				Ex(ebody)
			}
		}

		case e:ArrayExp => {
			val size = translateExp(e.size).unEx().get

			e.init_t.actual  match {
				case IntT() => {
					val init = translateExp(e.init).unEx().get
					val addr = frame().externalCall("initArray", List(size, init))
					Ex(addr)
				}
				case StrT() | ArrayT(_,_) | RecordT(_,_) => {
					val init = translateExp(e.init).unEx().get						   
					val memsz = TreeBinOp(Oper.Times, size, TreeConst(wordSize()))
					val addr = frame().externalCall("alloc", List(memsz))
					
					val ptr = TreeTemp(new Temp())
					val ptr_init = TreeMove(ptr, addr)
					val va = TreeTemp(new Temp())
					val labelS = new Label()
					val labelE = new Label()
					val l_S = TreeLabel(labelS)
					val l_E = TreeLabel(labelE)
					val var_init = TreeMove(va, TreeConst(0))
					val offset = TreeBinOp(Oper.Times, va, TreeConst(wordSize))
					val mem = TreeMem(TreeBinOp(Oper.Plus, ptr ,offset))
					val store = TreeMove(mem, init)
					val var_inc = TreeBinOp(Oper.Plus, va, TreeConst(1))
					val var_update = TreeMove(va, var_inc)
					val cmp = TreeCjump(Oper.Lt, va, size, labelS, labelE)

					val seq = TreeSeq.makeSeq(List[TreeStm](ptr_init,
															var_init,
															cmp,
															l_S,
															store,
															var_update,
															cmp,
															l_E))
					
					val eseq = TreeEseq(seq, ptr)
					Ex(eseq)
				} 
				case _ => {
					require(false)
					Ex(TreeConst(0))
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
			
			for ((field, acc) <- d.params zip Level.current().formals) {
				field.varEntry.access = acc
			}

			val parentFuncName = currentFuncName
			currentFuncName = d.name

			val tr = translateExp(d.body)

			currentFuncName = parentFuncName;

			val rv = frame().rv()

			val stms = 	tr.unEx() match {
				case Some(eseq:TreeEseq) => {
					//TODO: To find more simple way!
/*
					while (eseq->exp->isESEQ_T()) {
						eseq = (tree::ESEQ*)eseq->exp;
					}
*/
					val mv_return_value = TreeMove(TreeTemp(rv), eseq.exp)
					List(eseq.stm, mv_return_value)
				}
				case Some(e) => {
					val mv_return_value = TreeMove(TreeTemp(rv), e) 
					List(mv_return_value)
				}
				case None => {
					//procedure
					List(tr.unNx().get)
				}
			}
			val seq = TreeSeq.makeSeq(stms)
			val body = frame().procEntryExit1(seq)
			val nx = Nx(body)
			procEntryExit(nx)
			nx
		}

		case d:VarDec => {
			val acc = Level.current().allocLocal(d.escape)
			d.varEntry.access = acc
			val va = acc.simpleVar(Level.current())
			val init = translateExp(d.init).unEx().get
			Nx(TreeMove(va, init))
		}
		case d:TypeDec => {
			//TODO:
			Ex(TreeConst(0))
		}
	}

	private def translateRecordField(ast:RecordField):Translate = {
		translateExp(ast.init)
	}

	private def callAlloc(size:Int):TreeExp = {
		val csize = TreeConst(size)
		frame().externalCall("alloc", List(csize))
	}

	private def isBuiltinFunc(f:Symbol):Boolean = List('print, 'flush, 'ord, 'chr, 
											   'stringEqual, 'stringLen, 'substring, 'stringConcat, 
											   'not, 'getch, 'test).contains(f)



	private def convertStrOp(op:Oper.Value, el:TreeExp, er:TreeExp):TreeExp = {
		val args = List(el, er)
		op match {
			case Oper.Plus => frame().externalCall("stringConcat", args)
			case Oper.Eq => frame().externalCall("stringEqual", args)
			case Oper.Ne => {
				val streq = frame().externalCall("stringEqual", args)
				frame().externalCall("not", List(streq))
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
		val tmp = new Temp()
		val ret = TreeTemp(tmp)
		val const0 = TreeConst(0)
		val const1 = TreeConst(1)
		val ret_init = TreeMove(ret, const0)
		
		val l1 = new Label()
		val l2 = new Label()
		val done = new Label()
		val L1 = TreeLabel(l1)
		val L2 = TreeLabel(l2)
		val DONE = TreeLabel(done)

		val cj1 = TreeCjump(Oper.Ne, el, const0, l1, done)
		val cj2 = TreeCjump(Oper.Ne, er, const0, l2, done)
		val ret_update = TreeMove(ret, const1)
		
		val seq = TreeSeq.makeSeq(List[TreeStm](ret_init,
												cj1,
												L1,
												cj2,
												L2,
												ret_update,
												DONE))
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
		val tmp = new Temp()
		val ret = TreeTemp(tmp)
		val const0 = TreeConst(0)
		val const1 = TreeConst(1)
		val ret_init = TreeMove(ret, const1)
		
		val l1 = new Label()
		val l2 = new Label()
		val done = new Label()
		val L1 = TreeLabel(l1)
		val L2 = TreeLabel(l2)
		val DONE = TreeLabel(done)

		val cj1 = TreeCjump(Oper.Eq, el, const0, l1, done)
		val cj2 = TreeCjump(Oper.Eq, er, const0, l2, done)
		val ret_update = TreeMove(ret, const0)
		
		val seq = TreeSeq.makeSeq(List[TreeStm](ret_init,
												cj1,
												L1,
												cj2,
												L2,
												ret_update,
												DONE))
		TreeEseq(seq, ret)
	}

	private def procEntryExit(tr:Translate) = {
		//TODO:
/*
		val stm = tr.unNx()
		val frag = new ProcFragment(stm, frame())
		fragments = fragments ++ frag
		currentLevel = currentLevel->parent;	
*/
	}
	
	private def frame():Frame = Level.current().frame
	private def wordSize() = frame().wordSize()
}
