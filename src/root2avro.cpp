#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <TInterpreter.h>
#include <TFile.h>
#include <TTreeReader.h>

#include <avro.h>

#include "datawalker.h"

#define NA ((uint64_t)(-1))

using namespace ROOT::Internal;

// global variables for this tiny, single-threaded program (parallelism comes from multiple processes)
std::vector<std::string> fileLocations;
std::string              treeLocation;
uint64_t                 start = NA;
uint64_t                 end = NA;
std::vector<std::string> libs;
std::string              mode = "avro";
std::string              codec = "null";
std::string              schemaName = "";
std::string              ns = "";
bool                     debug = false;

TFile                   *file = nullptr;
TTreeReader             *reader = nullptr;

TTreeReader *getReader() { return reader; }

void help() {
  std::cerr << "Usage: root2avro fileLocation1, [fileLocation2, [...]], treeLocation" << std::endl << std::endl
            << "Where fileLocationN are either file paths or URLs TFile::Open can handle and treeLocation is the path of the" << std::endl
            << "TTree in all the files." << std::endl << std::endl
            << "Avro is streamed to standard output and can be redirected to a file." << std::endl << std::endl
            << "Options:" << std::endl
            << "  --start=NUMBER         First entry number to convert." << std::endl
            << "  --end=NUMBER           Entry number after the last to convert." << std::endl
            << "  --libs=LIB1,LIB2,...   Comma-separated list of .so files defining objects in the TTree (i.e. X_cxx.so with associated X_cxx_ACLiC_dict_rdict.pcm)." << std::endl
            << "  --mode=MODE            What to write to standard output: \"avro\" (Avro file, default), \"json\" (one JSON object per line), \"schema\" (Avro schema only)." << std::endl
            << "  --codec=CODEC          Codec for compressing the Avro output; may be \"null\" (uncompressed, default)," << std::endl
            << "                         \"deflate\", \"snappy\", \"lzma\", depending on libraries installed on your system." << std::endl
            << "  --name=NAME            Name for schema (taken from TTree name if not provided)." << std::endl
            << "  --ns=NAMESPACE         Namespace for schema (blank if not provided)." << std::endl
            << "  -d, -debug, --debug    If supplied, only show the generated C++ code and exit; do not run it." << std::endl
            << "  -h, -help, --help      Print this message and exit." << std::endl;
}

std::vector<std::string> splitByComma(std::string in) {
  std::vector<std::string> out;
  std::stringstream ss(in);
  std::string item;
  while (std::getline(ss, item, ','))
    out.push_back(item);
  return out;
}

void analyzeTree(TTree *tree);
void analyzeBranches(TObjArray *branches, TVirtualStreamerInfo *info, int level);
void analyzeElement(TBranch *branch, TStreamerElement *element, int level);

int main(int argc, char **argv) {
  for (int i = 1;  i < argc;  i++) {
    if (std::string(argv[i]) == std::string("-h")  ||
        std::string(argv[i]) == std::string("-help")  ||
        std::string(argv[i]) == std::string("--help")) {
      help();
      return 0;
    }
  }

  std::string startPrefix("--start=");
  std::string endPrefix("--end=");
  std::string libsPrefix("--libs=");
  std::string modePrefix("--mode=");
  std::string codecPrefix("--codec=");
  std::string namePrefix("--name=");
  std::string nsPrefix("--ns=");
  std::string badPrefix("-");

  for (int i = 1;  i < argc;  i++) {
    std::string arg(argv[i]);

    if (arg.substr(0, startPrefix.size()) == startPrefix) {
      std::string value = arg.substr(startPrefix.size(), arg.size());
      start = strtoul(value.c_str(), nullptr, 10);
    }

    else if (arg.substr(0, endPrefix.size()) == endPrefix) {
      std::string value = arg.substr(endPrefix.size(), arg.size());
      end = strtoul(value.c_str(), nullptr, 10);
    }

    else if (arg.substr(0, libsPrefix.size()) == libsPrefix) {
      libs = splitByComma(arg.substr(libsPrefix.size(), arg.size()));
    }

    else if (arg.substr(0, modePrefix.size()) == modePrefix) {
      mode = arg.substr(modePrefix.size(), arg.size());
    }

    else if (arg.substr(0, codecPrefix.size()) == codecPrefix)
      codec = arg.substr(codecPrefix.size(), arg.size());

    else if (arg.substr(0, namePrefix.size()) == namePrefix) {
      schemaName = arg.substr(namePrefix.size(), arg.size());
    }

    else if (arg.substr(0, nsPrefix.size()) == nsPrefix) {
      ns = arg.substr(nsPrefix.size(), arg.size());
    }

    else if (arg == std::string("-d")  ||  arg == std::string("-debug")  ||  arg == std::string("--debug"))
      debug = true;

    else if (arg.substr(0, badPrefix.size()) == badPrefix) {
      std::cerr << "Recognized switches are: --start, --end, --mode, --codec, --name, --ns, --debug, --help." << std::endl;
      return -1;
    }

    else
      fileLocations.push_back(arg);
  }

  if (fileLocations.size() < 2) {
    std::cerr << "At least two (non-switch) arguments are required." << std::endl;
    return -1;
  }
  treeLocation = fileLocations.back();
  fileLocations.pop_back();

  if (start != NA  &&  end != NA  &&  start >= end) {
    std::cerr << "Start must be less than end (if provided)." << std::endl;
    return -1;
  }

  for (int i = 0;  i < libs.size();  i++)
    gInterpreter->ProcessLine((std::string(".L ") + libs[i]).c_str());

  // FIXME: handle TChains
  file = TFile::Open(fileLocations[0].c_str());
  reader = new TTreeReader(treeLocation.c_str(), file);
    
  TreeWalker treeWalker;

  std::cout << std::endl << treeWalker.repr() << std::endl << std::endl;

  reader->Next();
  treeWalker.resolve();
  // std::cout << "RESOLVED? " << treeWalker.resolved() << std::endl;

  // std::cout << treeWalker.repr() << std::endl;

  std::cout << std::endl << treeWalker.avroSchema() << std::endl << std::endl;

  treeWalker.printJSON();

  while (reader->Next())
    treeWalker.printJSON();

  file->Close();

  return 0;
}
