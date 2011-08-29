import org.scalatest._
import tiger.ast._
import tiger.symbol._
import tiger.parser._
import tiger.typ._
import tiger.common.Oper
import tiger.tree._
import tiger.frame._
import scala.io.Source

class CanonSuite extends FunSuite {
	val dir:String = "/home/kataoka/tiger/testcases/basic/"

	def canonTest(fileName:String):Boolean = {
		println("--- " + fileName)
		val src = Source.fromFile(dir+fileName).mkString
		val ast = Tiger.parse(src)
		val ret = Tiger.typeCheck(ast)
		if (ret) {
			val fragments = Tiger.ast2tree(ast)
			val procs = fragments.filter(_.isInstanceOf[ProcFragment]).map(_.asInstanceOf[ProcFragment])
			for (proc <- procs) {
				val stms = Tiger.linearize(proc)
				stms.foreach(println)
			}
		}
		ret
	}
	test("func") {
		assert(canonTest("func01.tig") == true)
		assert(canonTest("func02.tig") == true)
		assert(canonTest("func03.tig") == true)
		assert(canonTest("func04.tig") == true)
/*
		assert(canonTest("func05.tig") == true)
		assert(canonTest("func06.tig") == true)
		assert(canonTest("func10.tig") == true)
		assert(canonTest("func11.tig") == true)
		assert(canonTest("func20.tig") == true)
*/
	}

}
