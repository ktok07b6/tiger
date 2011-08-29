import org.scalatest._
import tiger.ast._
import tiger.symbol._
import tiger.parser._
import tiger.typ._
import tiger.common.Oper
import tiger.tree._
import tiger.frame._
import scala.io.Source

class AS2TreeSuite extends FunSuite {
	val dir:String = "/home/kataoka/tiger/testcases/basic/"

	def ast2treeTest(fileName:String):Boolean = {
		println("--- " + fileName)
		val src = Source.fromFile(dir+fileName).mkString
		val ast = Tiger.parse(src)
		val ret = Tiger.typeCheck(ast)
		if (ret) {
			val fragments = Tiger.ast2tree(ast)
			println(fragments)
		}
		ret
	}

	test("decl") {
		assert(ast2treeTest("decl01.tig") == true)
		assert(ast2treeTest("decl02.tig") == true)
		assert(ast2treeTest("decl03.tig") == true)
		assert(ast2treeTest("decl04.tig") == true)
		assert(ast2treeTest("decl05.tig") == true)
		assert(ast2treeTest("decl06.tig") == true)
		assert(ast2treeTest("decl07.tig") == true)
		//assert(ast2treeTest("decl08.tig") == true)
		//assert(ast2treeTest("decl09.tig") == true)
		assert(ast2treeTest("decl10.tig") == true)
		assert(ast2treeTest("decl11.tig") == true)
		assert(ast2treeTest("decl12.tig") == true)
		assert(ast2treeTest("decl13.tig") == true)
		assert(ast2treeTest("decl14.tig") == true)
		assert(ast2treeTest("decl15.tig") == true)
		assert(ast2treeTest("decl16.tig") == true)
		assert(ast2treeTest("decl17.tig") == true)
		assert(ast2treeTest("decl18.tig") == true)
		//assert(ast2treeTest("decl19.tig") == true)
		assert(ast2treeTest("decl20.tig") == true)
		assert(ast2treeTest("decl21.tig") == true)
		assert(ast2treeTest("decl22.tig") == true)
		assert(ast2treeTest("decl23.tig") == true)
	}

	test("exp") {
		assert(ast2treeTest("exp01.tig") == true)
		assert(ast2treeTest("exp02.tig") == true)
		assert(ast2treeTest("exp03.tig") == true)
		assert(ast2treeTest("exp04.tig") == true)
		assert(ast2treeTest("exp05.tig") == true)
		assert(ast2treeTest("exp06.tig") == true)
		assert(ast2treeTest("exp07.tig") == true)
		assert(ast2treeTest("exp08.tig") == true)
	}
	test("op") {
		assert(ast2treeTest("op01.tig") == true)
		assert(ast2treeTest("op02.tig") == true)
		assert(ast2treeTest("op03.tig") == true)
		assert(ast2treeTest("op04.tig") == true)
		assert(ast2treeTest("op05.tig") == true)
		assert(ast2treeTest("op06.tig") == true)
		assert(ast2treeTest("op07.tig") == true)
		assert(ast2treeTest("op08.tig") == true)
		assert(ast2treeTest("op09.tig") == true)
		assert(ast2treeTest("op10.tig") == true)
		assert(ast2treeTest("op11.tig") == true)
		assert(ast2treeTest("op12.tig") == true)
		assert(ast2treeTest("op20.tig") == true)
		assert(ast2treeTest("op21.tig") == true)
		assert(ast2treeTest("op22.tig") == true)
		assert(ast2treeTest("op23.tig") == true)
		assert(ast2treeTest("op24.tig") == true)
		assert(ast2treeTest("op25.tig") == true)
		assert(ast2treeTest("op26.tig") == true)
		assert(ast2treeTest("op27.tig") == true)
		assert(ast2treeTest("op28.tig") == true)
		assert(ast2treeTest("op29.tig") == true)
		assert(ast2treeTest("op30.tig") == true)
		assert(ast2treeTest("op31.tig") == true)
		assert(ast2treeTest("op32.tig") == true)
		assert(ast2treeTest("op33.tig") == true)
		assert(ast2treeTest("op34.tig") == true)
		assert(ast2treeTest("op35.tig") == true)
		assert(ast2treeTest("op36.tig") == true)
		assert(ast2treeTest("op37.tig") == true)
		assert(ast2treeTest("op38.tig") == true)
		assert(ast2treeTest("op39.tig") == true)
		assert(ast2treeTest("op40.tig") == true)
		assert(ast2treeTest("op41.tig") == true)
		assert(ast2treeTest("op42.tig") == true)
	}

	test("func") {
		assert(ast2treeTest("func01.tig") == true)
		assert(ast2treeTest("func02.tig") == true)
		assert(ast2treeTest("func03.tig") == true)
		assert(ast2treeTest("func04.tig") == true)
		assert(ast2treeTest("func05.tig") == true)
		assert(ast2treeTest("func06.tig") == true)
		assert(ast2treeTest("func10.tig") == true)
		assert(ast2treeTest("func11.tig") == true)
		assert(ast2treeTest("func20.tig") == true)
	}

	test("for") {
		assert(ast2treeTest("for01.tig") == true)
		assert(ast2treeTest("for02.tig") == true)
		assert(ast2treeTest("for03.tig") == true)
		assert(ast2treeTest("for04.tig") == true)
		assert(ast2treeTest("for05.tig") == true)
	}

	test("if") {
		assert(ast2treeTest("if01.tig") == true)
		assert(ast2treeTest("if02.tig") == true)
		assert(ast2treeTest("if03.tig") == true)
		assert(ast2treeTest("if04.tig") == true)
		assert(ast2treeTest("if05.tig") == true)
		assert(ast2treeTest("if06.tig") == true)
		assert(ast2treeTest("if07.tig") == true)
		assert(ast2treeTest("if08.tig") == true)
	}

	test("return") {
		assert(ast2treeTest("return01.tig") == true)
		assert(ast2treeTest("return02.tig") == true)
		assert(ast2treeTest("return03.tig") == true)
	}

	test("scope") {
		assert(ast2treeTest("scope01.tig") == true)
		assert(ast2treeTest("scope02.tig") == true)
		assert(ast2treeTest("scope03.tig") == true)
		assert(ast2treeTest("scope04.tig") == true)
	}


	test("while") {
		assert(ast2treeTest("while01.tig") == true)
		assert(ast2treeTest("while02.tig") == true)
		assert(ast2treeTest("while03.tig") == true)
		assert(ast2treeTest("while04.tig") == true)
	}

}
