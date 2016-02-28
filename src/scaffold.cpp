#include "scaffold.h"

namespace scaffold {
  /////////////////////////////////////////////////////////////////////////////////// helper functions

  std::string rootDummy(std::string fieldName) {
    return std::string("root_") + fieldName;
  }

  std::string indentation(int indent) {
    std::string out;
    for (int i = 0;  i < indent;  i++)
      out += std::string(" ");
    return out;
  }

  std::string innerPrintJSON(std::string item, Def *def, std::string innerType) {
    if (def != nullptr)
      return item + std::string(".printJSON()");
    else if (innerType == std::string("bool")  ||  innerType == std::string("Bool_t"))
      return std::string("b(") + item + std::string(")");
    else if (innerType == std::string("char")  ||  innerType == std::string("Char_t"))
      return std::string("d(") + item + std::string(")");   // Char_t -> integer for our purposes
    else if (innerType == std::string("short")  ||  innerType == std::string("Short_t"))
      return std::string("hd(") + item + std::string(")");
    else if (innerType == std::string("int")  ||  innerType == std::string("Int_t"))
      return std::string("d(") + item + std::string(")");
    else if (innerType == std::string("long")  ||  innerType == std::string("Long_t")  ||  innerType == std::string("Long64_t"))
      return std::string("ld(") + item + std::string(")");
    else if (innerType == std::string("float")  ||  innerType == std::string("Float_t")  ||  innerType == std::string("Float16_t"))
      return std::string("fg(") + item + std::string(")");
    else if (innerType == std::string("double")  ||  innerType == std::string("Double_t")  ||  innerType == std::string("Double32_t"))
      return std::string("g(") + item + std::string(")");
    else if (innerType == std::string("unsigned char")  ||  innerType == std::string("UChar_t"))
      return std::string("u(") + item + std::string(")");   // Char_t -> integer for our purposes
    else if (innerType == std::string("unsigned short")  ||  innerType == std::string("UShort_t"))
      return std::string("hu(") + item + std::string(")");
    else if (innerType == std::string("unsigned int")  ||  innerType == std::string("UInt_t"))
      return std::string("u(") + item + std::string(")");
    else if (innerType == std::string("unsigned long")  ||  innerType == std::string("ULong_t")  ||  innerType == std::string("ULong64_t"))
      return std::string("lu(") + item + std::string(")");
    else if (innerType == std::string("string"))
      return std::string("s(\"\\\"\"); s(escapeJSON(") + item + std::string(").c_str()); s(\"\\\"\")");
    else if (innerType == std::string("TString"))
      return std::string("s(\"\\\"\"); s(escapeJSON(std::string(") + item + std::string(".Data())).c_str()); s(\"\\\"\")");
    else 
      throw std::invalid_argument(std::string("unrecognized type (A): ") + innerType);
  }

  std::string unrollTemplatesPrintJSON(int indent, std::string item, std::string dummy, std::vector<Template> templates, Def *def, std::string innerType) {
    std::string out;
    for (int i = 0;  i < templates.size();  i++)
      if (templates[i] == vectorOf) {
        std::string len = std::string("len") + std::to_string(i) + std::string("_") + dummy;
        std::string index = std::string("i") + std::to_string(i);
        std::string newitem = std::string("item") + std::to_string(i) + std::string("_") + dummy;
        out += indentation(indent + 2*i) + std::string("int ") + len + std::string(" = ") + item + std::string(".size();\n") +
               indentation(indent + 2*i) + std::string("for (int ") + index + std::string(" = 0;  ") + index + std::string(" < ") + len + std::string(";  ") + index + std::string("++) {\n") +
               indentation(indent + 2*i) + std::string("  auto ") + newitem + std::string(" = ") + item + std::string("[") + index + std::string("];\n") +
               indentation(indent + 2*i) + std::string("  if (") + index + std::string(" > 0) s(\", \");\n");
        if (i < templates.size() - 1)
          out += indentation(indent + 2*i) + std::string("  s(\"[\");\n");
        item = newitem;
      }
    out += indentation(indent + 2*templates.size()) + innerPrintJSON(item, def, innerType) + std::string(";\n");
    for (int i = templates.size() - 1;  i >= 0;  i--)
      if (templates[i] == vectorOf) {
        if (i < templates.size() - 1)
          out += indentation(indent + 2*i) + std::string("  s(\"]\");\n");
        out += indentation(indent + 2*i) + std::string("}\n");
      }
    return out;
  }

