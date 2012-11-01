/*!
 * @file core.hpp 
 * @brief Declaration of the functions that make up the Hylas language.
 * @author Eudoxia
 */

namespace Hylas
{
  using namespace std;
  using namespace llvm;
  
  typedef string (*hFuncPtr)(Form* code);
  
  map<string,hFuncPtr> TopLevel;
  map<string,hFuncPtr> Core;
  map<string,hFuncPtr> After;
  
  vector<string> allowedIntComparisons;
  vector<string> allowedFloatComparisons;
  
  string def(Form* form, bool global, bool typeonly);
  string def_global(Form* form);  
  string def_local(Form* form);  
  string def_as_global(Form* form);  
  string def_as_local(Form* form);  
  string set(Form* form);  
  string ret(Form* form);  
  string generic_math(Form* form, string opcode);  
  string add(Form* form);
  string fadd(Form* form);
  string sub(Form* form);
  string fsub(Form* form);
  string mul(Form* form);
  string fmul(Form* form);
  string udiv(Form* form);
  string sdiv(Form* form);
  string fdiv(Form* form);
  string urem(Form* form);
  string srem(Form* form);
  string frem(Form* form);  
  string icmp(Form* form);  
  string fcmp(Form* form);  
  string generic_bitop(Form* form, string opcode);  
  string shl(Form* form);
  string lshr(Form* form);
  string ashr(Form* form);
  string bit_and(Form* form);
  string bit_or(Form* form);
  string bit_xor(Form* form);  
  string byte_swap(Form* form);  
  string count_ones(Form* form);
  /*
  string count-leading-zeros(Form* form);
  string count-trailing-zeros(Form* form);
  */  
  string truncate(Form* form);  
  string extend(Form* form);  
  string zextend(Form* form);
  string sextend(Form* form);
  /*
  string floattoint(Form* form, bool to_float, bool signed);
  string inttofloat(Form* form),
  string floattohalf(Form* form);
  string halftofloat(Form* form);
  */  
  string ptrtoint(Form* form);
  string inttoptr(Form* form);  
  string bitcast(Form* form);
  string size(Form* form);
  string access(Form* form);  
  string flow(Form* form);  
  string simple_if(Form* form);  
  string begin(Form* form);  
  string main_fn(Form* form);  
  string foreign(Form* form);  
  string embed_llvm(Form* form);  
  string define_function(Form* form);  
  string define_recursive(Form* form);  
  string define_fast(Form* form);  
  string define_inline(Form* form);  
  string define_inline_recursive(Form* form);  
  string define_inline_fast(Form* form);
  string make_structure(Form* form);
  string construct(Form* form);  
  string make_array(Form* form, bool global);  
  string global_array(Form* in);  
  string local_array(Form* in);  
  string nth_array(Form* in);  
  string mem_allocate(Form* in);  
  string mem_store(Form* in);  
  string mem_load(Form* in);  
  string address(Form* in);  
  string toplevel_asm(Form* in);  
  string inline_asm(Form* in);  
  string word(Form* in);    
  string fixes(Form* in, bool pre);  
  string prefix(Form* in);  
  string postfix(Form* in);
  string import(Form* in);  
    /*!
    * @brief Takes a string (A path), then calls out to the JIT which loads the library
    * in the path.
    * @param form The string that holds the library's name or path
    * @return LLVM IR
    */
  string link_with_library(Form* form);
    /*!
     * @brief Allocates an object on the heap.
     * 
     */
  string create(Form* form);
    /*!
     * @brief Destroys a heap-allocated object.
     * 
     */
  string destroy(Form* form);
    /*!
     * @brief Map function names to their pointers
     * 
     */
  void init_stdlib();
}