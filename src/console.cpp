#include "hylas.hpp"
#include "core.hpp"
#include "errors.hpp"
#include "reader.hpp"
#include "fndef.hpp"
#include "external.hpp"
#include "types.hpp"
#include "utils.hpp"

#include "hylas.cpp"
#include "core.cpp"
#include "errors.cpp"
#include "reader.cpp"
#include "fndef.cpp"
#include "external.cpp"
#include "types.cpp"
#include "tests.cpp"
#include "utils.cpp"

int main(int argc, char *argv[])
{
  using namespace Hylas;
  printf("Hylas Lisp 0.7, by Eudoxia\n");
  init();
  if(argc == 1)
  {
    //Hylas called without files
    /*try
    {
      runTests();
    }
    catch(exception except)
    {
      cerr << getError() << endl;
      goto repl;
    }*/
    repl:
    try
    {
      while(true)
      {
        printf("\n>");
        JIT(Compile(read(stdin)));
        Run();
      }
    }
    catch(exception except)
    {
      cerr << getError() << endl;
      goto repl;
    }
  }
  else if(argc > 2)
  {
    //$hylas [action] [file] [output]
    try
    {
      string in = argv[1], out = argv[2];
      /*JIT(Compile(readFile(argv[1])));
      freopen(argv[2],"w",stderr);
      master.Program->dump();
      return EXIT_SUCCESS;*/
      ofstream file;
      file.open(out);
      file << JIT(Compile(readFile(in))).assembly;
      cerr << JIT(Compile(readFile(in))).assembly << endl;
      system(string("llvm-as " + out + " -o " + out).c_str());
      system(string("llvm-ld " + out + " -native -o " + out).c_str());
      file.close();
      return EXIT_SUCCESS;
    }
    catch(exception except)
    {
      cerr << getError() << endl;
      return EXIT_FAILURE;
    }
  }
  else
  {
    cerr << "Usage:\n"
         << "hylas - Invokes the interpreter\n"
         << "hylas [input] [output] - Compiles input and saves the executable in output"
         << endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
