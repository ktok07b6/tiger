import tiger.ast._
import tiger.symbol._

object Tiger {
	def main(args: Array[String]) {
		test2
	}

	def test() {
		val sa1 = SymbolTable.symbol("a")
		val sb1 = SymbolTable.symbol("b")
	   	val sa2 = SymbolTable.symbol("a")
		println(sa1 + " " + sa1.name)
		println(sb1 + " " + sb1.name)
		println(sa2 + " " + sa2.name)
		if (sa1 == sa2) println("ok")
		if (sa1.equals(sa2)) println("ok")
	}

	def test2() {
		val xs = List(1,2,3)
		val ys = List(4,5,6)
		for((x,y) <- xs zip ys) println(x+y)  
	}
}
