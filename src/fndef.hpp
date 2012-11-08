/*!
 * @file fndef.hpp
 * @brief Declaration of function definition and calling.
 * @author Eudoxia
 */

namespace Hylas
{
  using namespace std;
  using namespace llvm;
  
  #define noinline false
  #define doinline true  
  
  struct Lambda
  {
    string name;                        ///< The raw (IR) name of the function.
    string ret_type;                    ///< The return type signature.
    string fn_ptr_type;                 ///< The function pointer type signature.
    unsigned long nargs;                ///< The number of arguments.
    map<string,string> arguments;       ///< The map of argument names->type signatures
    bool fastcc;                        ///< Does the function use the Fast calling convention?
    bool tco;                           ///< Is the function Tail-Call Optimized?
    bool lining;                        ///< Is the function always inlined?
    bool accessor;
    string docstring;                   ///< Documentation string.
  };
  
  /*!
   * A metafunction is a vector of functions that have the same name but are
   * differentiated by their argument types.
   */
  struct MetaFunction
  {
    vector<Lambda> versions;            ///< Versions of the function
  };
  
  map<string,MetaFunction> FunctionTable; ///< The global function table
  
  void print(map<string,string> in);  
  void print_metafunction(string in);  
  void print_fntable();
  
  /** Functions may be regular, Fast (Using the Fast calling convention)
   * or Recursive (Tail-Call Optimized and using the Fast CC)*/
  enum fnType {Function, Fast, Recursive};
  
  void validate_function(Form* form);  
  string removeReturn(string in);
  
  string defineFunction(Form* form, fnType fn_type, bool inlining);  
  string callGeneric(long gen_pos, Form* code);
  string cleanPointer(string in);  
  string callFunction(Form* in);
}
