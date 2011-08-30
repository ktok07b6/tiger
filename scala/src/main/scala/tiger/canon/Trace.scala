package tiger.canon

import tiger.tree._
import tiger.common._
import scala.collection.mutable.ListBuffer

class Trace(blocks:List[List[TreeStm]]) {
	private var table:Map[Label, Int] = Map.empty
	var traced:List[TreeStm] = List.empty

	doTrace()

	def doTrace():Unit = {
		var index:Int = 0
		for (block <- blocks) {
			val label = block.head.asInstanceOf[TreeLabel].label
			table += (label->index)
			index += 1

			traceBlock(block)
		}
	}

	def traceBlock(block:List[TreeStm]):Unit = {
		val label = block.head.asInstanceOf[TreeLabel].label
		table -= label
		traced = traced ++ block
	
		val s = block.last
		s match {
			case j:TreeJump => traceJUMP(j)
			case cj:TreeCjump => traceCJUMP(cj)
		}
	}

	def traceJUMP(j:TreeJump):Unit = {
		if (j.targets.length == 1) {
			val label = j.targets.head
			
			if (table.contains(label)) {
				val block = blocks(table(label))
				//remove the JUMP statement
				require(traced.last.isInstanceOf[TreeJump])
				traced = traced.init
				traceBlock(block)
			}
		}
	}

	def traceCJUMP(cj:TreeCjump):Unit = {
		if (table.contains(cj.f)) {
			traceBlock(blocks(table(cj.f)))
		} else if (table.contains(cj.t)) {
			val block = blocks(table(cj.t))
			//TODO:
			//replace the CJUMP
			require(traced.last.isInstanceOf[TreeCjump])
			traced = traced.init
			val new_cj = TreeCjump(Oper.not(cj.op), 
								   cj.l, 
								   cj.r,
								   cj.f,
								   cj.t)
			traced = traced :+ new_cj
			traceBlock(block)
		} else {
			val falselab = new Label()
			traced = traced.init
			val new_cj = TreeCjump(cj.op, cj.l, cj.r, cj.t, falselab)
			traced = traced ++ List(new_cj, TreeLabel(falselab), TreeJump(0, List(cj.f)))
		}
	}
}
