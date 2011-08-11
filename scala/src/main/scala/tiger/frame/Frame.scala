package tiger.frame

import tiger.common._
import tiger.tree._


class FrameAccess {
	def exp(e:TreeExp):TreeExp = e
}

abstract class Frame {
	def allocLocal(escape:Boolean):FrameAccess
	def fp():Temp
	def wordSize():Int
	def externalCall(func:String, args:List[TreeExp]):TreeExp
	def rv():Temp
	def procEntryExit1(body:TreeStm):TreeStm
	def string(label:Label, value:String):String
	def staticChain(fp:TreeExp):TreeExp
	//def procEntryExit2(body:List[Instruction]):List[Instruction]
	//def procEntryExit3(body:List[Instruction]):List[Instruction]
	//def codegen(stm:TreeStm):List[Instruction]
	//def registers():Registers
	//def spillTemp(body:List[Instruction], spill:Temp):body:List[Instruction]
	//def setUsedRegs(regs:List[Temp])
	//def getEndFuncLabel():Label;
	var formals:List[FrameAccess] = List.empty
}

class ARMFrame extends Frame {
	def allocLocal(escape:Boolean):FrameAccess = {
		new FrameAccess()
	}

	def fp():Temp = {
		new Temp()
	}
	def wordSize():Int = {
		4
	}
	def externalCall(func:String, args:List[TreeExp]):TreeExp = {
		TreeConst(0)
	}
	def rv():Temp = {
		new Temp()
	}
	def procEntryExit1(body:TreeStm):TreeStm = {
		body
	}
	def string(label:Label, value:String):String = {
		""
	}
	def staticChain(fp:TreeExp):TreeExp = {
		fp
	}
}

object Frame {
	def newFrame(name:Symbol, escapes:List[Boolean]):Frame = {
		new ARMFrame()
	}
}
