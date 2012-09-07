  map<string,string> WordMacros;
  
  void addWordMacro(string word, string replacement)
  {
    map<string,string>::iterator seeker = WordMacros.find(word);
    if(seeker != WordMacros.end())
    {
      if(allow_RedefineMacros)
        WordMacros[word] = replacement;
      else
      {
        printf("WARNING: Redefining macro '%s' from '%s' to '%s'.",
               word.c_str(), WordMacros[word].c_str(), replacement.c_str());
        WordMacros[word] = replacement;
      }
    }
    else
    {
      WordMacros[word] = replacement;
    }  
  }
  
  string getMacro(string word)
  {
    map<string,string>::iterator seeker = WordMacros.find(word);
    if(seeker != WordMacros.end())
      return seeker->second;
    else
      return word;
  }
  
  inline Form* cons(Form* first, Form* second)
  {
    Form* self = new Form;
    tag(self) = List;
    car(self) = first;
    cdr(self) = second;
    return self;
  }
  
  Form* append(Form* first, Form* second)
  {
    if(first == NULL)
    {
      if(isatom(second))
      {
        printf("ERROR: (append) takes two lists, but the second argument was an atom.");
        Unwind();
      }
      else
        return second;
    }
    if(isatom(first))
    {
      printf("ERROR: (append) takes two lists, but the first argument was an atom.");
      Unwind();
    }
    if(isatom(second))
    {
      printf("ERROR: (append) takes two lists, but the second argument was an atom.");
      Unwind();
    }
    else
      return cons(car(first),append(cdr(first),second));
  }
  
  inline unsigned long length(Form* in)
  {
    long count = 0;
    if(in == NULL)
    {
      return 0;
    }
    Form* node = cdr(in);
    while(node != NULL && tag(node) == List)
    {
      ++count;
      node = cdr(node);
    }
    return count+1;
  }
  
  inline Form* nth(Form* in, long location)
  {
    if(in == NULL)
    {
      printf("ERROR: nth() signalled and 'out of range' error \
when trying to access the %li-th element of the form:\n%s\n\
whose length is %li.",location,preprint(in).c_str(),length(in));
      Unwind();
    }
    if(location == 0)
      return car(in);
    if(location < 0)
    {
      printf("ERROR: nth() says you can't use a negative number for nth.");
      Unwind();
    }
    Form* node = in;
    while(location > 0)
    {
      node = cdr(node);
      if(node == NULL)
      {
        printf("ERROR: nth() signalled and 'out of range' error \
when trying to access the %li-th element of the form:\n%s\n\
whose length is %li.",location,preprint(in).c_str(),length(in));
        Unwind();
      }
      --location;
    }
    return car(node);
  }

  long curline;
  int curcolumn;

  inline Form* makeForm(string in, bool tag)
  { Form* self = new Form; tag(self) = tag; val(self) = in;
    self->line = curline; self->column = curcolumn; return self; }
  
  inline void clear_reader()
  { curline, curcolumn = 0; }  
  
  inline char next_char(FILE* in)
  {
    curcolumn++;
    return getc(in);
  }
  
  inline void unget_char(char c, FILE* in)
  {
    curcolumn--;
    ungetc(c,in);
  }
  
  string next_token(FILE *in)
  {
    char ch = next_char(in);
    while(isspace(ch)) ch = next_char(in);
    if(ch == '\n')
    {
      ch = next_char(in);
      curline++;
    }
    if(ch == ')')
      return ")";
    if(ch == '(')
      return "(";
    if(ch == '"')
    {
      char text[10000];
      text[0] = '"';
      char tmp;
      int index = 1;
      ch = next_char(in);
      while(true)
      {
        if(ch == '"' && tmp != '\\')
          break;
        text[index++] = ch;
        tmp = ch;
        ch = next_char(in);
      }
      text[index++] = '"';
      text[index++] = '\0';
      return getMacro(text);
    }
    else
    {
      char buffer[300];
      int index = 0;
      while(!isspace(ch) && ch != ')' && ch != '(')
      {
        buffer[index++] = ch;
        ch = next_char(in);
      }
      buffer[index++] = '\0';
      if(ch == ')' || ch == '(') 
        unget_char(ch, in);
      return getMacro(buffer);
    }
  }
  
  Form* read_tail(FILE *in)
  {
    string token = next_token(in);
    if(token == ")")
    {
      return NULL;
    }
    else if(token == "(")
    {
      Form* first = read_tail(in);
      Form* second = read_tail(in);
      return cons(first, second);
    }
    else
    {
      Form* first = makeForm(token,Atom);
      Form* second = read_tail(in);
      return cons(first, second);
    }
  }
  
  Form* read(FILE* in)
  {
    string token = next_token(in);
    if(token == "(")
      return read_tail(in);
    if(token == ")")
      return NULL;
    Form* result = makeForm(token,Atom);
    clear_reader();
    return result;
  }
  
  Form* readFile(string filename)
  {
    FILE* ptr = fopen(filename.c_str(),"r");
    Form* tmp = read(ptr);
    fclose(ptr);
    return tmp;
  }
  
  Form* readString(string in)
  {
    FILE* ptr = fopen("reader.tmp","w+");
    fputs(in.c_str(),ptr);
    fputs("\n",ptr);
    fclose(ptr);
    return readFile("reader.tmp");
  }
  
  string preprint(Form* in)
  {
    string out;
    if(in == NULL)
      return "()";
    else if(islist(in))
    {
      out += "(";
      out += preprint(car(in));
      in = cdr(in);
      while(in != NULL && islist(in))
      {
        out += " ";
        out += preprint(car(in));
        in = cdr(in);
      }
      out += ")";
      return out;
    }
    else
    {
      return val(in);
    }
  }
  
  ostream& operator<<(ostream& out, Form* in)
  {
    out << preprint(in);
    return out;
  }
  
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
  
  #define BooleanTrue	0
  #define BooleanFalse	1
  #define Integer		2
  #define Real		3
  #define Symbol		4
  #define String		5
  #define Unidentifiable  6
  
  unsigned char analyze(string input)
  {
    if(input.length() < 1)
      Unwind();
    bool quoteFound	= false;
    unsigned int quotePosition		= 0;
    bool minusSignFound	= false;
    unsigned int minusSignPosition	= 0;
    unsigned int numberOfMinusSigns	= 0;
    bool periodFound	= false;
    unsigned int periodPosition		= 0;
    unsigned int numberOfPeriods	= 0;
    bool characterFound	= false;
    bool numericalFound	= false;
    bool eFound		= false;
    unsigned int ePosition		= 0;
    unsigned int numberOfEs		= 0;
    for(unsigned int i = 0; i <= input.length(); i++)
    {
      if(input[i] == '"')
      {
        if(!quoteFound)
        {
          quoteFound = true;
          quotePosition = i;
        }
      }
      else if(input[i] == '-')
      {
        if(!minusSignFound)
        {
          minusSignFound = true;
          minusSignPosition = i;
        }
        numberOfMinusSigns++;
      }
      else if(input[i] == '.')
      {
        if(!periodFound)
        {
          periodFound = true;
          periodPosition = i;
        }
        numberOfPeriods++;
      }
      else if(isalpha(input[i]))
      {
        characterFound = true;
      }
      else if(isdigit(input[i]))
      {
        numericalFound = true;
      }
      else if(input[i] == 'e' || input[i] == 'E')
      {
        if(!eFound)
        {
          eFound = true;
          ePosition = i;
        }
        numberOfEs++;
      }
    }
    if(quoteFound)
    {
      if(quotePosition == 0)
      {
        if(input[input.length()-1] == '"')
          return String;
      }
    }
    if(numericalFound)
    {
      if(characterFound && !eFound)
        return Symbol;
      else if(periodFound || eFound)
      {
        if(periodPosition != input.length()-1 && numberOfPeriods == 1)
        {
          if(isdigit(input[periodPosition+1]))
          {
            return Real;
          }
        }
        else if(ePosition != 0 && ePosition != input.length()-1 && numberOfEs == 1)
        {
          if(numberOfEs == 1 && isdigit(input[ePosition+1]) && isdigit(input[ePosition+1]))
          {
            return Real;
          }
        }
      }
      else
        return Integer;
    }
    else if(input == "true")
      return BooleanTrue;
    else if(input == "false")
      return BooleanFalse;
    else
      return Symbol;
    return Unidentifiable;
  }
