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
	override def getSymbol() = sym
}
case class SubscriptVar(va:ASTVar, exp:ASTExp) extends ASTVar {
	override def getSymbol() = va.getSymbol
}
//Exps
case class VarExp(va:ASTVar) extends ASTExp
case class NilExp() extends ASTExp
case class IntExp(n:Int) extends ASTExp
case class StringExp(s:String) extends ASTExp
case class CallExp(func:Symbol, funcEntry:FuncEntry, exps:List[ASTExp]) extends ASTExp
case class PlusOpExp(l:ASTExp, r:ASTExp) extends ASTExp
case class MinusOpExp(l:ASTExp, r:ASTExp) extends ASTExp
case class TimesOpExp(l:ASTExp, r:ASTExp) extends ASTExp
case class DivideOpExp(l:ASTExp, r:ASTExp) extends ASTExp
case class EqOpExp(l:ASTExp, r:ASTExp) extends ASTExp
case class NeOpExp(l:ASTExp, r:ASTExp) extends ASTExp
case class LtOpExp(l:ASTExp, r:ASTExp) extends ASTExp
case class GtOpExp(l:ASTExp, r:ASTExp) extends ASTExp
case class LeOpExp(l:ASTExp, r:ASTExp) extends ASTExp
case class GeOpExp(l:ASTExp, r:ASTExp) extends ASTExp
case class AndOpExp(l:ASTExp, r:ASTExp) extends ASTExp
case class OrOpExp(l:ASTExp, r:ASTExp) extends ASTExp
case class RecordExp(list:List[RecordField], typ:Symbol) extends ASTExp
case class SeqExp(seq:List[ASTExp]) extends ASTExp
case class AssignExp(va:ASTVar, exp:ASTExp) extends ASTExp
case class IfExp(test:ASTExp, thenexp:ASTExp, elseexp:ASTExp) extends ASTExp
case class WhileExp(test:ASTExp, body:ASTExp) extends ASTExp
case class ForExp(va:Symbol, varEntry:VarEntry, escape:Boolean, lo:ASTExp, hi:ASTExp, body:ASTExp) extends ASTExp
case class BreakExp() extends ASTExp
case class LetExp(decs:List[ASTDec], body:SeqExp) extends ASTExp
case class ArrayExp(typ:Symbol, size:ASTExp, init:ASTExp) extends ASTExp

//Decs
case class FunDec(name:Symbol, funcEntry:FuncEntry, params:List[TypeField], result:Symbol, body:ASTExp) extends ASTDec
case class VarDec(name:Symbol, varEntry:VarEntry, escape:Boolean, typ:Symbol, init:ASTExp) extends ASTDec
case class TypeDec(name:Symbol, typ:ASTType) extends ASTDec

//Types
case class NameTy(name:Symbol) extends ASTType
case class RecordTy(fields:List[TypeField]) extends ASTType
case class ArrayTy(name:Symbol) extends ASTType

case class TypeField(name:Symbol, escape:Boolean, typ:Symbol, varEntry:VarEntry) extends AST
case class RecordField(name:Symbol, varEntry:VarEntry, init:ASTExp) extends AST
