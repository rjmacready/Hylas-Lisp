#include "hylas.hpp"

int main()
{
  using namespace Hylas;
  printf("Hylas Lisp 0.6, by Eudoxia\n");
  init();
  try
  {
    runTests();
  }
  catch(exception except)
  {
    cerr << getError() << endl;
    goto repl;
  }
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
  return 0;
}