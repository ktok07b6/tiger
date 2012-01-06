package tiger.asm

import tiger.common._
import tiger.tree._
import tiger.frame._

import scala.collection.mutable.ListBuffer
trait CodeGen {
	def generate(stm:TreeStm):List[Instruction] = {
		insts.clear
		munchStm(stm)
		insts.toList
	}

	protected def munchStm(stm:TreeStm):Unit = stm match {
		case TreeSeq(l, r) => munchSeq(l, r)
		case TreeMove(dst, src) => munchMove(dst, src)
		case TreeLabel(l) => munchLabel(l)
		case TreeJump(e, labs) => require(labs.length == 1); munchJump(labs.head)
		case cj:TreeCjump => munchCjump(cj)
		case TreeExpr(exp) => munchExpr(exp)
	}
	
	protected def munchSeq(l:TreeStm, r:TreeStm):Unit = {
		munchStm(l)
		munchStm(r)
	}

	protected def munchExp(exp:TreeExp):Temp = exp match {
		case mem:TreeMem => munchMem(mem)
		case binop:TreeBinOp => munchBinOp(binop)
		case const:TreeConst => munchConst(const)
		case temp:TreeTemp => munchTemp(temp)
		case call:TreeCall => munchCall(call)
		case name:TreeName => munchName(name)
	}

	protected def emit(inst:Instruction):Unit = {
		insts += inst
	}

	protected var insts:ListBuffer[Instruction] = ListBuffer.empty

	protected def munchMove(dst:TreeExp, src:TreeExp):Unit
	protected def munchLabel(lab:Label):Unit
	protected def munchJump(lab:Label):Unit
	protected def munchCjump(cj:TreeCjump):Unit
	protected def munchExpr(exp:TreeExp):Unit
	protected def munchArgs(exps:List[TreeExp]):List[Temp]

	protected def munchMem(mem:TreeMem):Temp
	protected def munchBinOp(binop:TreeBinOp):Temp
	protected def munchCall(call:TreeCall):Temp
	protected def munchConst(const:TreeConst):Temp
	protected def munchTemp(tmp:TreeTemp):Temp
	protected def munchName(name:TreeName):Temp
}

class ARMCodeGen(fr:ARMFrame) extends CodeGen {
	val frame = fr
	val IMMEDIATE_MAX:Int = 255
	protected def munchMove(dst:TreeExp, src:TreeExp):Unit = {
		(dst, src) match {
			case (TreeMem(e), _) => {
				e match {
					case TreeBinOp(Oper.Plus, TreeConst(i1), TreeConst(i2)) => {
						val src_list = List(munchExp(e), munchExp(src))
						emit(AsmOper("str", "'s1, ['s0]", List.empty, src_list, ""))
					}
					case TreeBinOp(Oper.Plus, lhs, TreeConst(i)) if (i <= IMMEDIATE_MAX) => {
						val src_list = List(munchExp(lhs), munchExp(src))
						emit(AsmOper("str", 
									 "'s1, ['s0, #" + i + "]", 
									 List.empty, 
									 src_list, 
									 ""))
					}
					case TreeBinOp(Oper.Plus, TreeConst(i), rhs) if (i <= IMMEDIATE_MAX) => {
						val src_list = List(munchExp(rhs), munchExp(src))
						emit(AsmOper("str", 
									 "'s1, ['s0, #" + i + "]", 
									 List.empty, 
									 src_list, 
									 ""))
					}
					case _ => {
						val src_list = List(munchExp(e), munchExp(src))
						emit(AsmOper("str", "'s1, ['s0]", List.empty, src_list, ""))
					}
				}
			}
			case (TreeTemp(t), TreeConst(i)) => {
				emit(AsmMove("ldr", "'d0, =" + i, t, Instruction.nulltemp, ""))
			}
			case (TreeTemp(t), _) => {
				val src_exp = munchExp(src)
				emit(AsmMove("mov", "'d0, 's0", t, src_exp, ""))
			}
		}
	}

	protected def munchLabel(lab:Label):Unit = {
		emit(AsmLabel(lab + ":", lab));
	}

	protected def munchJump(lab:Label):Unit = {
		val op = AsmOper("b", "'j0", List.empty, List.empty, "");
		op.setJumps(List(lab));
		emit(op);
	}

	protected def munchCjump(cj:TreeCjump):Unit = {
	}

	protected def munchExpr(exp:TreeExp):Unit = {
		exp match {
			case TreeCall(func, args) => {
				//TODO: r0-r3 will be destoryed in c function
				val op = AsmOper("bl", "'j0", frame.registers.args, munchArgs(args), "");
				op.setJumps(List(func.label));
				emit(op);
			}
		}
	}

	protected def munchArgs(exps:List[TreeExp]):List[Temp] = {
		List.empty
	}

	protected def munchMem(mem:TreeMem):Temp = mem.exp match {
		case TreeBinOp(Oper.Plus, TreeConst(i1), TreeConst(i2)) => {
			val tmp = new Temp()
			emit(AsmOper("ldr", 
						 "'d0, ['s0]", 
						 List(tmp), 
						 List(munchExp(mem.exp)),
						 ""))
			tmp
		}
		case TreeBinOp(Oper.Plus, lhs, TreeConst(i)) if (i <= IMMEDIATE_MAX) => {
			val tmp = new Temp()
			emit(AsmOper("ldr", 
						 "'d0, ['s0, #" + i + "]", 
						 List(tmp), 
						 List(munchExp(lhs)),
						 ""))
			tmp
		}
		case TreeBinOp(Oper.Plus, TreeConst(i), rhs) if (i <= IMMEDIATE_MAX) => {
			val tmp = new Temp()
			emit(AsmOper("ldr", 
						 "'d0, ['s0, #" + i + "]", 
						 List(tmp), 
						 List(munchExp(rhs)),
						 ""))
			tmp
		}
		case _ => {
			val tmp = new Temp()
			emit(AsmOper("ldr", 
						 "'d0, ['s0]", 
						 List(tmp), 
						 List(munchExp(mem.exp)),
						 ""))
			tmp
		}
	}

	protected def munchBinOp(binop:TreeBinOp):Temp = {
		null
	}

	protected def munchCall(call:TreeCall):Temp = {
		//TODO: r0-r3 will be destoryed in c function
		val op = AsmOper("bl", 
						 "'j0", 
						 frame.registers.args,
						 munchArgs(call.args),
						 "")
		op.setJumps(List(call.func.label))
		emit(op)
		frame.rv
	}

	protected def munchConst(const:TreeConst):Temp = {
		val tmp = new Temp()
		emit(AsmOper("ldr", 
					 "'d0, =" + const.value, 
					 List(tmp),
					 List.empty,
					 ""))
		tmp
	}

	protected def munchTemp(tmp:TreeTemp):Temp = {
		tmp.temp
	}

	protected def munchName(name:TreeName):Temp = {
		val tmp = new Temp()
		emit(AsmOper("ldr", 
					 "'d0, " + name.label,
					 List(tmp),
					 List.empty,
					 ""))
		tmp
	}

}
