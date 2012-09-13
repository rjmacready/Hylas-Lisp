  struct Type;
  struct Generic;
  bool isInteger(string in);
  bool isCoreType(string in);
  bool isArgument(Form* in, map<string,Form*> arguments);
  Form* editForm(Form* in, map<string,Form*> replacements);
  string specializeType(Generic* in, map<string,Form*> replacements, string signature);
  string printTypeSignature(Form* form);
  bool checkTypeExistence(string name);
  string makeType(Form* in);
  string makeStructure(Form* in);
  bool checkGenericExistence(string name, bool id);
  Generic writeGeneric(Form* in, bool type);
  void addGeneric(string name, Generic in);
  Generic addGenericMethod(string name, Form* in);
  Generic makeGeneric(Form* in);
  
  #define typeSimple    0
  #define typeStructure 1
  #define typeFunction 2
  #define typeMethod    3
  
  
  struct BaseType
  {
    unsigned char id;
  };
  
  struct Type : BaseType
  {
    map<string,pair<int,string> > members;
  };
  
  struct Generic : Type
  {
    vector<string> arguments;
    Form* code;
    map<string,map<string, pair<int,string> > > specializations;
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
  
  string specializeType(Generic* in, map<string,Form*> replacements, string signature)
  {
    Form* specialization_code = editForm(in->code, replacements);
    string out = signature + " = type { ";
    string type;
    for(unsigned long i = 0; i < length(cdr(cdr(specialization_code))); i++)
    {
      type = printTypeSignature(cadr(nth(cdr(cdr(specialization_code)),i)));
      in->specializations[signature][val(car(nth(cdr(cdr(specialization_code)),i)))] = pair<int,string>(i,type);
      out += type + ",";
    }
    out = cutlast(out)+" }\n";
    printf("Specializing methods!\n");
    //Specialize methods
    for(unsigned long i = 0; i < in->methods.size(); i++)
    {
      printf("Specializing method %lu!\n",i);
      printf("Method code:\n%s\n",preprint(editForm(in->methods[i],replacements)).c_str());
      out += "\n" + emitCode(editForm(in->methods[i],replacements));
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
            for(map<string,map<string, pair<int,string> > >::iterator seeker = Generics[i].second.specializations.begin();
                seeker != Generics[i].second.specializations.end(); seeker++)
            { 
              if(seeker->first == signature)
                return signature;
            }
            map<string,Form*> replacements;
            for(j = 0; j < Generics[i].second.arguments.size(); j++)
            {
              replacements[Generics[i].second.arguments[j]] = nth(form,j+1);
            }
            for(j = 0; j < signature.length(); j++)
            {
              if(signature[j] == '*')
              {
                signature.replace(j,3,"ptr");
              }
            }
            push(specializeType(&(Generics[i].second),replacements,signature));
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
    BasicTypes[name] = tmp;
    string type = printTypeSignature(nth(in,2));
    return "%"+ name + " = type " + type;
  }
  
  void validateStructure(Form* in)
  {
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
  }
  
  string makeStructure(Form* in)
  {
    Type tmp;
    tmp.id = typeStructure;
    validateStructure(in);
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
          tmp.members[field_name] = pair<int,string>(i-2,printTypeSignature(cadr(current_field)));
        }
      }
      else
      {
        printf("ERROR: Structure fields must be lists, not atoms.");
        Unwind();
      }
    }
    string out = "%" + name + " = type {";
    for(map<string,pair<int,string> >::iterator seeker = tmp.members.begin(); seeker != tmp.members.end(); seeker++)
    {
      out += seeker->second.second + ", ";
    }
    out = cutlast(cutlast(out));
    out += "}";
    BasicTypes[name] = tmp;
    return out;
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
	  validateStructure(in);
      string name = val(nth(in,2));
      Form* code = readString("(structure " + name + ")");
      code = append(code, cdr(cdr(cdr(cdr(in)))));
      out.code = code;
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
          }
        }
        else
        {
          printf("ERROR: Structure fields must be lists, not atoms.");
          Unwind();
        }
	  }
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