  std::string innerSchema(int indent, std::string ns, Def *def, std::string innerType) {
    if (def != nullptr)
      return def->schema(indent + 2, ns);
    else if (innerType == std::string("bool")  ||  innerType == std::string("Bool_t"))
      return std::string("\"boolean\"");
    else if (innerType == std::string("char")  ||  innerType == std::string("Char_t"))
      return std::string("\"int\"");
    else if (innerType == std::string("short")  ||  innerType == std::string("Short_t"))
      return std::string("\"int\"");
    else if (innerType == std::string("int")  ||  innerType == std::string("Int_t"))
      return std::string("\"int\"");
    else if (innerType == std::string("long")  ||  innerType == std::string("Long_t")  ||  innerType == std::string("Long64_t"))
      return std::string("\"long\"");
    else if (innerType == std::string("float")  ||  innerType == std::string("Float_t")  ||  innerType == std::string("Float16_t"))
      return std::string("\"float\"");
    else if (innerType == std::string("double")  ||  innerType == std::string("Double_t")  ||  innerType == std::string("Double32_t"))
      return std::string("\"double\"");
    else if (innerType == std::string("unsigned char")  ||  innerType == std::string("UChar_t"))
      return std::string("\"int\"");
    else if (innerType == std::string("unsigned short")  ||  innerType == std::string("UShort_t"))
      return std::string("\"int\"");
    else if (innerType == std::string("unsigned int")  ||  innerType == std::string("UInt_t"))
      return std::string("\"long\"");
    else if (innerType == std::string("unsigned long")  ||  innerType == std::string("ULong_t")  ||  innerType == std::string("ULong64_t"))
      return std::string("\"double\"");
    else
      throw std::invalid_argument(std::string("unrecognized type (B): ") + innerType);
  }

  std::string unrollTemplatesSchema(int indent, std::string ns, std::vector<Template> templates, Def *def, std::string innerType) {
    std::string out;
    for (int i = 0;  i < templates.size();  i++)
      if (templates[i] == vectorOf) {
        out += std::string("\n") + indentation(indent + 2*i + 2) + std::string("{\"type\": \"array\", \"items\": ");
      }
    out += innerSchema(indent + 2*templates.size(), ns, def, innerType);
    for (int i = templates.size() - 1;  i >= 0;  i--)
      if (templates[i] == vectorOf) {
        out += std::string("}\n") + indentation(indent + 2*i);
      }
    return out;
  }

  /////////////////////////////////////////////////////////////////////////////////// class member definitions

  ////////////////////////////////////// Type

  Type::Type(std::string type, Kind kind) {
    if (kind == array) {
      int i;
      for (i = 0;  i < type.size()  &&  type[i] != '[';  i++)
        type_ += type[i];
      for (;  i < type.size();  i++) {
        if (type[i] == '[')
          dims_.push_back(0);
        else if ('0' <= type[i]  &&  type[i] <= '9'  &&  dims_.size() > 0)
          dims_.back() = 10 * dims_.back() + ((int)type[i] - (int)'0');
      }
    }
    else
      type_ = type;
    kind_ = kind;
  }

  std::string Type::typeName() { return type_; }

  std::string Type::arrayBrackets() {
    std::string out;
    for (int i = 0;  i < dims_.size();  i++)
      out += std::string("[") + std::to_string(i) + std::string("]");
    return out;
  }

