package tiger.frame

import tiger.common._
import tiger.tree._
import scala.collection.mutable.ListBuffer


abstract class FrameAccess {
	def exp(e:TreeExp):TreeExp
}

abstract class Registers {
	def regs():List[Temp]
	def args():List[Temp]
	def callerSaves():List[Temp]
	def calleeSaves():List[Temp]
}

abstract class Frame {
	def allocLocal(escape:Boolean):FrameAccess
	def fp():Temp
	val wordSize:Int
	def externalCall(func:String, args:List[TreeExp]):TreeExp
	def rv():Temp
	def procEntryExit1(body:TreeStm):TreeStm
	def string(label:Label, value:String):String
	def staticChain(fp:TreeExp):TreeExp
	//def procEntryExit2(body:List[Instruction]):List[Instruction]
	//def procEntryExit3(body:List[Instruction]):List[Instruction]
	//def codegen(stm:TreeStm):List[Instruction]
	def registers():Registers
	//def spillTemp(body:List[Instruction], spill:Temp):body:List[Instruction]
	//def setUsedRegs(regs:List[Temp])
	def getEndFuncLabel():Label
	var formals:List[FrameAccess] = List.empty
}

class ARMRegisters extends Registers {
	override def regs():List[Temp] = r
	override def args():List[Temp] = regargs
	override def callerSaves():List[Temp] = regcaller
	override def calleeSaves():List[Temp] = regcallee

	val r = List(new Temp('r0),
				 new Temp('r1), 
				 new Temp('r2), 
				 new Temp('r3), 
				 new Temp('r4), 
				 new Temp('r5), 
				 new Temp('r6), 
				 new Temp('r7), 
				 new Temp('r8),
				 new Temp('r9),
				 new Temp('r10),
				 new Temp('fp),
				 new Temp('r12),
				 new Temp('sp),
				 new Temp('lr),
				 new Temp('pc))
	val regargs = List(r(0),
					   r(1),
					   r(2),
					   r(3))
	val regcaller = List(r(0),
						 r(1),
						 r(2),
						 r(3))
	val regcallee = List(r(4),
						 r(5),
						 r(6),
						 r(7),
						 r(8),
						 r(9),
						 r(10))
}

class ARMFrame(name:Symbol, escapes:List[Boolean]) extends Frame {
	var frameOffset:Int = 0
	val endFuncLabel = new Label(Symbol("end_of_" + name.name))
	formals = escapes.map(allocLocal)

	class InFrame(offset:Int) extends FrameAccess {
		override def exp(fp:TreeExp):TreeExp = {
			val konst = TreeConst(offset)
			val bop = TreeBinOp(Oper.Plus, fp, konst)
			TreeMem(bop)
		}
	}

	class InReg(tmp:Temp) extends FrameAccess {
		override def exp(e:TreeExp):TreeExp = {
			TreeTemp(tmp)
		}
	}

	def allocLocal(escape:Boolean):FrameAccess = {
		if (escape) {
			val access = new InFrame(-frameOffset)
			frameOffset += wordSize
			access
		} else {
			new InReg(new Temp())
		}
	}

	def fp():Temp = {
		armregs.r(11)
	}

	val wordSize:Int = 4

	def externalCall(func:String, args:List[TreeExp]):TreeExp = {
		TreeCall(TreeName(new Label(Symbol(func))), args)
	}

	def rv():Temp = {
		armregs.r(0)
	}

	def procEntryExit1(body:TreeStm):TreeStm = {
		var newBody:ListBuffer[TreeStm] = ListBuffer.empty
		val fname = new Label(name)
		val l = TreeLabel(fname)
		newBody += l
		var extraArgOffset:Int = 12
		var i:Int = 0
		for (formal <- formals) {
			if (i < 4) {
				val r = TreeTemp(armregs.r(i))
				val tmp = formal.exp(TreeTemp(fp))
				newBody += TreeMove(tmp, r)
			} else {
				val offs = TreeConst(extraArgOffset)
				val base = TreeTemp(fp)
				val m = TreeMem(TreeBinOp(Oper.Plus, base, offs))
				val tmp = formal.exp(TreeTemp(fp))
				newBody += TreeMove(tmp, m)
				extraArgOffset += wordSize
			}
			i += 1
		}
		newBody += body
		TreeSeq.makeSeq(newBody.result:_*)
	}

	def string(label:Label, value:String):String = {
		label + ":\t.long\t" + label + "_body\n" + 
			label + "_body:\t.asciz\t\"" + value + "\"" + 
			"\n\t.align 2"
		/* example
		 str1:    .ascii      "mov\tr1, r2\n"
         .align 2
		 str2:    .asciz      "New Line\n"
         .align 2
		*/ 
	}

	def staticChain(fp:TreeExp):TreeExp = {
		TreeMem(fp)
	}

	val armregs = new ARMRegisters()
	def registers():Registers = armregs

	def getEndFuncLabel = endFuncLabel
}

object Frame {
	def newFrame(name:Symbol, escapes:List[Boolean]):Frame = {
		new ARMFrame(name, escapes)
	}
}
