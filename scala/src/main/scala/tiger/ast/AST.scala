package tiger.ast;

import tiger.symbol._
import tiger.typ._

abstract class AST

abstract class ASTVar extends AST {
	var varEntry:VarEntry = null;
	def getSymbol(): Symbol
}
abstract class ASTExp extends AST
abstract class ASTDec extends AST
abstract class ASTType extends AST

//Vars
case class SimpleVar(sym:Symbol) extends ASTVar {
	override def getSymbol() = sym
}
case class FieldVar(va:ASTVar, sym:Symbol) extends ASTVar {
	override def getSymbol() = va.getSymbol
}
case class SubscriptVar(va:ASTVar, exp:ASTExp) extends ASTVar {
	override def getSymbol() = va.getSymbol
}
//Exps
case class VarExp(va:ASTVar) extends ASTExp
case class NilExp() extends ASTExp
case class IntExp(n:Int) extends ASTExp
case class StringExp(s:String) extends ASTExp
case class CallExp(func:Symbol, exps:List[ASTExp]) extends ASTExp {
	var funcEntry:FuncEntry = null
}
object Oper extends Enumeration {
	val Plus, Minus, Times, Divide, Eq, Ne, Lt, Gt, Le, Ge, And, Or = Value
}
case class OpExp(op:Oper.Value, l:ASTExp, r:ASTExp) extends ASTExp
case class RecordExp(typ:Symbol, fields:List[RecordField]) extends ASTExp
case class SeqExp(seq:List[ASTExp]) extends ASTExp
case class AssignExp(va:ASTVar, exp:ASTExp) extends ASTExp
case class IfExp(test:ASTExp, thenexp:ASTExp, elseexp:ASTExp) extends ASTExp
case class WhileExp(test:ASTExp, body:ASTExp) extends ASTExp
case class ForExp(va:Symbol, lo:ASTExp, hi:ASTExp, body:ASTExp) extends ASTExp {
	var varEntry:VarEntry = null
	var escape:Boolean = false
}
case class BreakExp() extends ASTExp
case class LetExp(decs:List[ASTDec], body:List[ASTExp]) extends ASTExp
case class ArrayExp(typ:Symbol, size:ASTExp, init:ASTExp) extends ASTExp

//Decs
case class FunDec(name:Symbol, params:List[TypeField], result:Symbol, body:ASTExp) extends ASTDec {
	var funcEntry:FuncEntry = null
}
case class VarDec(name:Symbol, typ:Symbol, init:ASTExp) extends ASTDec {
	var varEntry:VarEntry = null
	var escape:Boolean = false
}
case class TypeDec(name:Symbol, typ:ASTType) extends ASTDec

//Types
case class NameTy(name:Symbol) extends ASTType
case class RecordTy(fields:List[TypeField]) extends ASTType
case class ArrayTy(name:Symbol) extends ASTType

case class TypeField(name:Symbol, typ:Symbol) extends AST {
	var varEntry:VarEntry = null
	var escape:Boolean = false
}
case class RecordField(name:Symbol, init:ASTExp) extends AST {
	var varEntry:VarEntry = null
}
