import tiger.ast._
import tiger.symbol._

object Tiger {
	def main(args: Array[String]) {
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
}
