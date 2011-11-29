import sbt._
import reaktor.scct.ScctProject
class Project(info:ProjectInfo) extends DefaultProject(info) with ScctProject {
	val scalaTest = "org.scalatest" % "scalatest_2.8.1" % "1.5.1"
}