  std::string Type::printJSON(int indent) { return std::string(); }

  // std::string loop(std::string name) {
  //   if (kind_ == scalar)
  //     return name + std::string(" << std::endl;\n");
  //   else if (kind_ == array  ||  kind_ == vector) {
  //     std::string out;
  //     for (int n = 0;  n < dims_.size();  n++) {
  //       std::string var = std::string("i") + std::to_string(n);
  //       for (int i = 0;  i < n;  i++)
  //         out += std::string("  ");
  //       out += std::string("for (int ") + var + std::string(" = 0;  ") + var + std::string(" < ") + std::to_string(dims_[n]) + std::string(";  ") + var + std::string("++)\n");
  //     }
  //     for (int i = 0;  i < dims_.size();  i++)
  //       out += std::string("  ");
  //     out += std::string("std::cout << \" \" << ") + name;
  //     for (int n = 0;  n < dims_.size();  n++) {
  //       std::string var = std::string("i") + std::to_string(n);
  //       out += std::string("[") + var + std::string("]");
  //     }
  //     out += std::string(";\n");
  //     out += std::string("  std::cout << std::endl;\n");
  //   }
  //   else if (kind_ == structure) {
  //   }
  // }

  ////////////////////////////////////// Def

  Def::Def(std::string typeName) : typeName_(typeName) { }

  std::string Def::typeName() { return typeName_; }

  std::string Def::name(int i) { return names_[i]; }

  Type Def::type(int i) { return types_[i]; }

  void Def::addBase(std::string base) {
    bases_.push_back(base);
  }

  void Def::addField(Type type, std::string name) {
    names_.push_back(name);
    types_.push_back(type);
  }

  bool Def::empty() {
    return (bases_.size() == 0  &&  types_.size() == 0);
  }

  std::string Def::forward() {
    if (empty())
      return std::string();
    else
      return std::string("class ") + typeName_ + std::string(";\n");
  }

  std::string Def::def() {
    if (empty())
      return std::string();
    else {
      std::string out = std::string("class ") + typeName_;
      for (int i = 0;  i < bases_.size();  i++) {
        if (i == 0)
          out += std::string(" : ");
        else
          out += std::string(", ");
        out += std::string("public ") + bases_[i];
      }
      out += std::string(" {\n");
      for (int i = 0;  i < names_.size();  i++) {
        out += std::string("  ") + type(i).typeName() + std::string(" ") + name(i) + type(i).arrayBrackets() + std::string(";\n");
      }
      out += std::string("  void printJSON() {\n");
      for (int i = 0;  i < names_.size();  i++) {
        // out += std::string("    std::cout << \"") + name(i) + std::string(" \";\n") + type(i).loop(name(i));
      }
      out += std::string("  }\n};\n");
      return out;
    }
  }

  std::string Def::ref() { return typeName_; }

  std::string Def::schema(int indent, std::string ns) {
    return std::string();
  }

  ////////////////////////////////////// ReaderValueNode

  ReaderValueNode::ReaderValueNode(std::string type, std::string name, Def *def) : type_(type), name_(name), def_(def) {
    std::string vectorOfPrefix("vector<");
    int start = 0;
    while (true)
      if (type_.substr(start, vectorOfPrefix.size()) == vectorOfPrefix) {
        templates_.push_back(vectorOf);
        start += vectorOfPrefix.size();
      }
      else
        break;
    int stop;
    for (stop = start;  stop < type_.size()  &&  type_[stop] != '>';  stop++);
    innerType_ = type_.substr(start, stop - start);
  }

  std::string ReaderValueNode::declare(int indent) {
    return indentation(indent) + std::string("TTreeReaderValue<") + type_ + std::string(" > *") + rootDummy(name_) + std::string(";\n");
  }

  std::string ReaderValueNode::init(int indent) {
    return indentation(indent) + rootDummy(name_) + " = new " + std::string("TTreeReaderValue<") + type_ + std::string(" >(*getReader(), \"") + name_ + std::string("\");\n");
  }

