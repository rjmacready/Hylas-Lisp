#include "hylas.hpp"

int main()
{
  using namespace Hylas;
  printf("Hylas Lisp 0.6, by Eudoxia\n");
  init();
  /*cout << "Running tests:\n" << endl;
  runTests();*/
  setjmp(buf);
  while(true)
  {
    printf("\n>");
    Form* code = read(stdin);
    string compiledCode = Compile(code);
    printf("Read form:\n%s\n",print(code).c_str());
    printf("Compiled form:\n%s",compiledCode.c_str());
    //compileIR(compiledCode);
    //Program->dump();
    /*if(!toplevel_exclusive)
     *    {
     *      Function* entryfn = Engine->FindFunctionNamed("entry");
     *      if(entryfn == NULL)
     *      {
     *  printf("ERROR: Couldn't find program entry point.");
     *  Unwind();
     }
     entryfnptr entry = reinterpret_cast<entryfnptr>(Engine->getPointerToFunction(entryfn));
     entry();
     entryfn->eraseFromParent();
     }*/
    //printf("\nExamination:\n");
    //print_fntable();
  }
  return 0;
}