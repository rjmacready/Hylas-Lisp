  jmp_buf buf;
  bool testmode = false;
      
  #define NormalError   0
  #define ReaderError   1
  #define GenericError  2
  #define MacroError    3
  
  unsigned char errormode = NormalError;
  
  inline void reseterror() { errormode = NormalError; }
  
  void Unwind()
  {
    if(testmode)
      exit(-1);
    else
      longjmp(buf,0);
  }
 
  string at(Form* in)
  {
    if(master.output == HTML)
      return "<br><em>Line " + to_string<long>(in->line) + ", column "
        + to_string<int>(in->column) + ":</em><br>" + in;
    else
      return (string)"\nLine " + to_string<long>(in->line) + ", column "
        + to_string<int>(in->column) + (string)":\n" + in;
  }
  
  void error_print() {} // termination version
  
  template<typename Arg1, typename... Args>
  void error_print(const Arg1& arg1, const Args&... args)
  {
      cout << arg1;
      error_print(args...); // note: arg1 does not appear here!
  }
  
  template<typename... T>
  void error(Form* head, T const& ... text)
  {
    switch(errormode)
    {
      case NormalError:
        if(master.output == HTML)
          cout << "<div class='normalerror'><strong><a href src='docs/Errors#Normal_Errors'>Normal Error</a>:</strong> ";
        else
          cout << "Normal Error: ";
        break;
      case ReaderError:
        if(master.output == HTML)
          cout << "<div class='readererror'><strong><a href src='docs/Errors#Reader_Errors'>Reader Error</a>:</strong> ";
        else
          cout << "Reader Error: ";
        break;
      case GenericError:
        if(master.output == HTML)
          cout << "<div class='genericerror'><strong><a href src='docs/Errors#Generic_Errors'>Error during Generic Expansion</a>:</strong> ";
        else
          cout << "Error during Generic Expansion: ";
        break;
      case MacroError:
        if(master.output == HTML)
          cout << "<div class='macroerror'><strong><a href src='docs/Errors#Macro_Errors'>Error during Macro Expansion</a>:</strong> ";
        else
          cout << "Error during Macro Expansion: ";
        break;        
    }
    error_print(text...);
    cout << at(head);
    if(master.output == HTML)
      cout << "</div>";
    Unwind();
  }