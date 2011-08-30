import tiger.ast._
import tiger.symbol._
import tiger.parser._
import tiger.typ._
import tiger.frame._
import tiger.tree._
import tiger.canon._
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

	def ast2tree(ast:Option[ASTExp]):List[Fragment] = ast match {
		case Some(ast) => AST2Tree.ast2tree(ast, new ARMFrame('__tiger_main, List()))
		case _ => println("!!!!! INVALID AST !!!!!"); List.empty 
	}

	def codegen(fragments:List[Fragment]):String = {
		val codes = for (frag <- fragments) yield frag match {
			case proc:ProcFragment => {
				linearize(proc)
				""
			}
			case data:DataFragment => {
				data.str
			}
		}
		codes.mkString
	}


	def linearize(proc:ProcFragment):List[TreeStm] = {
		val stms = Canon.linearize(proc.stm)
		val bb = new BasicBlock(proc.frame.getEndFuncLabel)
		val blocks = bb.makeBlocks(stms)
		//blocks.flatten
		val trace = new Trace(blocks)
		trace.traced
	}
}
