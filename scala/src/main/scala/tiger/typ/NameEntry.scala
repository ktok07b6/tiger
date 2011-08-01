package tiger.typ;

abstract class NameEntry;
case class VarEntry(typ:Type) extends NameEntry
case class FuncEntry(result:Type, params:List[Type]) extends NameEntry