  std::string ReaderValueNode::printJSON(int indent) {
    std::string item = std::string("item_") + rootDummy(name_);
    return indentation(indent) + std::string("s(\"\\\"") + name_ + std::string("\\\": \");\n") +
           indentation(indent) + std::string("auto ") + item + std::string(" = *(") + rootDummy(name_) + std::string("->Get());\n") +
           unrollTemplatesPrintJSON(indent, item, rootDummy(name_), templates_, def_, innerType_);
  }

  std::string ReaderValueNode::schema(int indent, std::string ns) {
    return indentation(indent) + std::string("{\"name\": \"") + name_ + std::string("\", \"type\": ") +
           unrollTemplatesSchema(indent, ns, templates_, def_, innerType_) +
           std::string("}");
  }

  ////////////////////////////////////// ReaderStringNode

  ReaderStringNode::ReaderStringNode(std::string name) : name_(name) { }

  std::string ReaderStringNode::declare(int indent) {
    return indentation(indent) + std::string("TTreeReaderArray<Char_t > *") + rootDummy(name_) + std::string(";\n");
  }

  std::string ReaderStringNode::init(int indent) {
    return indentation(indent) + rootDummy(name_) + " = new " + std::string("TTreeReaderArray<Char_t >(*getReader(), \"") + name_ + std::string("\");\n");
  }

  std::string ReaderStringNode::printJSON(int indent) {
    return indentation(indent) + std::string("s(\"\\\"") + name_ + std::string("\\\": \\\"\"); s(escapeJSON(std::string((char*)") + rootDummy(name_) + std::string("->GetAddress())).c_str()); s(\"\\\"\");\n");
  }

  std::string ReaderStringNode::schema(int indent, std::string ns) {
    return indentation(indent) + std::string("{\"name\": \"") + name_ + std::string("\", \"type\": \"string\"}");
  }

  ////////////////////////////////////// ReaderVectorBoolNode

  ReaderVectorBoolNode::ReaderVectorBoolNode(std::string name) : name_(name) { }

  std::string ReaderVectorBoolNode::declare(int indent) {
    return indentation(indent) + std::string("TTreeReaderValue<std::vector<bool> > *") + rootDummy(name_) + std::string(";\n");
  }

  std::string ReaderVectorBoolNode::init(int indent) {
    return indentation(indent) + rootDummy(name_) + " = new " + std::string("TTreeReaderValue<std::vector<bool> >(*getReader(), \"") + name_ + std::string("\");\n");
  }

  std::string ReaderVectorBoolNode::printJSON(int indent) {
    return indentation(indent) + std::string("s(\"\\\"") + name_ + std::string("\\\": [\");\n") +
           indentation(indent) + std::string("std::vector<bool> item_") + rootDummy(name_) + std::string(" = *(") + rootDummy(name_) + std::string("->Get());\n") +
           indentation(indent) + std::string("int len_") + rootDummy(name_) + std::string(" = item_") + rootDummy(name_) + std::string(".size();\n") +
           indentation(indent) + std::string("for (int i = 0;  i < len_") + rootDummy(name_) + std::string("; i++) {\n") +
           indentation(indent) + std::string("  if (i != 0) s(\", \");\n") +
           indentation(indent) + std::string("  b(item_") + rootDummy(name_) + std::string("[i]);\n") +
           indentation(indent) + std::string("}\n") +
           indentation(indent) + std::string("s(\"]\");\n");
  }

  std::string ReaderVectorBoolNode::schema(int indent, std::string ns) {
    return indentation(indent) + std::string("{\"name\": \"") + name_ + std::string("\", \"type\": {\"type\": \"array\", \"items\": \"boolean\"}}");
  }

  ////////////////////////////////////// ReaderArrayNode

