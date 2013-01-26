/*!
 * @mainpage Hylas Lisp
 * 
 * @section intro Introduction
 * Hylas Lisp is a statically-typed, wide-spectrum, JIT-compiled Lisp dialect
 * targetting the LLVM. It is built on the idea that languages should not be
 * limited to application domains or levels of abstraction.
 */

/*!
 * @file hylas.cpp
 * @brief Implementation of the Hylas code generator and JIT.
 * @author Eudoxia
 */

namespace Hylas
{
  using namespace std;
  using namespace llvm;
  
  ostream& operator<<(ostream& out, Form* in)
  { out << print(in); return out; }
  
  string operator+(string out, Form* in)
  { return out + print(in); }
  
  string operator+(Form* in, string out)
  { return print(in) + out; }
  
  inline void push(string in)
  {
    master.CodeStack.push_back(in);
  }
  
  inline void persistentPush(string in)
  {
    master.Persistent.push_back(in);
  }
  
  inline void error_unbound(Form* x)
  { error(x,"Symbol '",print(x),"' is unbound."); }
  
  Variable* lookup(string in)
  {
    for(long i = ScopeDepth; i != -1; i--)
    {
      //cerr << "Looking for " << in << " in scope " << i << endl;
      map<string,Variable>::iterator seeker = master.SymbolTable[i].vars.find(in);
      if(seeker != master.SymbolTable[i].vars.end())
      {
        //cerr << "Found it" << endl;
        (&seeker->second)->scope_address = i;
        return &seeker->second;
      }
    }
    //cerr << "Didn't find shit" << endl;
    return NULL;
  }
  
  /*
   * TEMPORARY REGISTERS
   */
  
  inline string gensym() { return "%uniqueness_guaranteed_" + to_string<int>(rand()); }
  
  inline string get_unique_tmp()
  { return "%tmp.version" + to_string(++tmp_version); }
  
  inline string get_tmp(long v)
  { return "%tmp.version" + to_string(v); }
  
  inline string get_current_tmp(){ return get_tmp(tmp_version);}
  
  /*
   * RESULT REGISTERS
   */
  
  string get_unique_res(string type)
  {
    string vnum = to_string(++res_version);
    master.SymbolTable[ScopeDepth].vars["%res.version" + vnum].type = type;
    return "%res.version" + vnum;
  }
  
  string get_unique_res_address(string type, string address, bool symbolic)
  {
    string vnum = to_string(++res_version);
    master.SymbolTable[ScopeDepth].vars["%res.version" + vnum].type = type;
    master.SymbolTable[ScopeDepth].vars["%res.version" + vnum].address = address;
    master.SymbolTable[ScopeDepth].vars["%res.version" + vnum].regtype = (symbolic?SymbolicRegister:LValue);
    master.SymbolTable[ScopeDepth].vars["%res.version" + vnum].scope_address = ScopeDepth/*((ScopeDepth==-1)?0:ScopeDepth)*/;
    return "; <with address " + address + ">\n%res.version" + vnum;
  }
  
  inline string get_res(long v)
  { return "%res.version" + to_string(v); }
  
  string res_type(string name)
  { 
    Variable* tmp = lookup(name);
    if(tmp == NULL)
    { nerror("Can't find register '",name,"'."); }
    return tmp->type;
  }
  
  inline string get_current_res(){ return get_res(res_version);}
  
  /*
   * LABELS
   */
  
  string get_unique_label()
  { return "%label.version" + to_string(++label_version); }
  
  string get_label(long v)
  { return "%label.version" + to_string(v); }
  
  inline string get_current_label(){ return get_label(label_version);}
  
  inline string functional_label(string in)
  {
    return cutfirst(in) + ":\n";
  }
  
  /*
   * REGISTERS
   */
  
  inline string allocate(string address, string type)
  { return address + " = alloca " + type + "\n"; }
  
  inline string store(string type, string value, string address)
  { return "store " + type + " " + value + ", " + type + "* " + address + "\n"; }
  
  inline string load(string destination, string type, string source)
  { return destination + " = load " + type + "* " + source; }
  
  inline string latest_type()
  { return res_type(get_current_res()); }
  
  inline string constant(string destination, string type, string value)
  { return destination + " = select i1 true, " + type + " " + value + ", " + type + " " + value; }
  
