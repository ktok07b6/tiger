package tiger.tree

import tiger.frame.Frame

abstract class Fragment
case class ProcFragment(stm:TreeStm, frame:Frame) extends Fragment
case class DataFragment(str:String) extends Fragment
