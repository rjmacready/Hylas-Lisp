#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <climits>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <csetjmp>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <stdarg.h>

/*#include "llvm/DerivedTypes.h"
 * #include "llvm/ExecutionEngine/ExecutionEngine.h"
 * #include "llvm/ExecutionEngine/JIT.h"
 * #include "llvm/LLVMContext.h"
 * #include "llvm/Module.h"
 * #include "llvm/PassManager.h"
 * #include "llvm/Analysis/Verifier.h"
 * #include "llvm/Analysis/Passes.h"
 * #include "llvm/Target/TargetData.h"
 * #include "llvm/Transforms/Scalar.h"
 * #include "llvm/Support/IRBuilder.h"
 * #include "llvm/Support/TargetSelect.h"
 * #include "llvm/Support/raw_ostream.h"
 * #include "llvm/Support/SourceMgr.h"
 * #include "llvm/ADT/Twine.h"
 * #include "llvm/Assembly/Parser.h"*/


namespace Hylas
{
  using namespace std;
  //using namespace llvm;
  
  /* Module* Program;
   *  LLVMContext& Context = getGlobalContext();
   *  IRBuilder<> Builder(Context);
   *  FunctionPassManager FPM(Program);
   *  ExecutionEngine* Engine;*/
  
  #define List	false
  #define Atom	true
  
  struct Form
  {
    bool		Tag;
    string	Value;
    Form*		Car;
    Form*		Cdr;
    long line;
    int column;
    ~Form()
    {
      delete Car; delete Cdr;
    }
  };
  
  #define val(x)		(x->Value)
  #define tag(x)		(x->Tag)
  #define isatom(x)	(tag(x) == Atom)
  #define islist(x)	(tag(x) == List)
  #define car(x)		(x->Car)
  #define cdr(x)		(x->Cdr)
  #define cadr(x)		(car(cdr(x)))
  #define cddr(x)		(cdr(cdr(x)))
  
  typedef void (*entryfnptr)();
  
  void Unwind();
  Form* cons(Form* first, Form* second);
  unsigned long length(Form* in);
  Form* nth(Form* in, long location);
  inline Form* makeForm(string in, bool tag);
  string next_token(FILE *in);
  Form* read_tail(FILE *in);
  Form* read(FILE* in);
  string preprint(Form* in);
  unsigned char analyze(string in);
  void atomize(string atom);
  string* lookup(string in);
  string type(string input);
  string code(string input);
  string emitCode(Form* form);
  
  struct Compiler
  {
	bool allow_RedefineMacros;
	bool allow_RedefineFunctions;
	bool output;
  };
  
  Compiler master;
  
  #define plain       false
  #define HTML  true
 
  #include "utils.hpp"
  #include "errors.hpp"
  #include "reader.hpp"
  
  typedef map<string,string> Scope;
  typedef vector<Scope> ST;
  ST SymbolTable;
  #define ScopeDepth SymbolTable.size()-1
  
  #define error_unbound(x)        \
  printf("ERROR:Symbol '%s' is unbound.",preprint(x).c_str()); \
  Unwind();
  
  string* lookup(string in)
  {
    for(long i = ScopeDepth; i != -1; i--)
    {
      map<string,string>::iterator seeker = SymbolTable[i].find(in);
      if(seeker != SymbolTable[i].end())
        return &seeker->second;
    }
    return NULL;
  }
  
  unsigned long tmp_version = -1;
  unsigned long res_version = -1;
  unsigned long label_version = -1;
  
  string cutlast(string in)
  {
    return string(in,0,in.length()-1);
  }
  
  /*
   * TEMPORARY REGISTERS
   */
  
  inline string get_unique_tmp()
  {
    return "%tmp.version" + to_string(++tmp_version);
  }
  
  inline string get_tmp(long long int v)
  {
    return "%tmp.version" + to_string(v);
  }
  
  inline string get_current_tmp(){ return get_tmp(tmp_version);}
  
  /*
   * RESULT REGISTERS
   */
  
  string get_unique_res(string type)
  {
    string vnum = to_string(++res_version);
    SymbolTable[ScopeDepth]["%res.version" + vnum] = type;
    return "%res.version" + vnum;
  }
  
  inline string get_res(long v)
  {
    return "%res.version" + to_string(v);
  }
  
  string res_type(string name)
  {
    string* tmp = lookup(name);
    if(tmp == NULL)
    { printf("ERROR: Can't find specific res register '%s'.",name.c_str()); Unwind(); }
    return *tmp;
  }
  
  inline string get_current_res(){ return get_res(res_version);}
  
  /*
   * LABELS
   */
  
  string get_unique_label()
  {
    return "%label.version" + to_string(label_version++);
  }
  
  string get_label(long long int v)
  {
    return "%label.version" + to_string(v);
  }
  
  inline string get_current_label(){ return get_label(label_version);}
  
  #define allocate(address,type)		\
  (string)address + " = alloca " + (string)type + "\n"
  #define store(type,value,address)		\
  "store " + (string)type + " " + (string)value + ", " + (string)type + "* " + (string)address + "\n"
  #define load(to,type,source)			\
  to + " = load " + (string)type + "* " + (string)source
  
  #define latest_type()   res_type(get_current_res())
  
  void dump_scope(unsigned long s)
  {
    for(map<string,string>::iterator i = SymbolTable[s].begin();
        i != SymbolTable[s].end(); i++)
    {
      printf("\n %s : %s",i->first.c_str(),i->second.c_str());
    }
  }
  
