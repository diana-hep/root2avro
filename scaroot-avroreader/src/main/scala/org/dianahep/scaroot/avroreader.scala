package org.dianahep.scaroot

import scala.collection.JavaConversions._
import scala.reflect.runtime.currentMirror
import scala.reflect.runtime.universe.NoSymbol
import scala.reflect.runtime.universe.Symbol
import scala.reflect.runtime.universe.typeOf
import scala.reflect.runtime.universe.TypeTag
import scala.util.Failure
import scala.util.Success

import org.apache.avro.Schema

import com.gensler.scalavro.types.AvroType

package avroreader {
  class RootTreeIterator[TYPE : TypeTag](fileLocations: Seq[String],
                                         treeLocation: String,
                                         includes: Seq[String] = Nil,
                                         libs: Seq[String] = Nil,
                                         inferTypes: Boolean = false,
                                         start: Long = 0L,
                                         end: Long = -1L,
                                         command: String = "./root2avro",
                                         environment: Map[String, String] = Map[String, String](),
                                         numberOfTrials: Int = 4) extends Iterator[TYPE] {
    if (fileLocations.isEmpty)
      throw new RuntimeException("Cannot build RootTreeIterator over an empty set of files.")
    if (start < 0)
      throw new IllegalArgumentException(s"The start ($start) must be greater than or equal to zero.")
    if (end >= 0  &&  start >= end)
      throw new IllegalArgumentException(s"If an ending index is given (greater than or equal to zero), then start ($start) must be less than end ($end).")

    private def packageName(sym: Symbol) = {
      def enclosingPackage(sym: Symbol): Symbol = {
        if (sym == NoSymbol) NoSymbol
        else if (sym.isPackage) sym
        else enclosingPackage(sym.owner)
      }
      val pkg = enclosingPackage(sym)
      if (pkg == currentMirror.EmptyPackageClass) ""
      else pkg.fullName
    }

    val name = typeOf[TYPE].typeSymbol.asClass.name.toString
    val ns = packageName(typeOf[TYPE].typeSymbol.asClass)

    def arguments(mode: String, index: Long = start) = List(command, fileLocations.head, treeLocation, "--mode=" + mode) ++
      (if (includes.isEmpty) Nil else List("--includes=" + includes.mkString(","))) ++
      (if (libs.isEmpty) Nil else List("--libs" + libs.mkString(","))) ++
      (if (!inferTypes) Nil else List("--inferTypes")) ++
      List("--start=" + index.toString) ++
      (if (end < 0L) Nil else List("--end=" + end.toString)) ++
      List("--name=" + name, "--ns=" + ns)

    val entryType = AvroType[Long]
    val dataType = AvroType[TYPE]
    val readerSchema = {
      val parser = new Schema.Parser
      parser.parse(dataType.schema.toString)
    }
    val writerSchema = {
      val processBuilder = new java.lang.ProcessBuilder(arguments("schema", 0L))
      val env = processBuilder.environment
      environment foreach {case (n, v) => env.put(n, v)}
      
      val process = processBuilder.start()
      val returnCode = process.waitFor

      val parser = new Schema.Parser
      parser.parse(process.getInputStream)
    }

    if (readerSchema != writerSchema)
      throw new RuntimeException(s"Reader's schema:\n    $readerSchema\ndoes not match writer's schema:\n    $writerSchema")

    private var entryIndex = -1L
    def index = entryIndex
    var process: Option[java.lang.Process] = None
    var avroStream: Option[java.io.InputStream] = None

    def restartProcess(index: Long) {
      println(s"restarting at $index")

      process.foreach(_.destroy())

      entryIndex = index
      if (end < 0L  ||  index < end) {
        val processBuilder = new java.lang.ProcessBuilder(arguments("avro-stream", index))
        val env = processBuilder.environment
        environment foreach {case (n, v) => env.put(n, v)}

        process = Some(processBuilder.start())
        avroStream = Some(process.get.getInputStream)
      }
    }
    restartProcess(start)

    private def getNext(trials: Int): Option[TYPE] =
      if (trials == 0) {
        process.foreach(_.destroy())
        process = None
        avroStream = None
        None
      }
      else avroStream match {
        case Some(stream) =>
          entryType.io read stream match {
            case Success(entry) if (entry == entryIndex) =>
              dataType.io read stream match {
                case Success(datum) =>
                  entryIndex += 1
                  Some(datum)                     // normal next
                case _ =>
                  restartProcess(entryIndex)      // couldn't parse Avro datum
                  getNext(trials - 1)
              }
            case Failure(_: java.io.EOFException) =>
              None                                // normal exit
            case _ =>
              restartProcess(entryIndex)          // wrong entry number
              getNext(trials - 1)
          }
        case None =>                              // stream is not open
          restartProcess(entryIndex)
          getNext(trials - 1)
      }

    private var theNext = getNext(numberOfTrials)

    def hasNext = !theNext.isEmpty
    def next() = {
      val out = theNext.get
      theNext = getNext(numberOfTrials)
      out
    }
  }
}
