  jmp_buf buf;
  bool testmode = false;
  
  void Unwind()
  {
    if(testmode)
      exit(-1);
    else
      longjmp(buf,0);
  }
  
  template <typename T>
  void var_print(const T& value)
  {
    cout << value << endl;
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
  #define MacroError		"Macro Error:"

  template <typename... T>
  void signal_error(string type, const T&... text)
  {
    cout << type;
    var_print(text...);
	Unwind();
  }