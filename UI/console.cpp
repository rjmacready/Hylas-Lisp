#include "../src/hylas.hpp"
#include "../src/core.hpp"
#include "../src/errors.hpp"
#include "../src/reader.hpp"
#include "../src/fndef.hpp"
#include "../src/external.hpp"
#include "../src/types.hpp"
#include "../src/utils.hpp"

#include "../src/hylas.cpp"
#include "../src/core.cpp"
#include "../src/errors.cpp"
#include "../src/reader.cpp"
#include "../src/fndef.cpp"
#include "../src/external.cpp"
#include "../src/types.cpp"
#include "../src/tests.cpp"
#include "../src/utils.cpp"

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
    string out;
    repl:
    try
    {
      while(true)
      {
        printf("\n>");
        out = JIT(Compile(readString("(print "+print(read(stdin))+")")));
        puts(out.c_str());
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
      //...
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
