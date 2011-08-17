import org.scalatest._
import scala.io.Source

class BasicTypeCheckSuite extends FunSuite {
	val dir:String = "/home/kataoka/tiger/testcases/basic/"

	def typeCheckTest(fileName:String):Boolean = {
		println("--- " + fileName)
		val src = Source.fromFile(dir+fileName).mkString
		val ast = Tiger.parse(src)
		Tiger.typeCheck(ast)
	}
/*
	test("decl") {
		assert(typeCheckTest("decl01.tig") == true)
		assert(typeCheckTest("decl02.tig") == true)
		assert(typeCheckTest("decl03.tig") == true)
		assert(typeCheckTest("decl04.tig") == true)
		assert(typeCheckTest("decl05.tig") == true)
		assert(typeCheckTest("decl06.tig") == true)
		assert(typeCheckTest("decl07.tig") == true)
		//assert(typeCheckTest("decl08.tig") == true)
		//assert(typeCheckTest("decl09.tig") == true)
		assert(typeCheckTest("decl10.tig") == true)
		assert(typeCheckTest("decl11.tig") == true)
		assert(typeCheckTest("decl12.tig") == true)
		assert(typeCheckTest("decl13.tig") == true)
		assert(typeCheckTest("decl14.tig") == true)
		assert(typeCheckTest("decl15.tig") == true)
		assert(typeCheckTest("decl16.tig") == true)
		assert(typeCheckTest("decl17.tig") == true)
		assert(typeCheckTest("decl18.tig") == true)
		//assert(typeCheckTest("decl19.tig") == true)
		assert(typeCheckTest("decl20.tig") == true)
		assert(typeCheckTest("decl21.tig") == true)
		assert(typeCheckTest("decl22.tig") == true)
		assert(typeCheckTest("decl23.tig") == true)
	}

	test("exp") {
		assert(typeCheckTest("exp01.tig") == true)
		assert(typeCheckTest("exp02.tig") == true)
		assert(typeCheckTest("exp03.tig") == true)
		assert(typeCheckTest("exp04.tig") == true)
		assert(typeCheckTest("exp05.tig") == true)
		assert(typeCheckTest("exp06.tig") == true)
		assert(typeCheckTest("exp07.tig") == true)
		assert(typeCheckTest("exp08.tig") == true)
	}
	test("op") {
		assert(typeCheckTest("op01.tig") == true)
		assert(typeCheckTest("op02.tig") == true)
		assert(typeCheckTest("op03.tig") == true)
		assert(typeCheckTest("op04.tig") == true)
		assert(typeCheckTest("op05.tig") == true)
		assert(typeCheckTest("op06.tig") == true)
		assert(typeCheckTest("op07.tig") == true)
		assert(typeCheckTest("op08.tig") == true)
		assert(typeCheckTest("op09.tig") == true)
		assert(typeCheckTest("op10.tig") == true)
		assert(typeCheckTest("op11.tig") == true)
		assert(typeCheckTest("op12.tig") == true)
		assert(typeCheckTest("op20.tig") == true)
		assert(typeCheckTest("op21.tig") == true)
		assert(typeCheckTest("op22.tig") == true)
		assert(typeCheckTest("op23.tig") == true)
		assert(typeCheckTest("op24.tig") == true)
		assert(typeCheckTest("op25.tig") == true)
		assert(typeCheckTest("op26.tig") == true)
		assert(typeCheckTest("op27.tig") == true)
		assert(typeCheckTest("op28.tig") == true)
		assert(typeCheckTest("op29.tig") == true)
		assert(typeCheckTest("op30.tig") == true)
		assert(typeCheckTest("op31.tig") == true)
		assert(typeCheckTest("op32.tig") == true)
		assert(typeCheckTest("op33.tig") == true)
		assert(typeCheckTest("op34.tig") == true)
		assert(typeCheckTest("op35.tig") == true)
		assert(typeCheckTest("op36.tig") == true)
		assert(typeCheckTest("op37.tig") == true)
		assert(typeCheckTest("op38.tig") == true)
		assert(typeCheckTest("op39.tig") == true)
		assert(typeCheckTest("op40.tig") == true)
		assert(typeCheckTest("op41.tig") == true)
		assert(typeCheckTest("op42.tig") == true)
	}

	test("func") {
		assert(typeCheckTest("func01.tig") == true)
		assert(typeCheckTest("func02.tig") == true)
		assert(typeCheckTest("func03.tig") == true)
		assert(typeCheckTest("func04.tig") == true)
		assert(typeCheckTest("func05.tig") == true)
		assert(typeCheckTest("func06.tig") == true)
		assert(typeCheckTest("func10.tig") == true)
		assert(typeCheckTest("func11.tig") == true)
		assert(typeCheckTest("func20.tig") == true)
	}

	test("for") {
		assert(typeCheckTest("for01.tig") == true)
		assert(typeCheckTest("for02.tig") == true)
		assert(typeCheckTest("for03.tig") == true)
		assert(typeCheckTest("for04.tig") == true)
		assert(typeCheckTest("for05.tig") == true)
	}

	test("if") {
		assert(typeCheckTest("if01.tig") == true)
		assert(typeCheckTest("if02.tig") == true)
		assert(typeCheckTest("if03.tig") == true)
		assert(typeCheckTest("if04.tig") == true)
		assert(typeCheckTest("if05.tig") == true)
		assert(typeCheckTest("if06.tig") == true)
		assert(typeCheckTest("if07.tig") == true)
		assert(typeCheckTest("if08.tig") == true)
	}

	test("return") {
		assert(typeCheckTest("return01.tig") == true)
		assert(typeCheckTest("return02.tig") == true)
		assert(typeCheckTest("return03.tig") == true)
	}

	test("scope") {
		assert(typeCheckTest("scope01.tig") == true)
		assert(typeCheckTest("scope02.tig") == true)
		assert(typeCheckTest("scope03.tig") == true)
		assert(typeCheckTest("scope04.tig") == true)
	}
*/
/*
	test("while") {
		assert(typeCheckTest("while01.tig") == true)
		assert(typeCheckTest("while02.tig") == true)
		assert(typeCheckTest("while03.tig") == true)
		assert(typeCheckTest("while04.tig") == true)
	}
*/
}
