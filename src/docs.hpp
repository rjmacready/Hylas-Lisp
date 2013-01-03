/*!
 * @file docs.hpp
 * @brief Interface to the Hylas documentation generator.
 * @author Eudoxia
 */

namespace Hylas
{
  using namespace std;
  using namespace llvm;
  
  typedef pair<string,string> HtmlFile;
  
  HtmlFile makeFile(string filename);
  
  struct Documentation
  {
    vector<HtmlFile>    files;
  };

  /*!
  * @brief Iterate over a form until finding a form of the kind 
  * (doc "[documentation]"). If found, return a pair of [documentation] and the 
  * edited form.
  */
  pair<string,Form*> getDocstring(Form* in);
  /*!
  * @brief Remove the nth element of a string.
  */
  Documentation docgen(CSS format);
  /*!
  * @brief Simply write the documentation to a file.
  */
  bool toFile(Documentation dox);
}
