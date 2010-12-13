package tiger.typ;

import tiger.ast._
import tiger.symbol._

object TypeCheck {
	def typeCheck(ast:AST):Type = ast match {
		case v:SimpleVar => {
			val ve:VarEntry = Table.getVarEntry(v.sym).get
			v.varEntry = ve
			ve.typ
		}
		case v:FieldVar => {
			val ve:VarEntry = Table.getVarEntry(v.sym).get
			val t:Type = typeCheck(v.va)
			val r:Record = t.actual().asInstanceOf[Record]
			//TODO:find v.sym in r
			v.varEntry = ve
			r.fieldType
		}
		case v:SubscriptVar => {
			val ve:VarEntry = Table.getVarEntry(v.getSymbol).get
			val expt:Type = typeCheck(v.exp)
			require(expt.actual().isInstanceOf[Int])
			val vat:Type = typeCheck(v.va)
			val arrayT:Array = vat.actual().asInstanceOf[Array]
			v.varEntry = ve
			arrayT.element
		}
		case e:VarExp => {
			null
		}
		case e:NilExp => {
			null
		}
		case e:IntExp => {
			null
		}
		case e:StringExp => {
			null
		}
		case e:CallExp => {
			null
		}
		case e:PlusOpExp => {
			null
		}
		case e:MinusOpExp => {
			null
		}
		case e:TimesOpExp => {
			null
		}
		case e:DivideOpExp => {
			null
		}
		case e:EqOpExp => {
			null
		}
		case e:NeOpExp => {
			null
		}
		case e:LtOpExp => {
			null
		}
		case e:GtOpExp => {
			null
		}
		case e:LeOpExp => {
			null
		}
		case e:GeOpExp => {
			null
		}
		case e:AndOpExp => {
			null
		}
		case e:OrOpExp => {
			null
		}
		case e:RecordExp => {
			null
		}
		case e:SeqExp => {
			null
		}
		case e:AssignExp => {
			null
		}
		case e:IfExp => {
			null
		}
		case e:WhileExp => {
			null
		}
		case e:ForExp => {
			null
		}
		case e:BreakExp => {
			null
		}
		case e:LetExp => {
			null
		}
		case e:ArrayExp => {
			null
		}
		case d:FunDec => {
			null
		}
		case d:VarDec => {
			null
		}
		case d:TypeDec => {
			null
		}
		case t:NameTy => {
			null
		}
		case t:RecordTy => {
			null
		}
		case t:ArrayTy => {
			null
		}
		case f:TypeField => {
			null
		}
		case f:RecordField => {
			null
		}
		case _ => null
	}
}
