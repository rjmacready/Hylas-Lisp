/*!
 * @file hylas.hpp
 * @brief Declaration of the Hylas code generator and JIT.
 * @author Eudoxia
 */

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
#include <exception>
#include <unistd.h>

#include "llvm/DerivedTypes.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Linker.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Assembly/Parser.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/Path.h"

namespace Hylas
{
  using namespace std;
  using namespace llvm;
  using namespace llvm::sys;
  
  LLVMContext& Context = getGlobalContext();
  
  #define List  false
  #define Atom  true
  
  struct Form
  {
    bool        Tag;            ///< Either an atom (true) or a list (false)
    string      Value;          ///< If an atom, this holds the symbol string ('derp','45','"hurp"')
    Form*       Car;            ///< If a list, pointer to the car
    Form*       Cdr;            ///< If a list, pointer to the cdr
    long line;                  ///< For the debugger: Line where this form was read
    int column;                 ///< For the debugger: Column where this form was read
    ~Form()
    {
      delete Car; delete Cdr;   ///A simple recursive destructor
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
  
  ostream& operator<<(ostream& out, Form* in);
  string operator+(string out, Form* in);
  string operator+(Form* in, string out);
  
  enum RegisterType { StdRegister, LValue, SymbolicRegister };
  
  struct Variable
  {
    string type;                ///< Type of the variable
    string address;             ///< Address (Symbol name, or tmp register)
    bool constant;              ///< Is it a constant?
    bool global;                ///< Is it global?
    long scope_address;         ///< Where in the scope stack is it?
    RegisterType regtype;       ///< The type of the var's register
  };
  
  struct Scope
  {
    map<string,Variable> vars;
    vector<string> labels;
  };

  typedef vector<Scope> Stack;
  
  enum TextOutput {Plain, HTML}; ///< Compiler output can be plain text or HTML
  
  struct RGB
  {
    unsigned int  Red; unsigned int Green; unsigned int Blue;
    RGB(unsigned int r, unsigned int g, unsigned int b): Red(r), Green(g), Blue(b) {}
  };
  
  RGB HSV_to_RGB(float h, float s, float v);
  RGB genColor();
  string exportRGB(RGB in);
  string matchKeyword(string in);
  map<string,RGB> defaultColorscheme();
  string defaultCSS();
  string getCSS();
  
  enum ErrorType {NormalError,ReaderError,GenericError,MacroError};
  
  struct Compiler
  {
    Module* Program;
    PassManager Passes;
    ExecutionEngine* Engine;
    string llvm_errstr;
    Linker* Loader;
    //General Behaviour options
    bool allow_RedefineMacros;
    bool allow_RedefineFunctions;
    bool allow_RedefineWordMacros;
    bool allow_RedefinePrePostfixes;
    //Should the output be HTML or plain text? (For pretty frontends)
    TextOutput output;
    string prompt;
    map<string,RGB> Colorscheme;
    string CSS;
    //Errors
    string errmsg;
    ErrorType errormode;
    bool debug;
    //Shit
    Stack SymbolTable;
    #define ScopeDepth (master.SymbolTable.size()-1)
    vector<string> CodeStack;
    vector<string> Persistent;
    vector<string> Namespaces;
    vector<string> NamespaceStack;
  };
  
  Compiler master;
  
  struct IR
  {
    string      assembly;
    string      ret_type;
  };
  
  #define mword    ("i"+to_string(sizeof(size_t)*8))
  
  inline void push(string in);
  inline void persistentPush(string in);
  inline void error_unbound(Form* x);  
  Variable* lookup(string in);
  
  unsigned long tmp_version = -1;
  unsigned long res_version = -1;
  unsigned long label_version = -1;
  unsigned long string_version = -1;
  unsigned long array_version = -1;
  
  // Temporary Registers
  
  inline string gensym();  
  inline string get_unique_tmp();
  inline string get_tmp(long v);  
  inline string get_current_tmp();
  
  // Result Registers
  
  string get_unique_res(string type);  
  string get_unique_res_address(string type, string address, bool symbolic=false);  
  inline string get_res(long v);  
  string res_type(string name);  
  inline string get_current_res();
  
  // Labels
  
  string get_unique_label();
  string get_label(long v); 
  inline string get_current_label();  
  inline string functional_label(string in);
  
  // Register Manipulation
  
  inline string allocate(string address, string type);  
  inline string store(string type, string value, string address);  
  inline string load(string destination, string type, string source);  
  inline string latest_type();  
  inline string constant(string destination, string type, string value);  
  void dump_scope(unsigned long s);
  
  //Namespaces
  
    /*!
     * @brief Check if a string has already been namespaced
     * 
     */
  inline bool isNamespaced(string in);
    /*!
     * @brief Take a symbol, and if it's not namespaced, append the current namespace to it
     * 
     */
  string getNamespace(string in);
  
  
  // Code generation
  
  string emitCode(Form* form); 
  IR Compile(Form* form);
  IR JIT(IR code);
  void Run();
  void init_optimizer();
  void init();
  void restart();
}
