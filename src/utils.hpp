  template <typename T>
  inline string to_string(T datum)
  {
    stringstream output;
    output << datum;
    return output.str();
  }
  
  template <typename T>
  inline T from_string(string datum)
  {
    stringstream stream(datum);
    T out;
    stream >> out;
    return out;
  }
  
  string dump_form(Form* input)
  {
    string out = (string)"Form: " + (isatom(input) ? "Atom" : "List") + "\n'";
    out += input + (string)"'\n";
    return out + "(Line " + to_string(input->line) + ", column " + to_string(input->column) + ")";
  }
  
  string cutlast(string in)
  {
    return string(in,0,in.length()-1);
  }