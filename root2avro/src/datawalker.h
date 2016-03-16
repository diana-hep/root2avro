#ifndef DATAWALKER_H
#define DATAWALKER_H

// C++ includes
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>

// Avro includes
#ifdef AVRO
#include <avro.h>
#endif

// ROOT includes
#include <TClass.h>
#include <TClonesArray.h>
#include <TDataMember.h>
#include <TDictionary.h>
#include <TFile.h>
#include <TInterpreter.h>
#include <TLeafB.h>
#include <TLeafC.h>
#include <TLeafD.h>
#include <TLeafElement.h>
#include <TLeafF.h>
#include <TLeaf.h>
#include <TLeafI.h>
#include <TLeafL.h>
#include <TLeafObject.h>
#include <TLeafO.h>
#include <TLeafS.h>
#include <TList.h>
#include <TObjArray.h>
#include <TRefArray.h>
#include <TRef.h>
#include <TString.h>
#include <TTree.h>
#include <TTreeReaderArray.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

using namespace ROOT::Internal;
// using namespace ROOT;

//// forwards
class ClassWalker;

// Must be kept in-sync with scaroot-reader/src/main/scala/org/dianahep/scaroot/reader.scala!
enum SchemaElement {
  SchemaNull                 = 0,
  SchemaBoolean              = 1,
  SchemaInt                  = 2,
  SchemaLong                 = 3,
  SchemaFloat                = 4,
  SchemaDouble               = 5,
  SchemaString               = 6,
  SchemaBytes                = 7,

  SchemaArray                = 8,
  SchemaMap                  = 9,

  SchemaRecordName           = 10,
  SchemaRecordNamespace      = 11,
  SchemaRecordDoc            = 12,
  SchemaRecordFieldName      = 13,
  SchemaRecordFieldDoc       = 14,
  SchemaRecordEnd            = 15,

  SchemaEnumName             = 16,
  SchemaEnumNamespace        = 17,
  SchemaEnumDoc              = 18,
  SchemaEnumSymbol           = 19,
  SchemaEnumEnd              = 20,

  SchemaFixedName            = 21,
  SchemaFixedNamespace       = 22,
  SchemaFixedDoc             = 23,
  SchemaFixedSize            = 24,

  SchemaUnionStart           = 25,
  SchemaUnionEnd             = 26,

  SchemaReference            = 27,
};

typedef void (*SchemaBuilder)(SchemaElement schemaElement, const char *word);

///////////////////////////////////////////////////////////////////// FieldWalker

class FieldWalker {
public:
  std::string fieldName;
  std::string typeName;
  FieldWalker(std::string fieldName, std::string typeName);
  void printEscapedString(const char *string, std::ostream &stream);
  std::string escapedString(const char *string);
  virtual size_t sizeOf() = 0;
  virtual const std::type_info *typeId() = 0;
  virtual bool empty() = 0;
  virtual bool resolved() = 0;
  virtual void resolve(void *address) = 0;
  virtual std::string repr(int indent, std::set<std::string> &memo) = 0;
  virtual std::string avroTypeName() = 0;
  virtual std::string avroSchema(int indent, std::set<std::string> &memo) = 0;
  virtual void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) = 0;
  virtual void printJSON(void *address) = 0;
#ifdef AVRO
  virtual bool printAvro(void *address, avro_value_t *avrovalue) = 0;
#endif
};

///////////////////////////////////////////////////////////////////// PrimitiveWalkers

class PrimitiveWalker : public FieldWalker {
public:
  PrimitiveWalker(std::string fieldName, std::string typeName);
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroSchema(int indent, std::set<std::string> &memo);
  virtual void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) = 0;
  virtual void printJSON(void *address) = 0;
  virtual void printJSON(TTreeReaderArrayBase *readerArrayBase, int i) = 0;
#ifdef AVRO
  virtual bool printAvro(void *address, avro_value_t *avrovalue) = 0;
  virtual bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue) = 0;
#endif
  virtual TTreeReaderValueBase *readerValue(TTreeReader *reader) = 0;
  virtual TTreeReaderArrayBase *readerArray(TTreeReader *reader) = 0;
};

