/*!
 * @file docs.cpp
 * @brief Implementation of the Hylas documentation generator.
 * @author Eudoxia
 */

namespace Hylas
{
  using namespace std;
  using namespace llvm;
  
  HtmlFile makeFile(string filename)
  {
    HtmlFile tmp;
    tmp.first = filename + ".html";
    return tmp;
  }
  
  Documentation docgen(CSS format=defaultCSS())
  {
    Documentation dox;
    //Declare all the necessary files
    HtmlFile index = makeFile("index");
    //Push the files
    dox.files.push_back(index);
    return dox;
  }
}
