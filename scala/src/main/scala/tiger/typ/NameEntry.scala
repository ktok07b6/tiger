package tiger.typ;

import tiger.common.Level
import tiger.common.LevelAccess

abstract class NameEntry;
case class VarEntry(typ:Type) extends NameEntry {
	var access:LevelAccess = null
}
case class FuncEntry(result:Type, params:List[Type]) extends NameEntry

