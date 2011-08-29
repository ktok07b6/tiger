package tiger.common

import tiger.tree._
import tiger.frame._

class LevelAccess (home:Level, acc:FrameAccess) {
	override def toString():String = ""

	def simpleVar(current:Level):TreeExp = {
		var fp:TreeExp = TreeTemp(current.frame.fp())
		//TODO
		acc.exp(fp)
	}
}

class Level(f:Frame) {
	val frame = f
	var formals:List[LevelAccess] = List.empty

	println(f.formals)
	for (fa <- f.formals) {
		val acc = new LevelAccess(this, fa)
		formals = acc::formals
	}

	def allocLocal(escape:Boolean):LevelAccess = new LevelAccess(this, frame.allocLocal(escape))
}

object Level {
	def newLevel(name:Symbol, escapes:List[Boolean]) = {
		val l = new Level(Frame.newFrame(name, escapes))
		levels = l::levels
	}
	def newTopLevel(f:Frame) = {
		levels = List(new Level(f))
	}
	def deleteLevel() = {
		levels = levels.tail
	}
	def current() = levels.head
	var levels:List[Level] = List.empty
}