  ReaderArrayNode::ReaderArrayNode(std::string type, std::string name, Def *def) : type_(type), name_(name), def_(def) {
    std::string vectorOfPrefix("vector<");
    int start = 0;
    while (true)
      if (type_.substr(start, vectorOfPrefix.size()) == vectorOfPrefix) {
        templates_.push_back(vectorOf);
        start += vectorOfPrefix.size();
      }
      else
        break;
    int stop;
    for (stop = start;  stop < type_.size()  &&  type_[stop] != '>';  stop++);
    innerType_ = type_.substr(start, stop - start);
  }

  std::string ReaderArrayNode::declare(int indent) {
    return indentation(indent) + std::string("TTreeReaderArray<") + type_ + std::string(" > *") + rootDummy(name_) + std::string(";\n");
  }

  std::string ReaderArrayNode::init(int indent) {
    return indentation(indent) + rootDummy(name_) + " = new " + std::string("TTreeReaderArray<") + type_ + std::string(" >(*getReader(), \"") + name_ + std::string("\");\n");
  }

  std::string ReaderArrayNode::printJSON(int indent) {
    std::string item = std::string("item_") + rootDummy(name_);
    return indentation(indent) + std::string("s(\"\\\"") + name_ + std::string("\\\": [\");\n") +
           indentation(indent) + std::string("int len_") + rootDummy(name_) + std::string(" = ") + rootDummy(name_) + std::string("->GetSize();\n") +
           indentation(indent) + std::string("for (int i = 0;  i < len_" ) + rootDummy(name_) + std::string(";  i++) {\n") +
           indentation(indent) + std::string("  if (i != 0) s(\", \");\n") +
           indentation(indent) + std::string("  auto ") + item + std::string(" = (*") + rootDummy(name_) + std::string(")[i];\n") +
           unrollTemplatesPrintJSON(indent + 2, item, rootDummy(name_), templates_, def_, innerType_) +
           indentation(indent) + std::string("}\n") +
           indentation(indent) + std::string("s(\"]\");\n");

    // out += indentation(indent) + std::string("s(\"\\\"") + name_ + std::string("\\\": [\");\n") +
    //   indentation(indent) + std::string("int len_") + rootDummy(name_) + std::string(" = ") + rootDummy(name_) + std::string("->GetSize();\n") +
    //   indentation(indent) + std::string("for (int i = 0;  i < len_") + rootDummy(name_) + std::string("; i++) {\n") +
    //   indentation(indent) + std::string("  if (i != 0) s(\", \");\n");
      
    // if (type_ == std::string("Bool_t"))
    //   out += indentation(indent) + std::string("  b((*") + rootDummy(name_) + std::string(")[i]);\n");
    // else if (type_ == std::string("Char_t"))
    //   out += indentation(indent) + std::string("  d((*") + rootDummy(name_) + std::string(")[i]);\n");   // Char_t -> integer for our purposes
    // else if (type_ == std::string("Short_t"))
    //   out += indentation(indent) + std::string("  hd((*") + rootDummy(name_) + std::string(")[i]);\n");
    // else if (type_ == std::string("Int_t"))
    //   out += indentation(indent) + std::string("  d((*") + rootDummy(name_) + std::string(")[i]);\n");
    // else if (type_ == std::string("Long_t"))
    //   out += indentation(indent) + std::string("  ld((*") + rootDummy(name_) + std::string(")[i]);\n");
    // else if (type_ == std::string("Long64_t"))
    //   out += indentation(indent) + std::string("  ld((*") + rootDummy(name_) + std::string(")[i]);\n");
    // else if (type_ == std::string("Float_t"))
    //   out += indentation(indent) + std::string("  fg((*") + rootDummy(name_) + std::string(")[i]);\n");
    // else if (type_ == std::string("Float16_t"))
    //   out += indentation(indent) + std::string("  fg((*") + rootDummy(name_) + std::string(")[i]);\n");
    // else if (type_ == std::string("Double_t"))
    //   out += indentation(indent) + std::string("  g((*") + rootDummy(name_) + std::string(")[i]);\n");
    // else if (type_ == std::string("Double32_t"))
    //   out += indentation(indent) + std::string("  g((*") + rootDummy(name_) + std::string(")[i]);\n");
    // else if (type_ == std::string("UChar_t"))
    //   out += indentation(indent) + std::string("  u((*") + rootDummy(name_) + std::string(")[i]);\n");   // Char_t -> integer for our purposes
    // else if (type_ == std::string("UShort_t"))
    //   out += indentation(indent) + std::string("  hu((*") + rootDummy(name_) + std::string(")[i]);\n");
    // else if (type_ == std::string("UInt_t"))
    //   out += indentation(indent) + std::string("  u((*") + rootDummy(name_) + std::string(")[i]);\n");
    // else if (type_ == std::string("ULong_t"))
    //   out += indentation(indent) + std::string("  lu((*") + rootDummy(name_) + std::string(")[i]);\n");
    // else if (type_ == std::string("ULong64_t"))
    //   out += indentation(indent) + std::string("  lu((*") + rootDummy(name_) + std::string(")[i]);\n");
    // else if (type_ == std::string("string"))
    //   out += indentation(indent) + std::string("  s(\"\\\"\"); s(escapeJSON((*") + rootDummy(name_) + std::string(")[i]).c_str()); s(\"\\\"\");\n");
    // else if (type_ == std::string("TString"))
    //   out += indentation(indent) + std::string("  s(\"\\\"\"); s(escapeJSON(std::string((*") + rootDummy(name_) + std::string(")[i].Data())).c_str()); s(\"\\\"\");\n");
    // else
    //   throw std::invalid_argument(std::string("unrecognized type (C): ") + type_);
    // out += indentation(indent) + std::string("};\n") +
    //   indentation(indent) + std::string("s(\"]\");\n");
    // return out;
  }

