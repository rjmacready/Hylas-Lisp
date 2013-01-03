#include "../src/hylas.hpp"
#include "../src/core.hpp"
#include "../src/errors.hpp"
#include "../src/reader.hpp"
#include "../src/fndef.hpp"
#include "../src/external.hpp"
#include "../src/types.hpp"
#include "../src/utils.hpp"
#include "../src/docs.hpp"

#include "../src/hylas.cpp"
#include "../src/core.cpp"
#include "../src/errors.cpp"
#include "../src/reader.cpp"
#include "../src/fndef.cpp"
#include "../src/external.cpp"
#include "../src/types.cpp"
#include "../src/tests.cpp"
#include "../src/utils.cpp"
#include "../src/docs.cpp"

int main(int argc, char *argv[])
{
  using namespace Hylas;
  printf("Hylas Lisp 0.7, by Eudoxia\n");
  init();
  if(argc == 1)
  {
    //Hylas called without files
    repl:
    try
    {
      while(true)
      {
        printf("\n>");
        JIT(Compile(readString("(puts (print " + print(read(stdin)) + "))")));
        puts(Run().c_str());
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
      string action = argv[1];
      Form* input = readFile(argv[2]);
      //cerr << print(input) << endl;
      JIT(Compile(input));
      if(action == "run")
        puts(Run().c_str());
      string output = argv[3];
      freopen(output.c_str(),"w+",stderr);
      master.Program->dump();
      if(action == "compile")
      {
        system(string("llvm-as " + output + " -o " + output).c_str());
        system(string("llvm-ld " + output + " -native -o " + output).c_str());
      }
      else if(action == "LLVM")
      {
        //Everything you need to do here has already been done above
      }
      else if(action == "BC")
      {
        system(string("llvm-as " + output + " -o " + output).c_str());
      }
      fclose(stderr);
    }
    catch(exception except)
    {
      fclose(stderr);
      cerr << getError() << endl;
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }
  else
  {
    cerr << "Usage:\n"
         << "hylas - Invokes the interpreter\n"
         << "\thylas run [input] - Executes the input in the JIT.\n"
         << "\thylas compile [input] [output] - Compiles [input] and saves the executable in [outout].\n"
         << "\thylas LLVM [input] [output] - Compiles [input] and saves the LLVM IR in [outout].\n"
         << "\thylas BC [input] [output] - Compiles [input] and saves the bitcode in [outout].\n"
         << endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
