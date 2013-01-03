/*!
 * @file macros.hpp
 * @brief Declaration of the macro system.
 * @author Eudoxia
 */

namespace Hylas
{
  using namespace std;
  using namespace llvm;
  
  struct Macro
  {
    string name;
  };
  
  struct RawMacro
  {
    string name;
  };
  
  vector<pair<string,Macro> > Macros;
  map<string,RawMacro> RawMacros;
  
  string makeRawMacro(Form* in);  
  string makeMacro(Form* in);
  
}