class BoolWalker : public PrimitiveWalker {
public:
  BoolWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class CharWalker : public PrimitiveWalker {
public:
  CharWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class UCharWalker : public PrimitiveWalker {
public:
  UCharWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class ShortWalker : public PrimitiveWalker {
public:
  ShortWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class UShortWalker : public PrimitiveWalker {
public:
  UShortWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class IntWalker : public PrimitiveWalker {
public:
  IntWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
  int value(TTreeReaderValueBase *readerValue);
};

class UIntWalker : public PrimitiveWalker {
public:
  UIntWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class LongWalker : public PrimitiveWalker {
public:
  LongWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class ULongWalker : public PrimitiveWalker {
public:
  ULongWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class FloatWalker : public PrimitiveWalker {
public:
  FloatWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class DoubleWalker : public PrimitiveWalker {
public:
  DoubleWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

///////////////////////////////////////////////////////////////////// AnyStringWalkers

class AnyStringWalker : public PrimitiveWalker {
public:
  AnyStringWalker(std::string fieldName, std::string typeName);
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
};

class CStringWalker : public AnyStringWalker {
public:
  CStringWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class StdStringWalker : public AnyStringWalker {
public:
  StdStringWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class TStringWalker : public AnyStringWalker {
public:
  TStringWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

///////////////////////////////////////////////////////////////////// MemberWalker

class MemberWalker : public FieldWalker {
public:
  size_t offset;
  FieldWalker *walker;
  std::string comment;

  MemberWalker(TDataMember *dataMember, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  static FieldWalker *specializedWalker(std::string fieldName, std::string typeName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
};

///////////////////////////////////////////////////////////////////// ClassWalker

class ClassWalker : public FieldWalker {
private:
  size_t sizeOf_;
  const std::type_info *typeId_;
public:
  TClass *tclass;
  std::string avroNamespace;
  std::map<const std::string, ClassWalker*> &defs;
  std::vector<MemberWalker*> members;

  ClassWalker(std::string fieldName, TClass *tclass, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  void fill();    // has side-effects, must be called soon after constructor

  std::vector<std::string> splitCppNamespace(std::string className);
  std::string dropCppNamespace(std::string className);
  std::string addCppNamespace(std::string className, std::string ns);

  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
};

///////////////////////////////////////////////////////////////////// PointerWalker

class PointerWalker : public FieldWalker {
public:
  FieldWalker *walker;

  PointerWalker(std::string fieldName, FieldWalker *walker);
  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
};

///////////////////////////////////////////////////////////////////// TRefWalker

class TRefWalker : public FieldWalker {
public:
  FieldWalker *walker;

  TRefWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
};

///////////////////////////////////////////////////////////////////// StdVectorWalker

class StdVectorWalker : public FieldWalker {
private:
  const std::type_info *typeId_;
public:
  FieldWalker *walker;

  StdVectorWalker(std::string fieldName, std::string typeName, FieldWalker *walker);
  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
};

///////////////////////////////////////////////////////////////////// StdVectorBoolWalker

class StdVectorBoolWalker : public FieldWalker {
public:
  FieldWalker *walker;

  StdVectorBoolWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
};

///////////////////////////////////////////////////////////////////// ArrayWalker

class ArrayWalker : public FieldWalker {
public:
  FieldWalker *walker;
  int numItems;

  ArrayWalker(std::string fieldName, FieldWalker *walker, int numItems);
  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
};

///////////////////////////////////////////////////////////////////// TObjArrayWalker

class TObjArrayWalker : public FieldWalker {
public:
  std::string avroNamespace;
  std::map<const std::string, ClassWalker*> &defs;
  FieldWalker *walker;
  TClass *classToAssert;

  TObjArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
};

///////////////////////////////////////////////////////////////////// TRefArrayWalker

class TRefArrayWalker : public FieldWalker {
public:
  std::string avroNamespace;
  std::map<const std::string, ClassWalker*> &defs;
  FieldWalker *walker;

  TRefArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
};

///////////////////////////////////////////////////////////////////// TClonesArrayWalker

class TClonesArrayWalker : public FieldWalker {
public:
  std::string avroNamespace;
  std::map<const std::string, ClassWalker*> &defs;
  FieldWalker *walker;

  TClonesArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
};

///////////////////////////////////////////////////////////////////// ExtractableWalker

class ExtractableWalker : public FieldWalker {
public:
#ifdef AVRO
  avro_value_t avroValue;
#endif

  ExtractableWalker(std::string fieldName, std::string typeName);
  bool empty();
  virtual void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) = 0;
  virtual void reset(TTreeReader *reader) = 0;
  virtual void *getAddress() = 0;
};

///////////////////////////////////////////////////////////////////// LeafWalker

class LeafDimension {
private:
  LeafDimension *next_;
  int size_;
  IntWalker *counter;
  TTreeReaderValueBase *counterReaderValue;
public:
  LeafDimension(LeafDimension *next, int size);
  LeafDimension(LeafDimension *next, IntWalker *counter, TTreeReader *reader);
  void reset(TTreeReader *reader);
  std::string repr();
  LeafDimension *next();   // linked list makes the recursive function in LeafWalker easier to understand
  int size();
  int flatSize();
};

class LeafWalker : public ExtractableWalker {
public:
  PrimitiveWalker *walker;
  TTreeReaderValueBase *readerValue;
  TTreeReaderArrayBase *readerArray;
  int dimensions;
  LeafDimension *dims;

  LeafWalker(TLeaf *tleaf, TTree *ttree, TTreeReader *reader);
  PrimitiveWalker *leafToPrimitive(TLeaf *tleaf);

  size_t sizeOf();
  const std::type_info *typeId();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  int printJSONDeep(int readerIndex, int readerSize, LeafDimension *dim);
  void printJSON(void *address);
#ifdef AVRO
  int printAvroDeep(int readerIndex, int readerSize, LeafDimension *dim, avro_value_t *avrovalue);
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
  void reset(TTreeReader *reader);
  void *getAddress();
};

///////////////////////////////////////////////////////////////////// ReaderValueWalker

class GenericReaderValue: public TTreeReaderValueBase {
public:
  std::string typeName;
  GenericReaderValue();
  GenericReaderValue(std::string fieldName, std::string typeName, TTreeReader *reader, FieldWalker *walker);
  const char *GetDerivedTypeName() const;
};

class ReaderValueWalker : public ExtractableWalker {
public:
  FieldWalker *walker;
  GenericReaderValue *value;

  ReaderValueWalker(std::string fieldName, TBranch *tbranch, TTreeReader *reader, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  size_t sizeOf();
  const std::type_info *typeId();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
  void reset(TTreeReader *reader);
  void *getAddress();
};

///////////////////////////////////////////////////////////////////// RawTBranchWalker

class RawTBranchWalker : public ExtractableWalker {
public:
  TBranch *tbranch;
  FieldWalker *walker;

  RawTBranchWalker(std::string fieldName, std::string typeName, FieldWalker *walker);
  size_t sizeOf();
  const std::type_info *typeId();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
};

class RawTBranchStdStringWalker : public RawTBranchWalker {
public:
  std::string *data;
  TTreeReader *reader;

  RawTBranchStdStringWalker(std::string fieldName, TTreeReader *reader);
  size_t sizeOf();
  const std::type_info *typeId();
  void reset(TTreeReader *reader);
  void *getAddress();
};

class RawTBranchTStringWalker : public RawTBranchWalker {
public:
  TString *data;
  TTreeReader *reader;

  RawTBranchTStringWalker(std::string fieldName, TTreeReader *reader);
  size_t sizeOf();
  const std::type_info *typeId();
  void reset(TTreeReader *reader);
  void *getAddress();
};

///////////////////////////////////////////////////////////////////// TreeWalker

class TreeWalker {
public:
  std::string fileLocation;
  std::string treeLocation;
  std::string avroNamespace;

  bool valid = false;
  std::string errorMessage = "";
  TFile *file;
  TTreeReader *reader;

  std::map<const std::string, ClassWalker*> defs;
  std::vector<ExtractableWalker*> fields;

#ifdef AVRO
  bool avroHeaderPrinted = false;
  avro_schema_t schema;
  avro_file_writer_t avroWriter;
  avro_value_iface_t *avroInterface;
  avro_value_t avroValue;
#endif

  TreeWalker(std::string fileLocation, std::string treeLocation, std::string avroNamespace, std::vector<std::string> libs);
  bool tryToOpenFile();
  void reset(std::string fileLocation);

  bool next();
  long numEntriesInCurrentTree();
  void setEntryInCurrentTree(long entry);

  bool resolved();
  void resolve();
  std::string repr();
  std::string avroSchema();
  void buildSchema(SchemaBuilder schemaBuilder);
  void printJSON();
#ifdef AVRO
  bool printAvroHeaderOnce(std::string &codec, int blockSize);
  bool printAvro();
  void closeAvro();
#endif
};

#endif // DATAWALKER_H
