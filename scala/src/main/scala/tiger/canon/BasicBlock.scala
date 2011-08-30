package tiger.canon

import tiger.tree._
import tiger.common._
import scala.collection.mutable.ListBuffer

class BasicBlock(done:Label) {
	private var block:List[TreeStm] = List.empty
	private var blocks:List[Any] = List.empty

	def makeBlocks(stms:List[TreeStm]):List[List[TreeStm]] = {
		mkBlocks(stms)
		blocks.toList.asInstanceOf[List[List[TreeStm]]]
	}

	private def doStms(stms:List[TreeStm]):Unit = { 
		if (stms.isEmpty) {
			//BasicBlockの最後はdoneラベルへのジャンプ
			//doneは関数のepilogの開始を表す
			block = block :+ TreeJump(0, List(done))
			blocks = blocks :+ block
			block = List.empty
			return
		}
		val s = stms.head
		s match {
			case TreeJump(_,_) | TreeCjump(_,_,_,_,_) => {
				//ジャンプがくればBasicBlockを区切る
				block = block :+ s
				blocks = blocks :+ block
				block = List.empty
				mkBlocks(stms.tail)
			}
			case TreeLabel(label) => {
				//ラベルがくればそのラベルへのジャンプを挿入し再帰処理
				doStms(TreeJump(0, List(label)) :: stms)
			}
			case _ => {
				block = block :+ s
				doStms(stms.tail)
			}
		}
	}

	private def mkBlocks(stms:List[TreeStm]):Unit = {
		if (stms.isEmpty) { 
			return
		}
		stms.head match {
			case l:TreeLabel => {
				//ラベルがくればBasicBlockの開始
				block = List(l)
				doStms(stms.tail)
			}
			case _ => {
				//ラベルでなければ新規ラベルを挿入し再帰処理
				mkBlocks(TreeLabel(new Label()) :: stms)
			}
		}
	}

}