  void dump_scope(unsigned long s)
  {
    for(map<string,Variable>::iterator i = master.SymbolTable[s].vars.begin();
        i != master.SymbolTable[s].vars.end(); i++)
    {
      printf("\n %s : %s",i->first.c_str(),i->second.type.c_str());
    }
  }
  
  inline bool isNamespaced(string in)
  {
    if(in.find(":") != string::npos)
      return true;
    return false;
  }
  
  string getNamespace(string in)
  {
    if(!isNamespaced(in))
    {
      for(unsigned long i = 0; i < master.NamespaceStack.size(); i++)
      {
        in = master.NamespaceStack[i] + ":" + in;
      }
    }
    return in;
  }
  
  string emitCode(Form* form/*, emissionContext ctx = Bottom*/)
  {
    string out;
    if(form == NULL)
      error(form,"Can't emit code for the null form.");
    else if(isatom(form))
    {
      switch(analyze(val(form)))
      {
        case BooleanTrue:
        {
          out = constant(get_unique_res("i1"),"i1","true");
          break;
        }
        case BooleanFalse:
        {
          out = constant(get_unique_res("i1"),"i1","false");
          break;
        }
        case Integer:
        {
          out = constant(get_unique_res("i64"),"i64",val(form));
          break;
        }
        case Character:
        {
          string c = string(val(form),1,val(form).length()-2);
          string address = "@___string" + to_string<unsigned long>(++string_version);
          push(address + " = global [2 x i8] c\"" + c + "\0\0\"");
          out += get_unique_res("i8") + " = load i8* getelementptr inbounds ([2 x i8]* " + address + ", i32 0, i64 0)";
          break;
        }
        case Real:
        {
          out = constant(get_unique_res("double"),"double",val(form));
          break;
        }
        case String:
        {
          //Remember strings come with their double quotes
          //Also convert them to unicode
          string str = cutboth(val(form));
          unsigned long length = str.length();
          stringstream ss;
          string result;
          for(unsigned long i = 0; i < str.length(); i++)
          { 
            string tmp;
            if(str[i] == '\\')
            {
              //Oh goodness, escape sequences
              i++;
              switch(str[i])
              {
                case 'n':
                  ss << "0A";
                  break;
                case '\\':
                  ss << "5C";
                  break;
                case '"':
                  ss << "22";
                  break;
                case '0':
                  ss << "00";
                  break;
                case 'a':
                  ss << "07";
                  break;
                case 'b':
                  ss << "08";
                  break;
                case 'f':
                  ss << "0C";
                  break;
                case 'r':
                  ss << "0D";
                  break;
                case 't':
                  ss << "09";
                  break;
                case 'v':
                  ss << "0B";
                  break;
                case 'x':
                  //TODO: Manage hex input
                  break;
                case 'o':
                  //TODO: Manage octal input
                  break;
                case 'U':
                  //TODO: Great, a unicode codepoint...
                  break;
                default:
                  error(form,"Unknown character escape sequence.");
              }
              length--;
            }
            else
            {
              ss << hex << (int)str[i];
            }
            tmp = ss.str();
            if(tmp.length() > 2)
            {
              tmp = string(tmp,tmp.length()-2);
            }
            result += '\\' + tmp;
            //cerr << "Result: " << result << endl;
          }
          string type = "[" + to_string<unsigned long>(length+1) + " x i8]";
          push("@___string" + to_string<unsigned long>(++string_version) + " = global " + type + " c\"" + result + "\\00\"");
          out = get_unique_res("i8*") + " = getelementptr " + type + "* @___string" + to_string<unsigned long>(string_version) + ", i64 0, i64 0";
          break;
        }
        case Symbol:
        {
          string sym = val(form);
          Variable* tmp = lookup(sym);
          if(tmp == NULL)
            error_unbound(form);
          else
            out = load(get_unique_res_address(tmp->type,tmp->address,true),tmp->type,((tmp->global) ? "@" : "%")
                + sym+to_string(tmp->scope_address));
          break;
        }
        case Unidentifiable:
        {
          error(form,"Received an unidentifiable form as input.");
          break;
        }
      }
    }
    else
    {
      if(islist(car(form)))
        error(form,"Lists can't be used as function names in calls. Until I implement lambda.");
      string func = val(car(form));
      map<string,hFuncPtr>::iterator seeker = Core.find(func);
      if(seeker != Core.end())
        out = seeker->second(form);
      else
        out = callFunction(form);
    }
    return out+"\n";
  }
  
