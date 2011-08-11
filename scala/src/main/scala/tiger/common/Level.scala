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

class Level(n:Symbol, f:Frame) {
	val name = n
	val frame = f
	var formals:List[LevelAccess] = List.empty

	for (f <- frame.formals) {
		val acc = new LevelAccess(this, f)
		formals = acc::formals
	}

	def allocLocal(escape:Boolean):LevelAccess = new LevelAccess(this, frame.allocLocal(escape))
}

object Level {
	def newLevel(name:Symbol, escapes:List[Boolean]) = {
		val l = new Level(name, Frame.newFrame(name, escapes))
		levels = l::levels
	}
	def newTopLevel(f:Frame) = {
		val l = new Level('nil, f)
		levels = l::Nil
	}

	def deleteLevel() = {
		levels = levels.drop(1)
	}
	def current() = levels.head
	var levels:List[Level] = List.empty
}