  std::string ReaderArrayNode::schema(int indent, std::string ns) {
    return indentation(indent) + std::string("{\"name\": \"") + name_ + std::string("\", \"type\":\n") +
           indentation(indent + 2) + std::string("{\"type\": \"array\", \"items\": ") +
           unrollTemplatesSchema(indent + 2, ns, templates_, def_, innerType_) +
           std::string("}\n") +
           indentation(indent) + std::string("}");

    // std::string out;
    // out += indentation(indent) + std::string("{\"name\": \"") + name_ + std::string("\", \"type\": {\"type\": \"array\", \"items\": ");
    // if (type_ == std::string("Bool_t"))
    //   out += std::string("\"boolean\"}}");
    // else if (type_ == std::string("Char_t"))
    //   out += std::string("\"int\"}}");
    // else if (type_ == std::string("Short_t"))
    //   out += std::string("\"int\"}}");
    // else if (type_ == std::string("Int_t"))
    //   out += std::string("\"int\"}}");
    // else if (type_ == std::string("Long_t"))
    //   out += std::string("\"long\"}}");
    // else if (type_ == std::string("Long64_t"))
    //   out += std::string("\"long\"}}");
    // else if (type_ == std::string("Float_t"))
    //   out += std::string("\"float\"}}");
    // else if (type_ == std::string("Float16_t"))
    //   out += std::string("\"float\"}}");
    // else if (type_ == std::string("Double_t"))
    //   out += std::string("\"double\"}}");
    // else if (type_ == std::string("Double32_t"))
    //   out += std::string("\"double\"}}");
    // else if (type_ == std::string("UChar_t"))
    //   out += std::string("\"int\"}}");
    // else if (type_ == std::string("UShort_t"))
    //   out += std::string("\"int\"}}");
    // else if (type_ == std::string("UInt_t"))
    //   out += std::string("\"long\"}}");
    // else if (type_ == std::string("ULong_t"))
    //   out += std::string("\"double\"}}");
    // else if (type_ == std::string("ULong64_t"))
    //   out += std::string("\"double\"}}");
    // else if (type_ == std::string("string"))
    //   out += std::string("\"string\"}}");
    // else if (type_ == std::string("TString"))
    //   out += std::string("\"string\"}}");
    // else
    //   throw std::invalid_argument(std::string("unrecognized type (D): ") + type_);
    // return out;
  }

