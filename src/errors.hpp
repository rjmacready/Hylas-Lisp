  jmp_buf buf;
  bool testmode = false;
  
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
      return (string)"(Line " + to_string<long>(in->line) + ", column " + to_string<int>(in->column) + (string)")";
    else
      return "<em>(Line " + to_string<long>(in->line) + ", column " + to_string<int>(in->column) + ")</em>";
  }
  
  #define ReaderError	"Reader Error:"
  #define NormalError	"Normal Error:"
  #define GenericError	"Generic Error:"
  #define MacroError	"Macro Error:"

  template<typename T>
  void error(T const& t)
  {
    cerr << t;
  }
  
  template<typename ... T>
  void error(string type, T const& ... text)
  {
    cerr << type;
    error(text ...);
    Unwind();
  }