  IR Compile(Form* form)
  {
    string out;
    string tmp;
    if(form == NULL)
      error(form,"Can't emit code for the null form.");
    else if(isatom(form))
    {
      if(val(form) == "quit")
        exit(0);
      else if(val(form) == "IR")
      {
        master.Program->dump();
        nerror("Dumped IR.");
      }
      else if(val(form) == "debug")
      {
        master.debug = !master.debug;
        nerror("Debug mode is ",(master.debug?"on":"off"),".");
      }
      else
        out = emitCode(form);
    }
    else
      out = emitCode(form/*,Top*/);
    for(unsigned long i = 0; i < master.Persistent.size(); i++)
      tmp += master.Persistent[i] + "\n";
    for(unsigned long i = 0; i < master.CodeStack.size(); i++)
      tmp += master.CodeStack[i] + "\n";
    out = "define " + latest_type() + " @entry(){\n" + out + "\nret " + latest_type() + " " + get_current_res() + "\n}";
    out = tmp + out;
    string type = latest_type();
    master.CodeStack.clear();
    clear_reader();
    return {out,type};
  }

  IR JIT(IR code)
  {
    SMDiagnostic errors;
    string parser_errors;
    ParseAssemblyString(code.assembly.c_str(),master.Program,errors,Context);
	if(master.debug)
	{
	  cerr << "Code:\n" << code.assembly << "\nIR:" << endl;
	  master.Program->dump();
	}
    llvm::Function* entryfn = master.Engine->FindFunctionNamed("entry");
    if(entryfn == NULL)
      nerror("ERROR: Couldn't find program entry point.");
    if(!errors.getMessage().empty())
    {
      entryfn->eraseFromParent();
      nerror("IR Parsed with errors: ",errors.getMessage(),"\nCode: \n",code.assembly);
    }
    if(verifyModule(*master.Program,ReturnStatusAction,&parser_errors))
    {
      entryfn->eraseFromParent();
      nerror("IR Parser Error: ",parser_errors);
    }
    master.Passes.run(*master.Program);
    return code;
  }

  string Run()
  {
    llvm::Function* entryfn = master.Engine->FindFunctionNamed("entry");
    if(entryfn == NULL)
      nerror("Couldn't find program entry point.");
    std::vector<GenericValue> args;
    GenericValue retval = master.Engine->runFunction(entryfn,args);
    master.Engine->freeMachineCodeForFunction(entryfn);
    entryfn->eraseFromParent();
    return /*""*/string((char*)(retval.PointerVal));
  }
  
  void init_optimizer()
  {
    /*master.Passes.add(createBasicAliasAnalysisPass());
    master.Passes.add(createInstructionCombiningPass());
    master.Passes.add(createReassociatePass());
    master.Passes.add(createGVNPass());
    master.Passes.add(createCFGSimplificationPass());
    master.Passes.add(createAggressiveDCEPass());*/
  }
   
  void init()
  {
    InitializeNativeTarget();
	master.version = 0.9;
    master.Program = new Module("Hylas Lisp",Context);
    master.Engine = ExecutionEngine::createJIT(master.Program);
    master.Loader = new Linker("Hylas Lisp",master.Program);
    master.Loader->addSystemPaths();
    master.allow_RedefineMacros = true;
    master.allow_RedefineWordMacros = true;
    master.allow_RedefinePrePostfixes = true;
    master.allow_RedefineFunctions = false;
    master.Colorscheme = defaultColorscheme();
    master.CSS = defaultCSS();
    master.errormode = NormalError;
    init_stdlib();
    init_types();
    init_optimizer();
    master.Engine =  EngineBuilder(master.Program).create();
	try
	{
	  ifstream base("src/base.hylas");
	  if(!base.good())
		nerror("Could not find base.hylas. You will not have print functions for the basic types.");
	  stringstream file;
	  file << base.rdbuf();
	  JIT(Compile(readString(file.str())));
	  Run();
	}
	catch(exception except)
    {
      cerr << getError() << endl;
	  exit(-1);
    }
  }
  
  void restart()
  {
    init();
  }
  
