package test.scala.scaroot

import scala.collection.mutable

import org.scalatest.FlatSpec
import org.scalatest.junit.JUnitRunner
import org.scalatest.Matchers

import org.dianahep.scaroot.reader._

class DefaultSuite extends FlatSpec with Matchers {
  "stuff" must "work" in {

    // case class Tree(event: Event)

    // case class Event(
    //   fType: String,
    //   fEventName: Option[String],
    //   fNtrack: Int,
    //   fNseg: Int,
    //   fNvertex: Int,
    //   fFlag: Long,
    //   fTemperature: Double,
    //   fMeasures: Seq[Int],
    //   fMatrix: Seq[Seq[Double]],
    //   fClosestDistance: Option[Double],
    //   fEvtHdr: EventHeader,
    //   fTracks: Option[Seq[Track]],
    //   fTriggerBits: TBits,
    //   fIsValid: Boolean)

    // case class EventHeader(fEvtNum: Int, fRun: Int, fDate: Int)

    // case class Track(
    //   fPx: Float,
    //   fPy: Float,
    //   fPz: Float,
    //   fRandom: Float,
    //   fMass2: Float,
    //   fBx: Float,
    //   fBy: Float,
    //   fMeanCharge: Float,
    //   fXfirst: Float,
    //   fXlast: Float,
    //   fYfirst: Float,
    //   fYlast: Float,
    //   fZfirst: Float,
    //   fZlast: Float,
    //   fCharge: Double,
    //   fVertex: Seq[Double],
    //   fNpoint: Int,
    //   fValid: Short,
    //   fNsp: Int,
    //   fPointValue: Option[Double],
    //   fTriggerBits: TBits)

    // case class TBits(fNbits: Long, fNbytes: Long, fAllBits: Option[Short])

    // val myclasses = Map("Event" -> My[Event], "EventHeader" -> My[EventHeader], "Track" -> My[Track], "TBits" -> My[TBits])

    // val iterator = RootTreeSingleIterator[Tree](List("../root2avro/test_Event/Event.root"), "T", List("../root2avro/test_Event/Event_cxx.so"), myclasses)

    // println(iterator.schema)

    // println(iterator.factory)

    // var i = 0
    // while (true) {
    //   val beforeTime = System.nanoTime
    //   var tmp: Tree = null

    //   while (iterator.hasNext) {
    //     tmp = iterator.next()
    //     if (i < 5) println(tmp)
    //     i += 1
    //   }

    //   println(s"seconds: ${(System.nanoTime - beforeTime) / 1e9} i: $i")
    //   iterator.reset()
    // }

    case class Tree(x: Int, y: Double, z: String)

    val myclasses = Map("t" -> My[Tree])

    val iterator = RootTreeSingleIterator[Tree](List("../root2avro/build/multipleLeaves.root"), "t")
    while (iterator.hasNext)
      println(iterator.next())

  }
}
