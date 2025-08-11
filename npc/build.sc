import mill._
import mill.scalalib._

val defaultScalaVersion = "2.13.15"
val chiselVersion = "6.6.0"

trait HasChisel extends SbtModule {
  override def scalaVersion = defaultScalaVersion

  override def scalacOptions = super.scalacOptions() ++
    Agg("-language:reflectiveCalls", "-Ymacro-annotations", "-Ytasty-reader")

  override def ivyDeps = super.ivyDeps() ++ Agg(
    ivy"org.chipsalliance::chisel:${chiselVersion}"
  )

  override def scalacPluginIvyDeps = super.scalacPluginIvyDeps() ++ Agg(
    ivy"org.chipsalliance:::chisel-plugin:${chiselVersion}"
  )
}

object npc extends HasChisel {
  override def millSourcePath = millOuterCtx.millSourcePath

  override def sources = T.sources(millSourcePath / "src" / "main" / "scala")
  override def resources = T.sources(millSourcePath / "src" / "main" / "resources")
}
