  jmp_buf buf;
  bool testmode = false;
      
  #define NormalError   0
  #define ReaderError   1
  #define GenericError  2
  #define MacroError    3
  
  inline void reseterror() { master.errormode = NormalError; }
  
  void Unwind()
  {
    if(testmode)
      exit(-1);
    else
      throw '\0';
  }
  
  string getError()
  { string tmp = master.errmsg; master.errmsg.clear(); return tmp; }
 
  string at(Form* in)
  {
    if(master.output == HTML)
      return "<br><em>Line " + to_string<long>(in->line) + ", column "
        + to_string<int>(in->column) + ":</em><br>" + in;
    else
      return (string)"\nLine " + to_string<long>(in->line) + ", column "
        + to_string<int>(in->column) + (string)":\n" + in;
  }
  
  inline string print(string in){ return in; }
  inline string print(char* in){ return string(in); }
  inline string print(const char* in){ return string(in); }
  inline string print(const char& in){ return string((const char*)in); }
  
  void error_print() {} // termination version
  
  template<typename Arg1, typename... Args>
  void error_print(const Arg1& arg1, const Args&... args)
  {
      master.errmsg += print(arg1);
      error_print(args...); // note: arg1 does not appear here!
  }
  
  void print_errormode()
  {
    switch(master.errormode)
    {
      case NormalError:
        if(master.output == HTML)
          master.errmsg += "<div class='error normalerror'><strong><a href src='Errors.html#Normal_Errors'>Normal Error</a>:</strong> ";
        else
          master.errmsg += "Normal Error: ";
        break;
      case ReaderError:
        if(master.output == HTML)
          master.errmsg += "<div class='error readererror'><strong><a href src='Errors.html#Reader_Errors'>Reader Error</a>:</strong> ";
        else
          master.errmsg += "Reader Error: ";
        break;
      case GenericError:
        if(master.output == HTML)
          master.errmsg += "<div class='error genericerror'><strong><a href src='Errors.html#Generic_Errors'>Error during Generic Expansion</a>:</strong> ";
        else
          master.errmsg += "Error during Generic Expansion: ";
        break;
      case MacroError:
        if(master.output == HTML)
          master.errmsg += "<div class='error macroerror'><strong><a href src='Errors.html#Macro_Errors'>Error during Macro Expansion</a>:</strong> ";
        else
          master.errmsg += "Error during Macro Expansion: ";
        break;
    }
  }
  
  template<typename... T>
  void error(Form* head, T const& ... text)
  {
    print_errormode();
    error_print(text...);
    master.errmsg += at(head);
    if(master.output == HTML)
      master.errmsg += "</div>";
    Unwind();
  }
  
  template<typename... T>
  void nerror(T const& ... text)
  {
    print_errormode();
    error_print(text...);
    if(master.output == HTML)
      master.errmsg += "</div>";
    Unwind();
  }
  
  template<typename... T>
  void warn(Form* head, T const& ... text)
  {    
    error_print(text...);
    master.errmsg += at(head);
    if(master.output == HTML)
      master.errmsg += "</div>";
  }