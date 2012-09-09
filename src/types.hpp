  struct Type;
  struct Generic;
  bool isInteger(string in);
  bool isCoreType(string in);
  bool isArgument(Form* in, map<string,Form*> arguments);
  Form* editForm(Form* in, map<string,Form*> replacements);
  string specializeType(Generic in, map<string,Form*> replacements, string signature);
  string printTypeSignature(Form* form);
  bool checkTypeExistence(string name);
  string makeType(Form* in);
  string makeStructure(Form* in);
  bool checkGenericExistence(string name, bool id);
  Generic writeGeneric(Form* in, bool type);
  void addGeneric(string name, Generic in);
  Generic addGenericMethod(string name, Form* in);
  Generic makeGeneric(Form* in);
  #define typeSimple    true
  #define typeStructure false
  
  struct Type
  {
    bool id;
    string contents;
    vector<Form*> methods;
  };
  
  #define typeFunction    true
  
  struct Generic
  {
    vector<string> arguments;
    bool id;
    Form* code;
    vector<string> specializations;
    vector<Form*> methods;
  };
  
  vector<string> CoreTypes;
  map<string,Type> BasicTypes;
  vector<pair<string,Generic> > Generics;
  
  bool isInteger(string in)
  {
    return ((in[0] == 'i') && (1 < from_string<long>(string(in,1,in.length()-1)) < (pow(2,23)-1)));
  }
  
  bool isCoreType(string in)
  {
    if(isInteger(in))
      return true;
    for(unsigned int i = 0; i < CoreTypes.size(); i++)
    {
      if(CoreTypes[i] == in)
        return true;
    }
    return false;
  }
  
  bool isArgument(Form* in, map<string,Form*> arguments)
  {
    if(isatom(in))
    {
      map<string,Form*>::iterator seeker = arguments.find(val(in));
      if(seeker != arguments.end())
        return true;
      return false;      
    }
    printf("ERROR: Can't provide a list to isArgument.");
    Unwind();
  }
  
  Form* editForm(Form* in, map<string,Form*> replacements)
  {
    if(in == NULL)
      return NULL;
    else if(islist(in))
    {
      Form* out;
      out = editForm(car(in),replacements);
      in = cdr(in);
      if(in == NULL)
      {
        out = cons(out,NULL);
      }
      else
      {
        while(in != NULL && islist(in))
        {
          out = append((isatom(out) ? cons(out,NULL) : out),
                      (isatom(car(in)) ? cons(editForm(car(in),replacements),NULL) : cons(editForm(car(in),replacements),NULL)));
          in = cdr(in);
        }
      }
      return out;
    }
    else
    {
      if(isArgument(in,replacements))
      {
        map<string,Form*>::iterator seeker = replacements.find(val(in));
        return seeker->second;
      }
      else
        return in;
    }
  }
  
  string specializeType(Generic in, map<string,Form*> replacements, string signature)
  {
    Form* specialization_code = editForm(in.code, replacements);
    string out = signature + " = type { ";
    for(unsigned long i = 0; i < length(cdr(cdr(specialization_code))); i++)
    {
      out += printTypeSignature(cadr(nth(cdr(cdr(specialization_code)),i))) + ",";
    }
    out = cutlast(out)+" }\n";
    printf("Specializing methods!\n");
    //Specialize methods
    for(unsigned long i = 0; i < in.methods.size(); i++)
    {
      printf("Specializing method %lu!\n",i);
      printf("Method code:\n%s\n",preprint(editForm(in.methods[i],replacements)).c_str());
      out += "\n" + emitCode(editForm(in.methods[i],replacements));
    }
    //Return the code for the specialization
    return out;
  }
  
  string printTypeSignature(Form* form)
  {
    if(form == NULL)
    { Unwind(); }
    else if(isatom(form))
    {
      string tmp = val(form);
      if(isCoreType(tmp))
      {
        return tmp;
      }
      else
      {
        map<string,Type>::iterator seeker = BasicTypes.find(tmp);
        if(seeker != BasicTypes.end())
        {
          return "%" + tmp;
        }
        else
        {
          printf("ERROR: Came across an unknown type: '%s'.",tmp.c_str());
          Unwind();
        }
      }
    }
    else if(islist(form))
    {
      if(islist(car(form)))
      { printf("ERROR: The name of a generic type can't be a list.\nCode: %s",preprint(form).c_str());
        Unwind(); }
      string type_name = val(car(form));
      if(type_name == "pointer")
      {
        return printTypeSignature(cadr(form))+"*";
      }
      for(unsigned long i = 0; i < Generics.size(); i++)
      {
        if((Generics[i].first == type_name) && (Generics[i].second.id == typeStructure))
        {
          if(Generics[i].second.arguments.size() != length(form)-1)
          {
            printf("ERROR: Wrong number of generic type arguments.");
            Unwind();
          }
          else
          {
            string signature = "%" + type_name + "_";
            unsigned long j = 1;
            for(; j < length(form); j++)
            {
              signature += printTypeSignature(nth(form,j)) + "_";
            }
            signature = cutlast(signature);
            for(j = 0; j < Generics[i].second.specializations.size(); j++)
            {
              if(Generics[i].second.specializations[j] == signature)
              {
                return signature;
              }
            }
            map<string,Form*> replacements;
            for(j = 0; j < Generics[i].second.arguments.size(); j++)
            {
              replacements[Generics[i].second.arguments[j]] = nth(form,j+1);
            }
            push(specializeType(Generics[i].second,replacements,signature));
            Generics[i].second.specializations.push_back(signature);
            for(j = 0; j < signature.length(); j++)
            {
              if(signature[j] == '*')
              {
                signature.replace(j,3,"ptr");
              }
            }
            return signature;
          }
        }
      }
      printf("ERROR: Came across an unknown generic type: '%s'.",type_name.c_str());
      Unwind();
    }
  }
  
  bool checkTypeExistence(string name)
  {
    return ((BasicTypes.find(name) != BasicTypes.end()) ? true : false);
  }
  
  string makeType(Form* in)
  {
    Type tmp;
    tmp.id = typeSimple;
    if(length(in) != 3)
    {
      printf("ERROR: Wrong number of arguments to (type).");
      Unwind();
    }
    if(isatom(cadr(in)))
    {
      if(analyze(val(cadr(in))) != Symbol)
      {
        printf("ERROR: Type name is a non-symbolic atom.");
        Unwind();
      }
    }
    string name = val(nth(in,1));
    if(checkTypeExistence(name))
    {
      printf("ERROR: Type already exists.");
      Unwind();
    }
    tmp.contents = "%"+ name + " = type " + printTypeSignature(nth(in,2));
    BasicTypes[name] = tmp;
    return tmp.contents;
  }
  
  string makeStructure(Form* in)
  {
    Type tmp;
    tmp.id = typeStructure;
    if(length(in) < 3)
    {
      printf("ERROR: Wrong number of arguments to (structure).");
      Unwind();
    }
    if(isatom(cadr(in)))
    {
      if(analyze(val(cadr(in))) != Symbol)
      {
        printf("ERROR: Structure name is a non-symbolic atom.");
        Unwind();
      }
    }
    map<string,string> fields;
    string name = val(nth(in,1));
    if(checkTypeExistence(name))
    {
      printf("ERROR: Type already exists.");
      Unwind();
    }
    for(unsigned long i = 2; i < length(in); i++)
    {
      Form* current_field = nth(in,i);
      if(islist(current_field))
      {
        if(length(current_field) != 2)
        {
          printf("ERROR: Structure fields must either be a (name type) pair or a single symbol in case of generic structures");
          Unwind();
        }
        else if(islist(car(current_field)))
        {
          printf("ERROR: Can't use a list as a field name.");
          Unwind();
        }
        else
        {
          string field_name = val(car(current_field));
          if(analyze(field_name) != Symbol)
          {
            printf("ERROR: Can't use a non-symbolic atom as a field name.");
            Unwind();
          }
          fields[field_name] = printTypeSignature(cadr(current_field));
        }
      }
      else
      {
        printf("ERROR: Structure fields must be lists, not atoms.");
        Unwind();
      }
    }
    tmp.contents += "%" + name + " = type {";
    for(map<string,string>::iterator seeker = fields.begin(); seeker != fields.end(); seeker++)
    {
      tmp.contents += seeker->second + ", ";
    }
    tmp.contents = cutlast(cutlast(tmp.contents));
    tmp.contents += "}";
    BasicTypes[name] = tmp;
    return tmp.contents;
  }
  
  bool checkGenericExistence(string name, bool id)
  {
    for(unsigned long i = 0; i < Generics.size(); i++)
    {
      if(Generics[i].first == name)
      {
        if(Generics[i].second.id == id)
          return true;
      }
    }
    return false;
  }
  
  Generic writeGeneric(Form* in, bool type)
  {
    //0        1                    2       3        4
    //(generic [structure|function] [name] ([args]) [code*]
    Generic out;
    out.id = type;
    if(type == typeStructure)
    {
      /* Now we modify the code to make it all ready for specialization
       * We star off with:
       * (generic structure derp (a) (element a))
       * And we end up with:
       * (structure derp (element a))
       */
      string name = val(nth(in,2));
      Form* code = readString("(structure " + name + ")");
      code = append(code, cdr(cdr(cdr(cdr(in)))));
      out.code = code;
    }
    else if(type == typeFunction)
    {
      /* Now we modify the code to make it all ready for specialization
       * We star off with:
       * (generic function durp (a b c)
       *   (function a ((i64 n) (i64 m)) ...))
       * And we end up with:
       * (function durp a ((i64 n) (i64 m)) ...)
       */
      string name = val(nth(in,2));
      Form* code = readString("(" + val(car(nth(in,4))) + " " + name + ")");
      code = append(code,cdr(nth(in,4)));
      out.code = code;
    }
    Form* arglist = nth(in,3);
    if(length(arglist) == 0)
    {
      printf("ERROR: Wrong number of arguments.");
      Unwind();
    }
    if(islist(arglist))
    {
      for(long i = 0; i < length(arglist); i++)
      {
        if(isatom(nth(arglist,i)))
        {
          if(analyze(val(nth(arglist,i))) != Symbol)
          {
            printf("ERROR: Arguments must be symbolic atoms.");
            Unwind();
          }
          else
          {
            out.arguments.push_back(val(nth(arglist,i)));
          }
        }
        else
        {
          printf("ERROR: Arguments must be symbolic atoms, but a list was found.");
          Unwind();
        }
      }
    }
    else
    {
      printf("ERROR: Argument list is not a list.");
      Unwind();
    }
    return out;
  }
  
  void addGeneric(string name, Generic in)
  {
    Generics.push_back(pair<string,Generic>(name,in));
  }
  
  Generic addGenericMethod(string name, Form* in)
  {
    for(unsigned long i = 0; i < Generics.size(); i++)
    {
      if(Generics[i].first == name)
      {
        if(Generics[i].second.id == typeStructure);
        {
          /*Found the proper one
           * Now we modify the code to make it all ready for specialization
           * We start off with:
           * (generic method nth of sequence
           *    (inline Type ((in (sequence Type)) (i64 n))
           *            (ret (gep Type in i64 1 i64 n))))
           * And we end up with:
           * (inline nth Type ((in (sequence Type)) (i64 n))
           *    (ret (gep Type in i64 1 i64 n)))
           */
          Form* code = readString("(" + val(car(nth(in,5))) + " " + val(nth(in,2)) + ")");
          code = append(code,cdr(nth(in,5)));
          Generics[i].second.methods.push_back(code);
          return Generics[i].second;
        }
      }
    }
  }
  
  Generic makeGeneric(Form* in)
  {
    if(length(in) < 4)
    {
      printf("ERROR: Wrong number of arguments.");
      Unwind();
    }
    if(isatom(nth(in,1)))
    {
      string text = val(nth(in,1));
      if(analyze(text) != Symbol)
      {
        printf("ERROR: The first argument to (generic) must be either 'function' or 'structure'.");
        Unwind();
      }
      else
      {
        Generic out;
        string name = val(nth(in,2));
        if(analyze(name) != Symbol)
        {
          printf("ERROR: The name of the generic must be a symbolic atom.");
          Unwind();
        }
        if(text == "structure")
        {
          out = writeGeneric(in, typeStructure);
          if(checkGenericExistence(name,typeStructure))
          {
            printf("ERROR: Generic already exists.");
            Unwind();
          }
          addGeneric(name,out);
          return out;
        }
        else if(text == "function")
        {
          out = writeGeneric(in, typeFunction);
          if(checkGenericExistence(name,typeFunction))
          {
            printf("ERROR: Generic already exists.");
            Unwind();
          }
          addGeneric(name,out);
          return out;
        }
        else if(text == "method")
        {
          if(isatom(nth(in,2)))
          {
            if(analyze(val(nth(in,2))) == Symbol)
            {
              if(isatom(nth(in,3)))
              {
                if(val(nth(in,3)) == "of")
                {
                  if(isatom(nth(in,4)))
                  {
                    if(analyze(val(nth(in,4))) == Symbol)
                    {
                      string generic_name = val(nth(in,4));
                      if(!checkGenericExistence(generic_name,typeStructure))
                      {
                        printf("ERROR: The Generic for this method doesn't exist.");
                        Unwind();
                      }
                      else
                      {
                        return addGenericMethod(generic_name,in);
                      }
                    }
                    else
                    {
                      printf("ERROR:");
                      Unwind();
                    }
                  }
                  else
                  {
                    printf("ERROR:");
                    Unwind();
                  }
                }
                else
                {
                  printf("ERROR:");
                  Unwind();
                }
              }
              printf("ERROR:");
              Unwind();
            }
            printf("ERROR:");
            Unwind();
          }
          printf("ERROR:");
          Unwind();
        }
        else
        {
          printf("ERROR: The second argument when making a generic is either 'function' or 'structure'. Some other text was received.");
          Unwind();
        }
      }
    }
    else
    {
      printf("ERROR: The first argument to (generic) must be either 'function' or 'structure', but a list was found.");
      Unwind();
    }
  }
  
  void init_types()
  {
    CoreTypes;
    CoreTypes.push_back("half");
    CoreTypes.push_back("float");
    CoreTypes.push_back("double");
    CoreTypes.push_back("x86_fp80");
    CoreTypes.push_back("fp128");
    CoreTypes.push_back("ppc_fp128");
  }