  ////////////////////////////////////// ReaderNestedArrayNode

  ReaderNestedArrayNode::ReaderNestedArrayNode(std::string type, std::string name, std::vector<int> fixedTail) : type_(type), name_(name), fixedTail_(fixedTail) { }

  std::string ReaderNestedArrayNode::declare(int indent) {
    return indentation(indent) + std::string("TTreeReaderArray<") + type_ + std::string(" > *") + rootDummy(name_) + std::string(";\n");
  }

  std::string ReaderNestedArrayNode::init(int indent) {
    return indentation(indent) + rootDummy(name_) + " = new " + std::string("TTreeReaderArray<") + type_ + std::string(" >(*getReader(), \"") + name_ + std::string("\");\n");
  }

  std::string ReaderNestedArrayNode::printJSON(int indent) { return std::string(); }

  // std::string loop(int indent) {
  //   return indentation(indent) + std::string("std::cout << \"") + name_ + std::string(": \";\n") +
  //          indentation(indent) + std::string("for (int i = 0;  i < ") + rootDummy(name_) + std::string("->GetSize(); i++)\n") +
  //          indentation(indent + 2) + std::string("std::cout << (*") + rootDummy(name_) + std::string(")[i] << \" \";\n") +
  //          indentation(indent) + std::string("std::cout << std::endl;\n");
  // }

  std::string ReaderNestedArrayNode::schema(int indent, std::string ns) {
    return std::string("");
  }

  ////////////////////////////////////// RawNode

  RawNode::RawNode(std::string type, std::string name) : type_(type), name_(name) { }

  std::string RawNode::declare(int indent) {
    return indentation(indent) + type_ + " *" + rootDummy(name_) + std::string(";\n") +
           indentation(indent) + "TBranch *b_" + rootDummy(name_) + std::string(";\n");
  }

  std::string RawNode::init(int indent) {
    return indentation(indent) + rootDummy(name_) + std::string(" = nullptr;\n") +
           indentation(indent) + std::string("b_") + rootDummy(name_) + std::string(" = nullptr;\n") +
           indentation(indent) + std::string("getReader()->GetTree()->SetBranchAddress(\"") + name_ + std::string("\", &") + rootDummy(name_) + std::string(", &b_") + rootDummy(name_) + std::string(");\n");
  }

  std::string RawNode::printJSON(int indent) {
    std::string out = indentation(indent) + std::string("b_") + rootDummy(name_) + std::string("->GetEntry(getReader()->GetCurrentEntry());\n") +
      indentation(indent) + std::string("s(\"\\\"") + name_ + std::string("\\\": \");\n");
    if (type_ == std::string("string"))
      out += indentation(indent) + std::string("  s(\"\\\"\"); s(escapeJSON(*") + rootDummy(name_) + std::string(").c_str()); s(\"\\\"\");\n");
    else if (type_ == std::string("TString"))
      out += indentation(indent) + std::string("  s(\"\\\"\"); s(escapeJSON(std::string(") + rootDummy(name_) + std::string("->Data())).c_str()); s(\"\\\"\");\n");
    else
      throw std::invalid_argument(std::string("unrecognized type (E): ") + type_);
    return out;
  }

  std::string RawNode::schema(int indent, std::string ns) {
    std::string out;
    out += indentation(indent) + std::string("{\"name\": \"") + name_ + std::string("\", \"type\": ");
    if (type_ == std::string("string"))
      out += std::string("\"string\"}");
    else if (type_ == std::string("TString"))
      out += std::string("\"string\"}");
    else
      throw std::invalid_argument(std::string("unrecognized type (F): ") + type_);
    return out;
  }

