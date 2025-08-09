import mill._
import mill.scalalib._

trait NpcChiselModule extends SbtModule {
  override def scalaVersion = "2.13.15"

  val chiselVersion = "6.6.0"

  override def ivyDeps = Agg(
    ivy"org.chipsalliance::chisel:${chiselVersion}"
  )

  override def scalacPluginIvyDeps = Agg(
    ivy"org.chipsalliance:::chisel-plugin:${chiselVersion}"
  )

  override def scalacOptions = Seq(
    "-language:reflectiveCalls",
    "-Ymacro-annotations",
    "-Ytasty-reader"
  )
}

object npc extends NpcChiselModule {
  override def sources = T.sources(millSourcePath / "src" / "main" / "scala")

  override def resources = T.sources(millSourcePath / "src" / "main" / "resources")
}
