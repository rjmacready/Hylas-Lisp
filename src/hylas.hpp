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
  PassManager Passes(Program);
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
  
  enum RegisterType { StdRegister, LValue, SymbolicRegister };
  
  struct Variable
  {
    string type;
    unsigned long address;
    bool constant;
    bool global;
    RegisterType regtype;
  };
  
  typedef map<string,Variable> Scope;
  typedef vector<Scope> ST;
  
  struct Compiler
  {
    //General Behaviour options
    bool allow_RedefineMacros;
    bool allow_RedefineFunctions;
    bool allow_RedefineWordMacros;
    bool allow_RedefinePrePostfixes;
    //Should the output be HTML or plain text? (For pretty frontends)
    bool output;
    //Shit
    ST SymbolTable;
    vector<string> CodeStack;
  };
  
  #define ScopeDepth master.SymbolTable.size()-1
  
  Compiler master;
  
  inline void push(string in)
  {
    master.CodeStack.push_back(in);
  }
  
  #define plain         false
  #define HTML          true
 
  #include "utils.hpp"
  #include "errors.hpp"
  #include "reader.hpp"
  
  inline void error_unbound(Form* x)
  { error(x,"Symbol '",print(x),"' is unbound."); }
  
  Variable* lookup(string in)
  {
    for(long i = ScopeDepth; i != -1; i--)
    {
      map<string,Variable>::iterator seeker = master.SymbolTable[i].find(in);
      if(seeker != master.SymbolTable[i].end())
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
    master.SymbolTable[ScopeDepth]["%res.version" + vnum].type = type;
    return "%res.version" + vnum;
  }
  
  string get_unique_res_address(string type, unsigned long address)
  {
    string vnum = to_string(++res_version);
    master.SymbolTable[ScopeDepth]["%res.version" + vnum].type = type;
    master.SymbolTable[ScopeDepth]["%res.version" + vnum].address = address;
    master.SymbolTable[ScopeDepth]["%res.version" + vnum].regtype = LValue;
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
  { return destination + " = select i1 true, " + type + " " + value + ", " + type + " " + value; }
  
  void dump_scope(unsigned long s)
  {
    for(map<string,Variable>::iterator i = master.SymbolTable[s].begin();
        i != master.SymbolTable[s].end(); i++)
    {
      printf("\n %s : %s",i->first.c_str(),i->second.type.c_str());
    }
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
            out = load(get_unique_res_address(tmp->type,tmp->address),tmp->type,((tmp->global) ? "@" : "%")
                + sym + to_string(ScopeDepth));
          break;
        }
        case String:
        {
          //Remember strings come with their double quotes
          //Also convert them to unicode
          string str = cutboth(val(form));
          unsigned long length = str.length();
          string type = "[" + to_string<unsigned long>(length+1) + " x i8]";
          stringstream ss;
          string result;
          for(unsigned long i = 0; i < length; i++)
          { 
            ss << hex << (int)str[i];
            string tmp = ss.str();
            if(tmp.length() > 2)
              tmp = string(tmp,tmp.length()-2);
            result += '\\' + tmp;
          }
          push("@str" + to_string<unsigned long>(++string_version) + " = global " + type + " c\"" + result + "\\00\"");
          out = get_unique_res("i8*") + " = getelementptr " + type + "* @str" + to_string<unsigned long>(string_version) + ", i64 0, i64 0";
          break;
        }
      }
    }
    else
    {
      if(islist(car(form)))
        error(form,"Lists can't be used as function names in calls. Until I implement lambda.");
      string func = val(car(form));
      map<string,hFuncPtr>::iterator seeker = TopLevel.find(func);
      if(seeker != TopLevel.end())
        out = seeker->second(form);
      seeker = Core.find(func);
      if(seeker != Core.end())
        out = seeker->second(form);
      else
        out = callFunction(form);
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
      if(islist(car(form)))
        error(form,"Lists can't be used as function names in calls. Until I implement lambda.");
      string func = val(car(form));
      map<string,hFuncPtr>::iterator seeker = TopLevel.find(func);
      if(seeker != TopLevel.end())
        out = seeker->second(form);
      seeker = Core.find(func);
      if(seeker != Core.end())
        out = seeker->second(form);
      else
        out = emitCode(form);
    }
    for(unsigned long i = 0; i < master.CodeStack.size(); i++)
      tmp += master.CodeStack[i] + "\n";
    out = "define " + latest_type() + " @entry(){\n" + out + "\nret " + latest_type() + " " + get_current_res() + "\n}";
    out = tmp + out;
    master.CodeStack.clear();
    tmp_version = -1;
    res_version = -1;
    label_version = -1;
    clear_reader();
    return out;
  }
  
  string JIT(string code)
  {
    /*SMDiagnostic errors;
    ParseAssemblyString(in.c_str(),Program,errors,Context);
    if(!errors.getMessage().empty())
      printf("\n%s",errors.getMessage().c_str());
    if(verifyModule(*Program))
      nerror("The IR verifier found an unknown error.");*/
    return code;
  }
  
  void init_optimizer()
  {
    /*addPass(master.Passes,createBasicAliasAnalysisPass());
    addPass(master.Passes,createInstructionCombiningPass());
    addPass(master.Passes,createReassociatePass());
    addPass(master.Passes,createGVNPass());
    addPass(master.Passes,createCFGSimplificationPass());
    addPass(master.Passes, createInstructionCombiningPass());
    addPass(master.Passes, createCFGSimplificationPass());
    addPass(master.Passes, createAggressiveDCEPass());
    addPass(master.Passes, createGlobalDCEPass());
    master.run(master.Program);*/
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
  
  void restart()
  {
    
  }
  
  #include "tests.hpp"
}