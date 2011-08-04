import org.scalatest._
import scala.io.Source

class TypeCheckSuite extends FunSuite {
	val dir:String = "/home/kataoka/tiger/testcases/integration/"
	
	def typeCheckTest(fileName:String):Boolean = {
		val src = Source.fromFile(dir+fileName).mkString
		val ast = Tiger.parse(src)
		Tiger.typeCheck(ast)
	}

	test("test01") {
		assert(typeCheckTest("test01.tig") == true)
	}
	test("test02") {
		assert(typeCheckTest("test02.tig") == true)
	}
	test("test03") {
		assert(typeCheckTest("test03.tig") == true)
	}

	test("test04") {
		assert(typeCheckTest("test04.tig") == true)
	}
	test("test05") {
		assert(typeCheckTest("test05.tig") == true)
	}
	test("test06") {
		assert(typeCheckTest("test06.tig") == true)
	}
/*
	test("test07") {
		assert(typeCheckTest("test07.tig") == true)
	}
	test("test08") {
		assert(typeCheckTest("test08.tig") == true)
	}
	test("test09") {
		assert(typeCheckTest("test09.tig") == false)
	}
	test("test10") {
		assert(typeCheckTest("test10.tig") == false)
	}
	test("test11") {
		assert(typeCheckTest("test11.tig") == false)
	}
	test("test12") {
		assert(typeCheckTest("test12.tig") == true)
	}
	test("test13") {
		assert(typeCheckTest("test13.tig") == false)
	}
	test("test14") {
		assert(typeCheckTest("test14.tig") == false)
	}
	test("test15") {
		assert(typeCheckTest("test15.tig") == false)
	}
	test("test16") {
		assert(typeCheckTest("test16.tig") == false)
	}
*/
}
