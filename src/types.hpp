/*!
 * @file types.hpp
 * @brief Declaration of Hylas' type system.
 * @author Eudoxia
 */

namespace Hylas
{
  using namespace std;
  using namespace llvm;
  
  #define typeSimple    0
  #define typeStructure 1
  #define typeFunction 2
  #define typeMethod    3
  
  /*!
   * The base class for simple types and structures
   */
  
  typedef map<string,pair<unsigned long,string> > MembersMap;
  
  struct BaseType
  {
    unsigned char id; ///< Determines whether it's a simple type or a structural type
  };
  
  struct Type : BaseType
  {
    MembersMap members; ///> Map of member names to a pair of their position in the structure and their type
    string definition;
  };
  
  struct Generic : Type
  {
    vector<string> arguments;
    Form* code;
    map<string,MembersMap> specializations;
    vector<Form*> methods;
  };
  
  vector<string> CoreTypes;
  map<string,Type> BasicTypes;
  vector<pair<string,Generic> > Generics;
  
  unsigned int width(string integer);
  unsigned long typeSize(string type);
  bool isInteger(string in);
  unsigned int fpwidth(string in);
  bool isCoreType(string in);
  bool isBasicType(string in);
  string specializeType(Generic* in, map<string,Form*> replacements, string signature);
  string printTypeSignature(Form* form);  
  bool checkTypeExistence(string name);  
  string makeType(Form* in);  
  void validateStructure(Form* in);  
  string makeStructure(Form* in);  
  bool checkGenericExistence(string name, bool id);  
  Generic writeGeneric(Form* in, bool type);  
  void addGeneric(string name, Generic in);  
  Generic addGenericAttachment(string name, Form* in);  
  Generic makeGeneric(Form* in);  
  string genericInterface(Form* form);  
  void init_types();
}