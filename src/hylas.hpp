#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <climits>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <csetjmp>
#include <climits>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <stdarg.h>
#include <regex>

/*#include "llvm/DerivedTypes.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Assembly/Parser.h"*/


namespace Hylas
{
  using namespace std;
  //using namespace llvm;
  
  /*Module* Program;
  LLVMContext& Context = getGlobalContext();
  IRBuilder<> Builder(Context);
  FunctionPassManager FPM(Program);
  ExecutionEngine* Engine;*/
  
  #define List  false
  #define Atom  true
  
  struct Form
  {
    bool        Tag;
    string      Value;
    Form*       Car;
    Form*       Cdr;
    long line;
    int column;
    ~Form()
    {
      delete Car; delete Cdr;
    }
  };
  
  #define val(x)        (x->Value)
  #define tag(x)        (x->Tag)
  #define isatom(x)     (tag(x) == Atom)
  #define islist(x)     (tag(x) == List)
  #define car(x)        (x->Car)
  #define cdr(x)        (x->Cdr)
  #define cadr(x)       (car(cdr(x)))
  #define cddr(x)       (cdr(cdr(x)))
  
  typedef void (*entryfnptr)();
  
  void Unwind();
  Form* cons(Form* first, Form* second);
  unsigned long length(Form* in);
  Form* nth(Form* in, long location);
  inline Form* makeForm(string in, bool tag);
  string next_token(FILE *in);
  Form* read_tail(FILE *in);
  Form* read(FILE* in);
  string print(Form* in);
  unsigned char analyze(string in);
  void atomize(string atom);
  string type(string input);
  string code(string input);
  string emitCode(Form* form);
  
  ostream& operator<<(ostream& out, Form* in)
  { out << print(in); return out; }
  
  string operator+(string out, Form* in)
  { return out + print(in); }
  
  string operator+(Form* in, string out)
  { return print(in) + out; }
  
  struct Compiler
  {
    //General Behaviour options
    bool allow_RedefineMacros;
    bool allow_RedefineFunctions;
    bool allow_RedefineWordMacros;
    bool allow_RedefinePrePostfixes;
    //Should the output be HTML or plain text? (For pretty frontends)
    bool output;
  };
  
  Compiler master;
  
  #define plain         false
  #define HTML          true
 
  #include "utils.hpp"
  #include "errors.hpp"
  #include "reader.hpp"
  
  struct Variable
  {
    string type;
    unsigned long address = -1;
    bool constant;
    bool global;
    bool lvalue;
  };
  
  typedef map<string,Variable> Scope;
  typedef vector<Scope> ST;
  ST SymbolTable;
  #define ScopeDepth SymbolTable.size()-1
  
  inline void error_unbound(Form* x)
  { error(x,"Symbol '",print(x),"' is unbound."); }
  
  Variable* lookup(string in)
  {
    for(long i = ScopeDepth; i != -1; i--)
    {
      map<string,Variable>::iterator seeker = SymbolTable[i].find(in);
      if(seeker != SymbolTable[i].end())
        return &seeker->second;
    }
    return NULL;
  }
  
  unsigned long tmp_version = -1;
  unsigned long res_version = -1;
  unsigned long label_version = -1;
  unsigned long string_version = -1;
  unsigned long array_version = -1;
  
  /*
   * TEMPORARY REGISTERS
   */
  
  inline string gensym() { return "%uniqueness_guaranteed_" + to_string<int>(rand()); }
  
  inline string get_unique_tmp()
  { return "%tmp.version" + to_string(++tmp_version); }
  
  inline string get_tmp(long v)
  { return "%tmp.version" + to_string(v); }
  
  inline string get_current_tmp(){ return get_tmp(tmp_version);}
  
  /*
   * RESULT REGISTERS
   */
  
  string get_unique_res(string type)
  {
    string vnum = to_string(++res_version);
    SymbolTable[ScopeDepth]["%res.version" + vnum].type = type;
    return "%res.version" + vnum;
  }
  
  string get_unique_res_address(string type, unsigned long address)
  {
    string vnum = to_string(++res_version);
    SymbolTable[ScopeDepth]["%res.version" + vnum].type = type;
    SymbolTable[ScopeDepth]["%res.version" + vnum].address = address;
    SymbolTable[ScopeDepth]["%res.version" + vnum].lvalue = true;
    return "%res.version" + vnum;
  }
  
  inline string get_res(long v)
  { return "%res.version" + to_string(v); }
  
  string res_type(string name)
  { 
    Variable* tmp = lookup(name);
    if(tmp == NULL)
    { printf("Can't find symbol."); Unwind(); }
    return tmp->type;
  }
  
  inline string get_current_res(){ return get_res(res_version);}
  
  /*
   * LABELS
   */
  
  string get_unique_label()
  { return "%label.version" + to_string(++label_version); }
  
  string get_label(long v)
  { return "%label.version" + to_string(v); }
  
  inline string get_current_label(){ return get_label(label_version);}
  
  inline string functional_label(string in)
  {
    return cutfirst(in) + ":\n";
  }
  
  /*
   * REGISTERS
   */
  
  inline string allocate(string address, string type)
  { return address + " = alloca " + type + "\n"; }
  
  inline string store(string type, string value, string address)
  { return "store " + type + " " + value + ", " + type + "* " + address + "\n"; }
  
