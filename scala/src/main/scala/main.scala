import tiger.ast._
import tiger.symbol._
import tiger.parser._
import tiger.typ._
import tiger.frame._
import tiger.tree._
import scala.io.Source

object Tiger {
	def main(args:Array[String]) {
		val src = Source.fromFile(args(0)).mkString
		val ast = parse(src)
		typeCheck(ast)
	}

	def parse(src:String):Option[ASTExp] = {
		val parser = new Parser
		val result = parser.parse(src)
		result match {
			case Some(ast) => ASTPrinter.print(ast);
			case _ => println("!!!!! PARSE FAILED !!!!!:"); println(src)
		}
		result
	}

	def typeCheck(ast:Option[ASTExp]):Boolean = ast match {
		case Some(ast) => TypeCheck.typeCheck(ast)
		case _ => println("!!!!! INVALID AST !!!!!"); false
	}

	def ast2tree(ast:Option[ASTExp]):Tree = ast match {
		case Some(ast) => AST2Tree.ast2tree(ast, new ARMFrame('frame, List()))
		case _ => println("!!!!! INVALID AST !!!!!"); TreeConst(0) 
	}
}