  typedef string (*hFuncPtr)(Form* code);
  
  map<string,hFuncPtr> Core;
  
  vector<string> CodeStack;
  
  inline void push(string in)
  {
    CodeStack.push_back(in);
  }

  #include "types.hpp"
  #include "fndef.hpp"
  #include "core.hpp"
  #include "tests.hpp"
  #include "macros.hpp"
  
  string emitCode(Form* form)
  {
    string out;
    printf("Emitting code for '%s'!\n",preprint(form).c_str());
    if(isatom(form))
    {
      switch(analyze(val(form)))
      {
        case BooleanTrue:
        {
          out = allocate(get_unique_tmp(),"i1");
          out += store("i1","1",get_current_tmp());
          out += load(get_unique_res("i1"),"i1",get_current_tmp());
          break;
        }
        case BooleanFalse:
        {
          out = allocate(get_unique_tmp(),"i1");
          out += store("i1","0",get_current_tmp());
          out += load(get_unique_res("i1"),"i1",get_current_tmp());
          break;
        }
        case Integer:
        {
          out = allocate(get_unique_tmp(),"i64");
          out += store("i64",val(form),get_current_tmp());
          out += load(get_unique_res("i64"),"i64",get_current_tmp());
          break;
        }
        case Real:
        {
          out = allocate(get_unique_tmp(),"double");
          out += store("double",val(form),get_current_tmp());
          out += load(get_unique_res("double"),"double",get_current_tmp());
          break;
        }
        case Symbol:
        {
          /*map<string,string>::iterator seeker;
          string sym = val(form);
          string tmp;
          string type;
          for(long i = ScopeDepth; i != -1; i--)
          {
            seeker = SymbolTable[i].find(sym);
            if(seeker != SymbolTable[i].end())
            {
              type = seeker->second;
              tmp = sym + to_string<long>(i);
              out = load(get_unique_res(type),type,"%"+tmp);
            }
          }
          string* tmp = lookup(val(form));
          if(tmp != NULL)
            out = load(get_unique_res(val(form)),val(form),"%"+val(form));
          else
            error_unbound(val(form));
          break;*/
          string sym = val(form);
          string* tmp = lookup(sym);
          if(tmp == NULL)
          {
            error_unbound(form);
          }
          else
            out = load(get_unique_res(*tmp),*tmp,"%"+sym+to_string(ScopeDepth));
          break;
        }
      }
    }
    else
    {
      if(islist(car(form)))
      {
        printf("ERROR: Lists can't be used as a function call until I implement lambda.");
        Unwind();
      }
      string func = val(car(form));
      map<string,hFuncPtr>::iterator seeker = Core.find(func);
      if(seeker != Core.end())
      {
        out = seeker->second(form);
      }
      else
      {
        out = callFunction(func,cdr(form));
      }
      /*if(true)
       *      {
       *    		out = allocate(get_unique_tmp(),val(nth(form,0)));
       *    		out += store(val(nth(form,0)),val(nth(form,1)),get_current_tmp());
       *    		out += load(get_unique_res(val(nth(form,0))),val(nth(form,0)),get_current_tmp());
       }*/
    }
    return out+"\n";
  }
  
  string TopLevel(Form* in)
  {
    //...
    string out;
    for(unsigned long i = 0; i < CodeStack.size(); i++)
    {
      out += CodeStack[i] + "\n";
    }
    CodeStack.clear();
    return out;
  }
  
  void init_optimizer()
  {
    /*FPM.add(createBasicAliasAnalysisPass());
     *    FPM.add(createInstructionCombiningPass());
     *    FPM.add(createReassociatePass());
     *    FPM.add(createGVNPass());
     *    FPM.add(createCFGSimplificationPass());
     *    FPM.add(createPromoteMemoryToRegisterPass());
     *    FPM.doInitialization();*/
  }
  
  void init()
  {
    //InitializeNativeTarget();
    //Program = new Module("Hylas Lisp",Context);
    master.allow_RedefineMacros = true;
    master.allow_RedefineFunctions = false;
    master.output = plain;
    init_stdlib();
    init_types();
    init_optimizer();
    //Engine =  EngineBuilder(Program).create();
  }
  
  /*void compileIR(string in)
   *  {
   *    SMDiagnostic errors;
   *    ParseAssemblyString(in.c_str(),Program,errors,Context);
   *    if(!errors.getMessage().empty())
   *      printf("\n%s",errors.getMessage().c_str());
   *    if(verifyModule(*Program))
   *    {
   *      printf("The IR verifier found an unknown error.");
   *      Unwind();
   }
   }*/
  
}

int main()
{
  using namespace Hylas;
  printf("Hylas Lisp 0.1, by Eudoxia\n");
  init();
  cout << "Running tests:\n" << endl;
  setjmp(buf);
  runTests();
  while(true)
  {
    printf("\n>");
    Form* code = read(stdin);
    string compiledCode = /*"define void masterentry(){\n" +*/ emitCode(code) /*+ "ret void\n}"*/;
    printf("Read form:\n%s\n",preprint(code).c_str());
    printf("Compiled form:\n%s",compiledCode.c_str());
    //compileIR(compiledCode);
    //Program->dump();
    /*if(!toplevel_exclusive)
     *    {
     *      Function* entryfn = Engine->FindFunctionNamed("entry");
     *      if(entryfn == NULL)
     *      {
     *	printf("ERROR: Couldn't find program entry point.");
     *	Unwind();
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
