/*!
 * @file utils.hpp
 * @brief Declaration of mostly string-related utilities.
 * @author Eudoxia
 */

namespace Hylas
{
  using namespace std;
  using namespace llvm;
  
  template <typename T>
  inline string to_string(T datum);
  
  template <typename T>
  inline T from_string(string datum);
  
  string dump_form(Form* input);
  
  inline string cutlast(string in);
  
  inline string cutfirst(string in);
  
  inline string cutboth(string in);
}
