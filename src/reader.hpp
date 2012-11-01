/*!
 * @file reader.hpp
 * @brief Implementation of the Hylas reader and printer.
 * @author Eudoxia
 */

namespace Hylas
{
  using namespace std;
  using namespace llvm;
  
  map<string,Form*> WordMacros;
  map<char,string> Prefixes;
  map<char,string> Postfixes;

  void addWordMacro(string word, Form* replacement);
    /*!
     * @brief Take a string. If it's a word macro, return the expanded form.
     * If not, return the string.
     * @param word String to match.
     * @return The expanded macro, if it was found, otherwise the original string.
     */
  void addWordMacro(string word, string replacement);
  string getMacro(string word);
  string tryPrefixOrPostfix(string word);
  inline Form* cons(Form* first, Form* second);
  Form* append(Form* first, Form* second);
  inline unsigned long length(Form* in);
  inline Form* nth(Form* in, long location);
  
  enum AnalysisType {BooleanTrue, BooleanFalse, Integer, Character, Real, String, Symbol, Unidentifiable};  
  AnalysisType analyze(string input);
  inline Form* makeForm(string in, bool tag);
  inline void clear_reader();
  inline char next_char(FILE* in);
  inline void unget_char(char c, FILE* in);
  string next_token(FILE *in);
  Form* read_tail(FILE *in);
  bool isArgument(Form* in, map<string,Form*> arguments);
  string getNamespace(string in);
  Form* editForm(Form* in, map<string,Form*> replacements);
  Form* expand(Form* in, unsigned char order);
  Form* expandEverything(Form* in);
  Form* read(FILE* in);
  Form* readFile(string filename);
  Form* readString(string in);
  
  string print(Form* in);
}