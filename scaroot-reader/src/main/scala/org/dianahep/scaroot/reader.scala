package org.dianahep.scaroot

// import scala.language.existentials
import scala.language.experimental.macros
// import scala.language.higherKinds
import scala.reflect.macros.Context
import scala.reflect.runtime.universe.Type
import scala.reflect.runtime.universe.WeakTypeTag

import com.sun.jna.Pointer
import com.sun.jna.Memory
import com.sun.jna.NativeLong

import org.dianahep.scaroot.reader.schema._
import org.dianahep.scaroot.reader.factory._

package reader {
  /////////////////////////////////////////////////// class to use when no My[TYPE] is supplied

  class Generic(val fields: Map[String, Any]) {
    def apply(field: String): Any = fields(field)
    override def toString() = s"""Generic(Map(${fields.map({case (k, v) => "\"" + k + "\"" + " -> " + v.toString}).mkString(", ")}))"""
  }
  object Generic {
    def apply(fields: Map[String, Any]) = new Generic(fields)
    def unapply(x: Generic) = Some(x.fields)
  }

  /////////////////////////////////////////////////// user-friendly representation of non-String bytes (like Python 3)

  class Bytes(array: Array[Byte]) extends Seq[Byte] {
    def apply(idx: Int) = array(idx)
    def iterator = Iterator[Byte](array: _*)
    def length = array.size
    def decode = new String(array)
    def decode(charset: String) = new String(array, charset)
    override def toString() = s"""Bytes(${array.mkString(", ")})"""
  }
  object Bytes {
    def apply(array: Array[Byte]) = new Bytes(array)
    def apply(iterable: Iterable[Byte]) = new Bytes(iterable.toArray)
    def apply(bytes: Byte*) = new Bytes(bytes.toArray)
    def encode(str: String) = new Bytes(str.getBytes)
    def encode(str: String, charset: String) = new Bytes(str.getBytes(charset))
  }

  /////////////////////////////////////////////////// user's class specification (a factory-factory!)

  abstract class My[TYPE] {
    def name: String
    def fieldTypes: List[(String, Type)]
    def apply(factories: List[(String, Factory[_])]): FactoryClass[TYPE]
  }
  object My {
    def apply[TYPE]: My[TYPE] = macro applyImpl[TYPE]

    def applyImpl[TYPE : c.WeakTypeTag](c: Context): c.Expr[My[TYPE]] = {
      import c.universe._
      val dataClass = weakTypeOf[TYPE]
      val dataClassName = dataClass.toString

      val constructorParams = dataClass.declarations.collectFirst {
        case m: MethodSymbol if (m.isPrimaryConstructor) => m
      }.get.paramss.head

      var i = 0
      val fieldTypes = List.newBuilder[Tree]
      val getFields = List.newBuilder[Tree]

      constructorParams.foreach {param =>
        val name = param.asTerm.name.decodedName.toString
        val tpe = param.typeSignature
        fieldTypes += q"""$name -> weakTypeOf[$tpe]"""
        getFields += q"""factoryArray($i).asInstanceOf[Factory[$tpe]](byteBuffer)"""
        i += 1
      }

      c.Expr[My[TYPE]](q"""
        import java.nio.ByteBuffer

        import scala.reflect.runtime.universe.weakTypeOf

        import org.dianahep.scaroot.reader._
        import org.dianahep.scaroot.reader.schema._
        import org.dianahep.scaroot.reader.factory._

        new My[$dataClass] {
          // What you know at compile-time...
          val name = $dataClassName
          val fieldTypes = List(..${fieldTypes.result})

          def apply(factories: List[(String, Factory[_])]) =
            new FactoryClass[$dataClass](name, factories) {
              // What you know when you read a ROOT schema...
              // (I'll do the type-checking in the factory-builder, not here. Better error messages that way.)

              val factoryArray = factories.map(_._2).toArray

              // Fast runtime loop...
              def apply(byteBuffer: ByteBuffer) = {
                new $dataClass(..${getFields.result})
              }
            }
        }
      """)
    }
  }

  /////////////////////////////////////////////////// entry point for iterating over ROOT files

  class RootTreeIterator[TYPE](fileLocations: Seq[String],
                               treeLocation: String,
                               libs: Seq[String] = Nil,
                               myclasses: Map[String, My[_]] = Map[String, My[_]](),
                               start: Long = 0L,
                               end: Long = -1L,
                               run: Long = 1L,
                               skip: Long = 0L) extends Iterator[TYPE] {

    // FIXME: if TYPE is not Generic, add 'treeLocation -> My[TYPE]' to myclasses (note: that macro would have to be materialized implicitly; also, it's safe because this class already has a non-trivial constructor).

    private var libscpp = Pointer.NULL
    libs foreach {lib => libscpp = RootReaderCPPLibrary.addVectorString(libscpp, lib)}

    // Pack of state variables that all have to be kept in sync!
    // Limit user access to setIndex, reset(), and incrementIndex(), which should preserve interrelationships.
    private var done = true
    private var treeWalker = Pointer.NULL
    private var entryIndex = 0L
    private var fileIndex = 0
    private var entryInFileIndex = 0L

    private var entriesInFileArray = Array.fill[Long](fileLocations.size)(-1L)   // opening files is expensive
    private def entriesInFile(i: Int) = {
      if (entriesInFileArray(i) < 0) {
        RootReaderCPPLibrary.reset(treeWalker, fileLocations(i))
        entriesInFileArray(i) = RootReaderCPPLibrary.numEntriesInCurrentTree(treeWalker)
      }
      entriesInFileArray(i)
    }

    def index = entryIndex

    // Go to a random position (not a common feature for an Iterator to have, but useful, particularly for implementing "start").
    def setIndex(index: Long) {
      entryIndex = 0L
      fileIndex = 0
      entryInFileIndex = 0L
      while (entryIndex < index) {
        if (fileIndex >= entriesInFileArray.size) {
          done = true
          entryIndex = -1L
          throw new IllegalArgumentException(s"Total number of entries is ${entriesInFileArray.sum}, so $index would be beyond the last.")
        }
        if (entryIndex + entriesInFile(fileIndex) <= index) {
          fileIndex += 1
          entryIndex += entriesInFile(fileIndex)
        }
        else {
          entryInFileIndex = index - entryIndex
          entryIndex = index
        }
      }
      RootReaderCPPLibrary.reset(treeWalker, fileLocations(fileIndex))
    }

    def reset() { setIndex(0L) }  // synonym

    // Go forward by one (the usual case).
    def incrementIndex() {
      entryIndex += 1L
      entryInFileIndex += 1L
      if (entryInFileIndex >= entriesInFile(fileIndex)) {
        fileIndex += 1
        entryInFileIndex = 0L
        if (fileIndex >= entriesInFileArray.size)
          done = true
        else
          RootReaderCPPLibrary.reset(treeWalker, fileLocations(fileIndex))
      }
    }

    val schema: SchemaClass =
      if (!fileLocations.isEmpty) {
        treeWalker = RootReaderCPPLibrary.newTreeWalker(fileLocations(0), treeLocation, "", libscpp)

        if (RootReaderCPPLibrary.valid(treeWalker) == 0)
          throw new RuntimeException(RootReaderCPPLibrary.errorMessage(treeWalker))

        done = (RootReaderCPPLibrary.next(treeWalker) == 0)
        while (!done  &&  RootReaderCPPLibrary.resolved(treeWalker) == 0) {
          RootReaderCPPLibrary.resolve(treeWalker)
          done = (RootReaderCPPLibrary.next(treeWalker) == 0)
        }

        Schema(treeWalker)
      }
      else
        throw new RuntimeException("Cannot build RootTreeIterator over an empty set of files.")

    val factory = FactoryClass[TYPE](schema, myclasses)

    setIndex(start)

    private var bufferSize = new NativeLong(64*1024)
    private var buffer = new Memory(bufferSize.longValue)
    private var byteBuffer = buffer.getByteBuffer(0, bufferSize.longValue)
    private var statusByte = 1.toByte

    def hasNext = !done
    def next() = {
      if (done)
        throw new RuntimeException("next() called on empty RootTreeIterator (create a new one to run over the data again)")

      // Set the status byte to 1 (writing) and let C++ write to the buffer.
      statusByte = 1
      buffer.setByte(0, statusByte)
      RootReaderCPPLibrary.copyToBuffer(treeWalker, entryInFileIndex, buffer, bufferSize)
      byteBuffer.rewind()

      // Check the status byte to find out if copying failed due to a buffer that's too small (the only error we handle).
      statusByte = byteBuffer.get
      while (statusByte == 2) {
        // Get a new, bigger buffer (and let the old one be garbage collected).
        bufferSize = new NativeLong(bufferSize.longValue * 2L)
        buffer = new Memory(bufferSize.longValue)
        byteBuffer = buffer.getByteBuffer(0, bufferSize.longValue)

        // Try, try again.
        statusByte = 1
        buffer.setByte(0, statusByte)
        RootReaderCPPLibrary.copyToBuffer(treeWalker, entryInFileIndex, buffer, bufferSize)
        byteBuffer.rewind()
        statusByte = byteBuffer.get
      }

      // Interpret the data in the buffer, creating Scala objects.
      val out = factory(byteBuffer)

      // Increment the counter and see if it's time to step to the next file.
      incrementIndex()

      out
    }
  }
  object RootTreeIterator {
    def apply[TYPE](fileLocations: Seq[String],
                    treeLocation: String,
                    libs: Seq[String] = Nil,
                    myclasses: Map[String, My[_]] = Map[String, My[_]](),
                    start: Long = 0L,
                    end: Long = -1L,
                    run: Long = 1L,
                    skip: Long = 0L) =
      new RootTreeIterator[TYPE](fileLocations, treeLocation, libs, myclasses, start, end, run, skip)
  }
}
