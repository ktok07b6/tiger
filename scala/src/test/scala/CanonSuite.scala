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
		assert(canonTest("func05.tig") == true)
		assert(canonTest("func06.tig") == true)
		assert(canonTest("func10.tig") == true)
		assert(canonTest("func11.tig") == true)
		assert(canonTest("func20.tig") == true)
	}

/*
	test("for") {
		assert(canonTest("for01.tig") == true)
		assert(canonTest("for02.tig") == true)
		assert(canonTest("for03.tig") == true)
		assert(canonTest("for04.tig") == true)
		assert(canonTest("for05.tig") == true)
	}

	test("if") {
		assert(canonTest("if01.tig") == true)
		assert(canonTest("if02.tig") == true)
		assert(canonTest("if03.tig") == true)
		assert(canonTest("if04.tig") == true)
		assert(canonTest("if05.tig") == true)
		assert(canonTest("if06.tig") == true)
		assert(canonTest("if07.tig") == true)
		assert(canonTest("if08.tig") == true)
	}

	test("return") {
		assert(canonTest("return01.tig") == true)
		assert(canonTest("return02.tig") == true)
		assert(canonTest("return03.tig") == true)
	}

	test("scope") {
		assert(canonTest("scope01.tig") == true)
		assert(canonTest("scope02.tig") == true)
		assert(canonTest("scope03.tig") == true)
		assert(canonTest("scope04.tig") == true)
	}

	test("while") {
		assert(canonTest("while01.tig") == true)
		assert(canonTest("while02.tig") == true)
		assert(canonTest("while03.tig") == true)
		assert(canonTest("while04.tig") == true)
	}
*/
}
