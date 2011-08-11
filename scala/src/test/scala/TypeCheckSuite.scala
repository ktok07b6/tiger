import org.scalatest._
import scala.io.Source

class TypeCheckSuite extends FunSuite {
	val dir:String = "/home/kataoka/tiger/testcases/integration/"
	
	def typeCheckTest(fileName:String):Boolean = {
		val src = Source.fromFile(dir+fileName).mkString
		val ast = Tiger.parse(src)
		Tiger.typeCheck(ast)
	}
/*
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
	test("test17") {
		assert(typeCheckTest("test17.tig") == true)
	}
	test("test18") {
		assert(typeCheckTest("test18.tig") == true)
	}
	test("test19") {
		assert(typeCheckTest("test19.tig") == false)
	}
	test("test20") {
		assert(typeCheckTest("test20.tig") == false)
	}
	test("test21") {
		assert(typeCheckTest("test21.tig") == false)
	}
	test("test22") {
		assert(typeCheckTest("test22.tig") == false)
	}
	test("test23") {
		assert(typeCheckTest("test23.tig") == false)
	}
	test("test24") {
		assert(typeCheckTest("test24.tig") == false)
	}
	test("test25") {
		assert(typeCheckTest("test25.tig") == false)
	}
	test("test26") {
		assert(typeCheckTest("test26.tig") == false)
	}
	test("test27") {
		assert(typeCheckTest("test27.tig") == true)
	}
	test("test28") {
		assert(typeCheckTest("test28.tig") == false)
	}
	test("test29") {
		assert(typeCheckTest("test29.tig") == false)
	}
	test("test30") {
		assert(typeCheckTest("test30.tig") == true)
	}
	test("test31") {
		assert(typeCheckTest("test31.tig") == false)
	}
	test("test32") {
		assert(typeCheckTest("test32.tig") == false)
	}
	test("test33") {
		assert(typeCheckTest("test33.tig") == false)
	}
	test("test34") {
		assert(typeCheckTest("test34.tig") == false)
	}
	test("test35") {
		assert(typeCheckTest("test35.tig") == false)
	}
	test("test36") {
		assert(typeCheckTest("test36.tig") == false)
	}
	test("test37") {
		assert(typeCheckTest("test37.tig") == true)
	}
	test("test38") {
		assert(typeCheckTest("test38.tig") == true)
	}
	test("test39") {
		assert(typeCheckTest("test39.tig") == true)
	}


	test("test40") {
		assert(typeCheckTest("test40.tig") == false)
	}
	test("test41") {
		assert(typeCheckTest("test41.tig") == true)
	}
	test("test42") {
		assert(typeCheckTest("test42.tig") == true)
	}
	test("test43") {
		assert(typeCheckTest("test43.tig") == false)
	}
	test("test44") {
		assert(typeCheckTest("test44.tig") == true)
	}
	test("test45") {
		assert(typeCheckTest("test45.tig") == false)
	}
	test("test46") {
		assert(typeCheckTest("test46.tig") == true)
	}
	test("test47") {
		assert(typeCheckTest("test47.tig") == true)
	}
	test("test48") {
		assert(typeCheckTest("test48.tig") == true)
	}
	test("test49") {
		assert(typeCheckTest("test49.tig") == false)
	}
*/
}
