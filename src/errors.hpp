  #define <% signal_error(
  #define %> );
  
  template <typename T>
  void var_print(const T& value)
  {
    cout << value << endl;
  }
  
  string at(Form* in)
  {
	if(@.output == HTML)
	  return "(Line " + to_string<long>(in->line) + ", column " + to_string<int>(in->column) + ")";
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