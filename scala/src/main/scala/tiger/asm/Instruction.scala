package tiger.asm

import tiger.common._

abstract class Instruction {
	def use():List[Temp]
	def define():List[Temp]
	def jumps():List[Label]

	def replaceUse(oldt:Temp, newt:Temp):Instruction
	def replaceUse(index:Int, newt:Temp):Instruction
	def replaceDef(oldt:Temp, newt:Temp):Instruction
	def replaceDef(index:Int, newt:Temp):Instruction

	def getOpcode():String
	def getOperands():String
	def getComment():String
}

object Instruction {
	def format(inst:Instruction, tempMap:Temp=>Temp):String = {
		val opcode = inst.getOpcode
		val operands = inst.getOperands
		val comment  = inst.getComment
		if (opcode.isEmpty) {
			return "\t" + comment
		}

		var result:String = inst match {
			case l:AsmLabel => opcode
			case _          => "\t" + opcode + "\t"
		}

		val uses:List[Temp] = inst.use
		val defs:List[Temp] = inst.define
		val jmps:List[Label] = inst.jumps
		val ops = operands.toList

		def form(str:String, ops:List[Char]):String = ops match {
			case Nil => str
			case '\''::typ::nc::xs => {
				val num = nc.toInt
				val s:String = typ match {
					case 's' => {
						val t = tempMap(uses(num))
						if (t != null) t.toString else ""
					}
					case 'd' => {
						val t = tempMap(defs(num))
						if (t != null) t.toString else ""
					}
					case 'j' => {
						jmps(num).toString
					}
					case _ => error("invalid assem format")
				}
				if (s.isEmpty()) {
					//If a temp of instruction is not used,
					//this instruction is ignored.
					return "\t@" + opcode + "\t" + operands + comment
				}
				form(str+s, xs)
			}
			case x::xs => {
				form(str+x, xs)
			}
		}
		result = form(result, ops)

		//println("                          old assem = " + assem)
		//println("new assem = " + result)
		if (!comment.isEmpty) {
			result += "\t" + comment
		}
		result
	}

	val nulltemp:Temp = new Temp()
}

case class AsmOper(opcode:String, 
				   operands:String, 
				   dst:List[Temp], 
				   src:List[Temp],
				   comment:String) extends Instruction {
	var targets:List[Label] = List.empty

	override def use():List[Temp] = src
	override def define():List[Temp] = dst
	override def jumps():List[Label] = targets

	override def replaceUse(oldt:Temp, newt:Temp):Instruction = {
		val newsrc = src.map {x => if (x == oldt) newt else x}
		AsmOper(opcode, operands, dst, newsrc, comment)
	}
	override def replaceUse(index:Int, newt:Temp):Instruction = {
		AsmOper(opcode, operands, dst, src.updated(index, newt), comment)
	}
	override def replaceDef(oldt:Temp, newt:Temp):Instruction = {
		val newdst = dst.map {x => if (x == oldt) newt else x}
		AsmOper(opcode, operands, newdst, src, comment)
	}
	override def replaceDef(index:Int, newt:Temp):Instruction = {
		AsmOper(opcode, operands, dst.updated(index, newt), src, comment)
	}
	override def getOpcode():String = opcode
	override def getOperands():String = operands
	override def getComment():String = comment

	def setJumps(jumps:List[Label]) = {
		targets = jumps
	}
}

case class AsmMove(opcode:String, 
				   operands:String, 
				   dst:Temp, 
				   src:Temp,
				   comment:String) extends Instruction {
	override def use():List[Temp] = List(src)
	override def define():List[Temp] = List(dst)
	override def jumps():List[Label] = List()

	override def replaceUse(oldt:Temp, newt:Temp):Instruction = {
		require(oldt == src)
		AsmMove(opcode, operands, dst, newt, comment)
	}
	override def replaceUse(index:Int, newt:Temp):Instruction = {
		AsmMove(opcode, operands, dst, newt, comment)
	}
	override def replaceDef(oldt:Temp, newt:Temp):Instruction = {
		require(oldt == dst)
		AsmMove(opcode, operands, newt, src, comment)
	}
	override def replaceDef(index:Int, newt:Temp):Instruction = {
		AsmMove(opcode, operands, newt, src, comment)
	}
	override def getOpcode():String = opcode
	override def getOperands():String = operands
	override def getComment():String = comment
}

case class AsmLabel(assem:String, label:Label) extends Instruction {
	override def use():List[Temp] = List()
	override def define():List[Temp] = List()
	override def jumps():List[Label] = List()

	override def replaceUse(oldt:Temp, newt:Temp):Instruction = error("do not call this")
	override def replaceUse(index:Int, newt:Temp):Instruction = error("do not call this")
	override def replaceDef(oldt:Temp, newt:Temp):Instruction = error("do not call this")
	override def replaceDef(index:Int, newt:Temp):Instruction = error("do not call this")
	override def getOpcode():String = ""
	override def getOperands():String = ""
	override def getComment():String = ""
}
