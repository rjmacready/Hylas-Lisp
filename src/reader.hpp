  map<string,string> WordMacros;
  map<char,string> Prefixes;
  map<char,string> Postfixes;
  
  void addWordMacro(string word, string replacement)
  {
    map<string,string>::iterator seeker = WordMacros.find(word);
    if(seeker != WordMacros.end())
    {
      if(master.allow_RedefineMacros)
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
  
  string tryPrefixOrPostfix(string word)
  {
    if(word.length() < 3)
      return word;
    char prefix = word[0];
    char postfix = word[word.length()-1];
    map<char,string>::iterator seeker = Prefixes.find(prefix);
    if(seeker != Prefixes.end()) //Found, apply
    {
      word = cutfirst(word);
      word = seeker->second+word;
    }
    seeker = Postfixes.find(postfix);
    if(seeker != Postfixes.end()) //Found, apply
    {
      word = cutlast(word);
      word = word+seeker->second;
    }
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
whose length is %li.",location,print(in).c_str(),length(in));
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
whose length is %li.",location,print(in).c_str(),length(in));
        Unwind();
      }
      --location;
    }
    return car(node);
  }
  
  unsigned long curline = 0;
  unsigned int curcolumn = 0;

  inline Form* makeForm(string in, bool tag)
  { Form* self = new Form; tag(self) = tag; val(self) = in;
    self->line = curline; self->column = curcolumn; return self; }
  
  inline void clear_reader()
  { curline = 0; curcolumn = 0; }  
  
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
    char tmp;
    while(isspace(ch)) ch = next_char(in);
    if(ch == '\n')
    {
      ch = next_char(in);
      curline++;
      curcolumn = 0;
    }
    if(ch == '!')
    {
      tmp = ch;
      ch = next_char(in);
      if(ch == '-')
      {
        ch = next_char(in);
        while(true)
        {
          if(tmp == '-' && ch == '!')
            { ch = next_char(in); break; }
          if(tmp == '\n')
            curcolumn = 0;
          tmp = ch;
          ch = next_char(in);
        }
      }
      else
        while(ch != '\n') ch = next_char(in);
    }
    if(ch == ')')
      return ")";
    if(ch == '(')
      return "(";
    if(ch == '"')
    {
      char text[10000];
      text[0] = '"';
      int index = 1;
      ch = next_char(in);
      while(true)
      {
        if(ch == '"' && tmp != '\\')
          break;
        if(tmp == '\n')
          curcolumn = 0;
        text[index++] = ch;
        tmp = ch;
        ch = next_char(in);
      }
      text[index++] = '"';
      text[index++] = '\0';
      return string(text);
    }
    char text[300];
    int index = 0;
    while(!isspace(ch) && ch != ')' && ch != '(')
    {
      text[index++] = ch;
      ch = next_char(in);
    }
    text[index++] = '\0';
    if(ch == ')' || ch == '(') 
      unget_char(ch, in);
    return text;
  }
  
  Form* read_tail(FILE *in)
  {
    string token = tryPrefixOrPostfix(getMacro(next_token(in)));
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
    errormode = ReaderError;
    string token = next_token(in);
    if(token == "(")
      return read_tail(in);
    if(token == ")")
      return NULL;
    Form* result = makeForm(token,Atom);
    reseterror();
    return result;
  }
  
  Form* readFile(string filename)
  {
    errormode = ReaderError;
    FILE* ptr = fopen(filename.c_str(),"r");
    Form* tmp = read(ptr);
    fclose(ptr);
    reseterror();
    return tmp;
  }
  
  Form* readString(string in)
  {
    errormode = ReaderError;
    FILE* ptr = fopen("reader.tmp","w+");
    fputs(in.c_str(),ptr);
    fputs("\n",ptr);
    fclose(ptr);
    reseterror();
    return readFile("reader.tmp");
  }
  
  string print(Form* in)
  {
    if(master.output == HTML)
    {
      
    }
    else
    {
      string out;
      if(in == NULL)
        return "()";
      else if(islist(in))
      {
        out += "(";
        out += print(car(in));
        in = cdr(in);
        while(in != NULL && islist(in))
        {
          out += " ";
          out += print(car(in));
          in = cdr(in);
        }
        out += ")";
        return out;
      }
    }
    return val(in);
  }
  
  #define BooleanTrue	        0
  #define BooleanFalse	        1
  #define Integer	        2
  #define Character             3
  #define Real		        4
  #define Symbol	        5
  #define String	        6
  #define Unidentifiable        7
  
  /*unsigned char analyze2(string input)
  {
    if(input == "true")
      return BooleanTrue;
    else if(input == "false")
      return BooleanFalse;
    if(regex_match (s,e))
      cout << "string object matched\n";
    else if(regex_match(input,regex("[-+]?\d+")))
      return Integer;
    else if(regex_match(input,regex("\'[a-zA-Z0-9]\'")))
      return Character;
    else if(regex_match(input,regex("[-+]?\d\+\.?([eE][+-]?)?\d+")))
      return Real;
    else if(regex_match(input,regex("[.]+")))
      return Symbol;
    else if(regex_match(input,regex("\"[.\n\r]*\"")))
      return String;
    else
      return Unidentifiable;
  }*/
  
  unsigned char analyze(string input)
  {
    bool doubleQuoteFound               = false;
    bool singleQuoteFound               = false;
    bool minusSignFound	                = false;
      unsigned long minusSignPosition	= 0;
      unsigned long numberOfMinusSigns	= 0;
    bool periodFound	                = false;
      unsigned long periodPosition	= 0;
      unsigned long numberOfPeriods	= 0;
    bool characterFound	                = false;
    bool numericalFound                 = false;
    bool eFound		                = false;
      unsigned long ePosition		= 0;
      unsigned long numberOfEs		= 0;
    for(unsigned long i = 0; i <= input.length(); i++)
    {
      if(input[i] == '"')
      {
        if(!doubleQuoteFound)
        {
          doubleQuoteFound = true;
        }
      }
      if(input[i] == '\'')
      {
        if(!singleQuoteFound)
        {
          singleQuoteFound = true;
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
    if(doubleQuoteFound)
    {
      if(input[input.length()-1] == '"' && input[0] == '"')
        return String;
    }
    if(singleQuoteFound)
    {
      if(input[input.length()-1] == '\'')
        return Character;
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