  inline string load(string destination, string type, string source)
  { return destination + " = load " + type + "* " + source; }
  
  inline string latest_type()
  { return res_type(get_current_res()); }
  
  inline string constant(string destination, string type, string value)
  { return destination + " = select i1 true, " + type + " " + value + ", " + type + " " value + "\n"; }
  
  void dump_scope(unsigned long s)
  {
    for(map<string,Variable>::iterator i = SymbolTable[s].begin();
        i != SymbolTable[s].end(); i++)
    {
      printf("\n %s : %s",i->first.c_str(),i->second.type.c_str());
    }
  }
  
  vector<string> CodeStack;
  
  inline void push(string in)
  {
    CodeStack.push_back(in);
  }

  #include "types.hpp"
  #include "fndef.hpp"
  #include "core.hpp"
  #include "macros.hpp"
  
  string emitCode(Form* form)
  {
    string out;
    if(form == NULL)
      error(form,"Can't emit code for the null form.");
    else if(isatom(form))
    {
      switch(analyze(val(form)))
      {
        case BooleanTrue:
        {
          out = constant(get_unique_res("i1"),"i1","true");
          break;
        }
        case BooleanFalse:
        {
          out = constant(get_unique_res("i1"),"i1","false");
          break;
        }
        case Integer:
        {
          out = constant(get_unique_res("i64"),"i64",val(form));
          break;
        }
        case Character:
        {
          string c = string(val(form),1,val(form).length()-2);
          string address = "@str" + to_string<unsigned long>(++string_version);
          push(address + " = global [2 x i8] c\"" + c + "\0\0\"");
          out += get_unique_res("i8") + " = load i8* getelementptr inbounds ([2 x i8]* " + address + ", i32 0, i64 0)";
          break;
        }
        case Real:
        {
          out = constant(get_unique_res("double"),"double",val(form));
          break;
        }
        case Symbol:
        {
          string sym = val(form);
          Variable* tmp = lookup(sym);
          if(tmp == NULL)
            error_unbound(form);
          else
            out = load(get_unique_res(tmp->sym),tmp->sym,((tmp->global) ? "@" : "%")
                + sym + to_string(ScopeDepth));
          break;
        }
        case String:
        {
          //Remember strings come with their double quotes
          string str = cutboth(val(form));
          long length = str.length()-1;
          string type = "[" + to_string<long>(length) + " x i8]";
          push("@str" + to_string<unsigned long>(++string_version) + " = global " + type + " c\"" + str + "\\0A\\00\"");
          out = get_unique_res(type) + " = getelementptr " + type + "* @str" + to_string<unsigned long>(string_version) + ", i64 0, i64 0";
          break;
        }
      }
    }
    else
    {
      out = callFunction(func,false);
      if(out == "NOFOUND")
      {
        string func = val(car(form));
        map<string,hFuncPtr>::iterator seeker = Core.find(func); // We know it will be an atom because callFunction tests for that
        if(seeker != Core.end())
          out = seeker->second(form);
      }
    }
    return out+"\n";
  }
  
  string Compile(Form* form)
  {
    string out;
    string tmp;
    if(form == NULL)
      error(form,"Can't emit code for the null form.");
    else if(isatom(form))
      out = emitCode(form);
    else
    {
      out = callFunction(func,false);
      if(out == "NOFOUND")
      {
        string name = val(car(form));
        map<string,hFuncPtr>::iterator seeker = TopLevel.find(name); // We know it will be an atom because callFunction tests for that
        if(seeker != TopLevel.end())
          out = seeker->second(form);
        else
        {
          seeker = Core.find(name);
          if(seeker != Core.end())
            out = seeker->second(form);
          else
          {
            error("No function (Or variable with a matching function pointer type) matches the name '",name,"' and the protype PRINT THIS.");
          }
        }
      }
    }
    for(unsigned long i = 0; i < CodeStack.size(); i++)
      tmp += CodeStack[i] + "\n";
    out = "define " + latest_type() + " @entry(){\n" + out + "ret " + latest_type() + " " + get_current_res() + "\n}";
    out = tmp + out;
    CodeStack.clear();
    tmp_version = -1;
    res_version = -1;
    label_version = -1;
    clear_reader();
    return out;
  }
  
  void init_optimizer()
  {
    /*FPM.add(createBasicAliasAnalysisPass());
    FPM.add(createInstructionCombiningPass());
    FPM.add(createReassociatePass());
    FPM.add(createGVNPass());
    FPM.add(createCFGSimplificationPass());
    FPM.doInitialization();*/
  }
  
  void init()
  {
    //InitializeNativeTarget();
    //Program = new Module("Hylas Lisp",Context);
    master.allow_RedefineMacros = true;
    master.allow_RedefineWordMacros = true;
    master.allow_RedefinePrePostfixes = true;
    master.allow_RedefineFunctions = false;
    master.output = plain;
    init_stdlib();
    init_types();
    init_optimizer();
    srand(time(NULL)); //For the color generator
    //Engine =  EngineBuilder(Program).create();
  }
  
  void compileIR(string in)
  {
    /*SMDiagnostic errors;
    ParseAssemblyString(in.c_str(),Program,errors,Context);
    if(!errors.getMessage().empty())
      printf("\n%s",errors.getMessage().c_str());
    if(verifyModule(*Program))
      nerror("The IR verifier found an unknown error.");*/
  }
  
  #include "tests.hpp"
}