  RGB HSV_to_RGB(float h, float s, float v)
  {
    int h_i = (int)(h*6);
    float f = h*6-h_i;
    float p = v*(1-s);
    float q = v*(1-f*s);
    float t = v*(1-((1-f)*s));
    float r=0,g=0,b=0;
    if(h_i == 0)
    {
      r=v;
      g=t;
      b=p;
    }
    if(h_i == 1)
    {
      r=q;
      g=v;
      b=p;
    }
    if(h_i == 2)
    {
      r=p;
      g=v;
      b=t;
    }
    if(h_i == 3)
    {
      r=p;
      g=q;
      b=v;
    }
    if(h_i == 4)
    {
      r=t;
      g=p;
      b=v;
    }
    if(h_i == 5)
    {
      r=v;
      g=p;
      b=q;
    }
    return RGB((unsigned int)(r*256),
               (unsigned int)(g*256),
               (unsigned int)(b*256));
  }
  
  RGB genColor()
  {
    static double golden_ratio = 0.618033988749895;
    srand(time(NULL));
    static int h = rand();
    h += golden_ratio;
    h = h%1;
    return HSV_to_RGB(h,0.5,0.95);    
  }
  
  string exportRGB(RGB in)
  {
    return "<span style='color:rgb(" + to_string(in.Red)
          + "," + to_string(in.Green)
          + "," + to_string(in.Blue) + ");display:inline;'>";
  }
  
  string matchKeyword(string in)
  {
    //Is it a constant?
    switch(analyze(in))
    {
      case BooleanTrue:
        in = exportRGB(master.Colorscheme.find("BooleanTrue")->second) + in + "</span>";
        break;
      case BooleanFalse:
        in = exportRGB(master.Colorscheme.find("BooleanFalse")->second) + in + "</span>";
        break;
      case Integer:
        in = exportRGB(master.Colorscheme.find("Integer")->second) + in + "</span>";
        break;
      case Character:
        in = exportRGB(master.Colorscheme.find("Character")->second) + in + "</span>";
        break;
      case Real:
        in = exportRGB(master.Colorscheme.find("Real")->second) + in + "</span>";
        break;
      case String:
        in = exportRGB(master.Colorscheme.find("String")->second) + in + "</span>";
        break;
      case Symbol:
      {
        //Is it a TopLevel or Core function?
        if(Core.find(in) != Core.end())
          in = exportRGB(master.Colorscheme.find("Core")->second) + in + "</span>";
        //Is it a symbol?
        else if(lookup(in) != NULL)
          in = exportRGB(master.Colorscheme.find("Symbol")->second) + in + "</span>";
        //Is it a type?
        else if(checkTypeExistence(in))
          in = exportRGB(master.Colorscheme.find("Type")->second) + in + "</span>";
        else if(checkGenericExistence(in,true) ||
                checkGenericExistence(in,false))
          in = exportRGB(master.Colorscheme.find("Generic")->second) + in + "</span>";
        //lol wtf is it just return it
        break;
      }
      case Unidentifiable:
      {
        nerror("Received an unidentifiable form as input.");
        break;
      }
    }
    return in;
  }

  map<string,RGB> defaultColorscheme()
  {
    map<string,RGB> tmp;
    tmp.insert(pair<string,RGB>("BooleanTrue",RGB(0,205,0)));
    tmp.insert(pair<string,RGB>("BooleanFalse",RGB(238,0,0)));
    tmp.insert(pair<string,RGB>("Integer",RGB(227,207,87)));
    tmp.insert(pair<string,RGB>("Character",RGB(255,174,185)));
    tmp.insert(pair<string,RGB>("Real",RGB(255,211,155)));
    tmp.insert(pair<string,RGB>("String",RGB(255,62,150)));
    tmp.insert(pair<string,RGB>("Core",RGB(255,97,3)));
    tmp.insert(pair<string,RGB>("Symbol",RGB(113,198,113)));
    tmp.insert(pair<string,RGB>("Type",RGB(72,118,255)));
    tmp.insert(pair<string,RGB>("Generic",RGB(159,121,238)));
    return tmp;
  }

  CSS defaultCSS()
  {
    string tmp;
    tmp = ".error { border: 1px solid #aaa; box-shadow: 5px 5px 5px #ccc;\
    margin: 10px; padding: 10px; } ";
    tmp += ".normalerror { background: #B2DFEE; }";
    tmp += ".readererror { background: #CDCDB4; }";
    tmp += ".genericerror { background: #000000; }";
    tmp += ".macroerror { background: #000000; }";
    return tmp;
  }

  CSS getCSS()
  { return "<style type=\"text/css\">" + master.CSS + "</style>"; }
}
