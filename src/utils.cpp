/*!
 * @file utils.cpp
 * @brief Implementation of mostly string-related utilities.
 * @author Eudoxia
 */

namespace Hylas
{
  using namespace std;
  using namespace llvm;
  
  template <typename T>
  inline string to_string(T datum)
  {
    stringstream output;
    output << datum;
    return output.str();
  }
  
  template <typename T>
  inline T from_string(string datum)
  {
    stringstream stream(datum);
    T out;
    stream >> out;
    return out;
  }
  
  string dump_form(Form* input)
  {
    string out = (string)"Form: " + (isatom(input) ? "Atom" : "List") + "\n'";
    out += input + (string)"'\n";
    return out + "(Line " + to_string(input->line) + ", column " + to_string(input->column) + ")";
  }
  
  inline string cutlast(string in)
  { return string(in,0,in.length()-1); }
  
  inline string cutfirst(string in)
  { return string(in,1); }
  
  inline string cutboth(string in)
  { return cutfirst(cutlast(in)); } 
}