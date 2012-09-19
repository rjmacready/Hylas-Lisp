  jmp_buf buf;
  bool testmode = false;
  unsigned char errormode;
    
  #define NormalError   0
  #define ReaderError   1
  #define GenericError  2
  #define MacroError    3
  
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
      return (string)"\nLine " + to_string<long>(in->line) + ", column "
        + to_string<int>(in->column) + (string)":\n" + in;
    else
      return "<br><em>Line " + to_string<long>(in->line) + ", column "
        + to_string<int>(in->column) + ":</em><br>" + in;
  }
  
  template<typename T>
  void error(T const& t)
  {
    cout << t;
  }
  
  template<typename ... T>
  void error(T const& ... text)
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
          cout <<  "Reader Error: ";
        break;
      case GenericError:
        if(master.output == HTML)
          cout << "<div class='genericerror'><strong><a href src='docs/Errors#Generic_Errors'>Error during Generic Expansion</a>:</strong> ";
        else
          cout <<  "Error during Generic Expansion: ";
        break;
      case MacroError:
        if(master.output == HTML)
          cout << "<div class='macroerror'><strong><a href src='docs/Errors#Macro_Errors'>Error during Macro Expansion</a>:</strong> ";
        else
          cout <<  "Error during Macro Expansion: ";
        break;        
    }
    error(text ...);
    if(master.output == HTML)
      cout << "</div>";
    Unwind();
  }