  /////////////////////////////////////////////////////////////////////////////////// public access functions

  Node **newArray(int size) {
    Node **out = new Node*[size];
    for (int i = 0;  i < size;  i++)
      out[i] = new InertNode;
    return out;
  }

  std::string definitions(std::map<const std::string, Def*> defs) {
    std::string out;
    for (auto pair = defs.begin();  pair != defs.end();  ++pair) {
      out += pair->second->forward();
    }
    for (auto pair = defs.begin();  pair != defs.end();  ++pair) {
      out += std::string("\n");
      out += pair->second->def();
    }
    return out;
  }

  std::string declarations(Node **scaffoldArray, int scaffoldSize) {
    std::string out;
    for (int i = 0;  i < scaffoldSize;  i++)
      out += scaffoldArray[i]->declare(2);
    return out;
  }

  std::string init(Node **scaffoldArray, int scaffoldSize) {
    std::string out;
    out += std::string("  void init() {\n");
    for (int i = 0;  i < scaffoldSize;  i++)
      out += scaffoldArray[i]->init(4);
    out += std::string("  }\n");
    return out;
  }

  std::string printJSON(Node **scaffoldArray, int scaffoldSize) {
    std::string out;
    out += std::string("  std::string escapeJSON(std::string string) {\n");
    out += std::string("    return string;\n");   // TODO! Escape bad characters, especially '\n'
    out += std::string("  }\n");
    out += std::string("\n");
    out += std::string("  void s(const char *x) { fputs(x, stdout); }\n");
    out += std::string("  void c(char x) { printf(\"%c\", x); }\n");
    out += std::string("  void b(char x) { if (x == 0) fputs(\"false\", stdout); else fputs(\"true\", stdout); }\n");
    out += std::string("  void hd(short x) { printf(\"%hd\", x); }\n");
    out += std::string("  void hu(unsigned short x) { printf(\"%hu\", x); }\n");
    out += std::string("  void d(int x) { printf(\"%d\", x); }\n");
    out += std::string("  void u(unsigned int x) { printf(\"%u\", x); }\n");
    out += std::string("  void ld(long x) { printf(\"%ld\", x); }\n");
    out += std::string("  void lu(unsigned long x) { printf(\"%lu\", x); }\n");
    out += std::string("  void fg(float x) { printf(\"%.9g\", x); }\n");
    out += std::string("  void g(double x) { printf(\"%.17g\", x); }\n\n");
    out += std::string("\n");
    out += std::string("  void run() {\n");
    out += std::string("    init();\n");
    out += std::string("    while (getReader()->Next()) {\n");
    out += std::string("      s(\"{\");\n");
    for (int i = 0;  i < scaffoldSize;  i++) {
      if (i > 0)
        out += std::string("      s(\", \");\n");
      out += std::string("\n") + scaffoldArray[i]->printJSON(6);
    }
    out += std::string("      s(\"}\\n\");\n");
    out += std::string("    }\n  }\n");
    return out;
  }

  std::string schema(int indent, std::string name, std::string ns, Node **scaffoldArray, int scaffoldSize) {
    std::string out;
    out += indentation(indent) + std::string("{\"type\": \"record\",\n");
    out += indentation(indent) + std::string(" \"name\": \"") + name + std::string("\",\n");
    if (ns.size() > 0)
      out += indentation(indent) + std::string(" \"namespace\": \"") + ns + std::string("\",\n");
    out += indentation(indent) + std::string(" \"fields\": [\n");
    for (int i = 0;  i < scaffoldSize;  i++) {
      out += scaffoldArray[i]->schema(3, ns);
      if (i < scaffoldSize - 1)
        out += std::string(",\n");
      else
        out += std::string("\n ]\n");
    }
    out += indentation(indent) + std::string("}\n");
    return out;
  }

}
