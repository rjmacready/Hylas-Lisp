/*!
 * @file errors.hpp
 * @brief Declaration of the error system.
 * @author Eudoxia
 */

namespace Hylas
{
  using namespace std;
  using namespace llvm;
  
  inline void reseterror();
  
  class Error : public exception
  { int herp; };
  
  void Unwind();
  
  string getError();
  string at(Form* in);
  
  inline string print(string in);
  inline string print(char* in);
  inline string print(const char* in);
  inline string print(const char& in);
  
  void error_print() {} // termination version  
  template<typename Arg1, typename... Args>
  void error_print(const Arg1& arg1, const Args&... args);  
  void print_errormode();  
  template<typename... T>
  void error(Form* head, T const& ... text);  
  template<typename... T>
  void nerror(T const& ... text);  
  template<typename... T>
  void warn(Form* head, T const& ... text);
}
