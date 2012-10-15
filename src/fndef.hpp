  #define noinline false
  #define doinline true
  
  struct Lambda
  {
    string name;
    string ret_type;
    string fn_ptr_type;
    unsigned long nargs;
    map<string,string> arguments;
    bool fastcc;
    bool tco;
    bool lining;
    string docstring;
  };
  
  struct MetaFunction
  {
    vector<Lambda> versions;
  };
  
  map<string,MetaFunction> FunctionTable;
  
  /*
   * - Iterate over the arguments, noting type-annotated and polymorphic arguments
   * - Iterate over the code, storing all expressions in a list 'precode'
   * - Find a typecase, ensure there is no code after it
   * - Ensure the first argument to typecase is an argument declared as being polymorphic
   * - Iterate over every branch of the typecase, ensuring the first element of each list is a valid type name
   *  - Call defineFunction with the same function name, where the code is the precode plus the rest of the code in this branch (This should recursively apply nested typecases).*/
  
  void print(map<string,string> in)
  {
    printf("{\n");
    for(map<string,string>::iterator i = in.begin(); i != in.end(); i++)
    {
      printf("\t%s : %s\n", i->first.c_str(), i->second.c_str());
    }
    printf("}\n");
  }
  
  void print_metafunction(string in)
  {
    for(unsigned int i = 0; i < FunctionTable[in].versions.size(); i++)
    {
      printf("\t\t{\n");
      printf("\t\t\t Return type: %s\n",FunctionTable[in].versions[i].ret_type.c_str());
      printf("\t\t\t Number of Arguments: %lu\n",FunctionTable[in].versions[i].nargs);
      printf("\t\t\t Tail Call Optimized?: %s\n",FunctionTable[in].versions[i].tco ? "Yes" : "No");
      printf("\t\t}\n");
    }
  }
  
  void print_fntable()
  {
    printf("{\n");
    for(map<string,MetaFunction>::iterator i = FunctionTable.begin(); i != FunctionTable.end(); i++)
    {
      printf("\t%s: \n",i->first.c_str());
      print_metafunction(i->first);
    }
    printf("}\n");
  }
  
  enum fnType {Function, Fast, Recursive};
  
  const unsigned int name_pos = 1;
  const unsigned int ret_type_pos = 2;
  const unsigned int args_pos = 3;
  const unsigned int docstring_pos = 4;
  
  void validate_function(Form* form)
  {
    if(length(form) < 2)
      error(form,"Can't define a function with no name.");
    else if(length(form) < 3)
      error(form,"Can't define a function with no return type.");
    else if(length(form) < 4)
      error(form,"Can't define a function with no argument list.");
    if(length(form) < 5)
      error(form,"Can't define a function with no body (The block of code to execute).");
    if(tag(nth(form,name_pos)) != Atom)
      error(form,"Can't use a list as a function name.");
    else if(analyze(val(nth(form,name_pos))) != Symbol)
      error(form,"form,Can't use a non-symbolic atom as a function name.");
    if(nth(form,args_pos) != NULL)
      if(tag(nth(form,args_pos)) != List)
        error(form,"The argument list must be a list.");
  }
  
  string removeReturn(string in)
  { return string(in,in.find("(")); }
  
  string defineFunction(Form* form, fnType fn_type, bool inlining)
  {
    //0                                                1      2          3             [3]         3/4 (Depending on the docstring)
    //(function|fast|recursive|inline|recursive-inline [name] [ret_type] (arg1, arg1, ..., argn) "docstring" [body]*
    const bool has_docstring = /*((tag(nth(form,docstring_pos)) == Atom) && (analyze(val(nth(form,docstring_pos))) == String))*/ false;
    const unsigned int body_starting_pos = (has_docstring ? 1+docstring_pos : docstring_pos); 
    validate_function(form);
    Lambda newfn;
    if(fn_type == Function) { newfn.fastcc = false; newfn.tco = false; newfn.lining = inlining; }
    else if(fn_type == Fast) { newfn.fastcc = true; newfn.tco = false; newfn.lining = inlining; }
    else if(fn_type == Recursive) { newfn.fastcc = true; newfn.tco = true; newfn.lining = inlining; }
    string fn_name = val(nth(form,name_pos));
    newfn.ret_type = printTypeSignature(nth(form,ret_type_pos));
    /*map<string,Lambda>::iterator macfind = Macros.find(fn_name);
    if(macfind != Macros.end())
      { printf("ERROR: A macro has already been defined with that name."); Unwind(); }*/
    Scope new_scope;
    master.SymbolTable.push_back(new_scope);
    //Iterate over arguments
    map<string,string> fn_args;
    newfn.fn_ptr_type = newfn.ret_type + "(";
    Form* current_arg;
    if(nth(form,args_pos) != NULL)
    {
      for(unsigned long i = 0; i < length(nth(form,args_pos)); i++)
      {
        current_arg = nth(nth(form,args_pos),i);
        if(tag(current_arg) == List)
        {
          if(tag(nth(current_arg,0)) != Atom)
          {
            printf("ERROR: An argument name must be a symbol, not a list or any other kind of atom.");
            Unwind();
          }
          else
          {
            string argname = val(nth(current_arg,0));
            map<string,string>::iterator seeker = fn_args.find(argname);
            if(seeker != fn_args.end())
            {
              printf("ERROR: Argument name '%s' was already defined in this argument list:\n%s",
                     argname.c_str(),print(nth(form,2)).c_str());
            }
            else
            {
              string type = printTypeSignature(nth(current_arg,1));
              fn_args[argname] = type;
              master.SymbolTable[ScopeDepth][argname].type = type;
              master.SymbolTable[ScopeDepth][argname].constant = false;
              master.SymbolTable[ScopeDepth][argname].global = false;
              master.SymbolTable[ScopeDepth][argname].regtype = LValue;
              newfn.fn_ptr_type += type + ",";
            }
          }
        }
        else
        { 
          if(val(current_arg) == "...")
          {
            fn_args[gensym()] = "...";
            newfn.fn_ptr_type += "...,";
          }
        }
      }
    }
    newfn.arguments = fn_args;
    newfn.nargs = fn_args.size();
    newfn.fn_ptr_type = ((nth(form,args_pos) == NULL)?newfn.fn_ptr_type:cutlast(newfn.fn_ptr_type)) + ")*";
    string rem = removeReturn(newfn.fn_ptr_type);
    string tmp_code;
    map<string,MetaFunction>::iterator seeker = FunctionTable.find(fn_name);
    if(seeker != FunctionTable.end())
    {
      for(unsigned int i = 0; i < seeker->second.versions.size(); i++)
      {
        if(removeReturn(seeker->second.versions[i].fn_ptr_type) == rem) //Compare prototypes without comparing return types
          error(form,"A function with the same prototype (",cutlast(rem),") has already been defined.");
      }
      newfn.name = fn_name + to_string(seeker->second.versions.size()-1);
      seeker->second.versions.push_back(newfn);
    }
    else
    {
      newfn.name = fn_name + "0";
      MetaFunction new_metafn;
      new_metafn.versions.push_back(newfn);
      FunctionTable[fn_name] = new_metafn;
    }
    string arg_name, base_name, arg_code, out;
    for(map<string,string>::reverse_iterator i = fn_args.rbegin(); i != fn_args.rend(); i++)
    {
      if(i->second == "...")
      {
        arg_code += "...,";
        //TODO: Check that it's the last one on the list
      }
      else
      {
        arg_name = "%" + i->first+to_string(ScopeDepth);
        base_name = arg_name + "_base";
        arg_code += i->second + " " + base_name + ",";
        tmp_code += allocate(arg_name,i->second);
        tmp_code += store(i->second,base_name,arg_name);
      }
    }
    string processed_name = "@" + fn_name + ((seeker != FunctionTable.end()) ? to_string(seeker->second.versions.size()-1) : "0");
    tmp_code = (string)"define " + (newfn.fastcc? "fastcc ":"ccc ") + newfn.ret_type + " " + processed_name
        + "(" + cutlast(arg_code) + ")\n{\n" + tmp_code;
    //Compile the code
    for(unsigned long i = body_starting_pos; i < length(form);i++)
      tmp_code += emitCode(nth(form,i));
    push(tmp_code + "ret " + newfn.ret_type + " " + get_current_res() + "\n}");
    out += constant(get_unique_res(newfn.fn_ptr_type),newfn.fn_ptr_type,processed_name);
    return out;
  }
  
  string callGeneric(long gen_pos, Form* code)
  {
    //Not implemented yet
    return print(code) + to_string(gen_pos);
  }
  bool isFunctionPointer(string in)
  {
    return ((in.find('(') != string::npos) &&
            (in.find(')') != string::npos) &&
            (in.find('(') < in.find(')')) &&
            (in[in.length()-1] == '*'));
    //TODO some other tests are needed
  }
  
  string cleanPointer(string in)
  {
    //i32(i8*,i64)* -> (i8*,i64)
    string arglist = cutlast(string(in,in.find('(')));
    return arglist;
  }
  
  string callFunction(Form* in)
  {
    //First of all: What does the function look like? Get a "partial function pointer": put the argument types into a list
    string pointer, out;
    string arguments = "(";
    if(cdr(in) == NULL)
      pointer = "()";
    else
    {
      pointer = "(";
      for(unsigned long i = 1; i < length(in); i++)
      {
        out += emitCode(nth(in,i));
        pointer += latest_type() + ",";
        arguments += latest_type() + " " + get_current_res() + ",";
      }
      pointer = cutlast(pointer) + ")";
      arguments = cutlast(arguments) + ")";
    }
    if(isatom(car(in)))
    {
      //Calling a function by name
      //First, find the name in the table
      string name = val(car(in));
      map<string,MetaFunction>::iterator seeker = FunctionTable.find(name);
      if(seeker != FunctionTable.end())
      {
        //Name found, now we compare our pointer to each version's pointer sans the return type
        for(unsigned long i = 0; i < seeker->second.versions.size(); i++)
        {
          cout << "Pointer: " << pointer << endl;
          string target = cleanPointer(seeker->second.versions[i].fn_ptr_type);
          cout << "Pointer compared: " << target << endl;
          if((pointer == target) ||
             (string(pointer,0,target.find("...")) == string(target,0,target.find("..."))))
          {
            string ret_type = seeker->second.versions[i].ret_type;
            //Found our match, emit code to call the function
            out += get_unique_res(seeker->second.versions[i].ret_type) + " = " + (seeker->second.versions[i].tco ? "tail call " : "call ");
            out += (seeker->second.versions[i].fastcc ? "fastcc " : "ccc ");
            out += ret_type + " ";
            out += pointer + "* @"
                + seeker->second.versions[i].name;
            out += arguments;
            return out;
          }
        }
      }
      else
      {
        //Name not found in the function table, now let's try variables
        for(long i = ScopeDepth; i != -1; i--)
        {
          map<string,Variable>::iterator seeker = master.SymbolTable[i].find(name);
          if(seeker != master.SymbolTable[i].end())
          {
            if(isFunctionPointer(seeker->second.type))
            {
              string target = cleanPointer(seeker->second.type);
              if((pointer == target) ||
                 (string(pointer,0,target.find("...")) == string(target,0,target.find("..."))))
              {
                //Found our match, emit code to call the pointer
                string ret_type = string(seeker->second.type,0,seeker->second.type.find('('));
                out += get_unique_res(ret_type) + " = call " + ret_type + " " + pointer + arguments;
                //We don't provide the calling convention
                return out;
              }
            }
          }
        }
      }
      error(in,"No function (Or variable with a matching function pointer type) matches the name '",name,"' and the protype ",pointer,".");
    }
    else if(islist(car(in)))
    {
      //Emit code for the for the form. Is it a function pointer?
      out += emitCode(nth(in,0));
      if(isFunctionPointer(latest_type()))
      {
        string target = cleanPointer(latest_type());
        if((pointer == target) ||
           (string(pointer,0,target.find("...")) == string(target,0,target.find("..."))))
        {
          string ret_type = string(latest_type(),0,latest_type().find('('));
          out += get_unique_res(ret_type) + " = call " + ret_type + " " + pointer + "* " + get_current_res() + arguments;
          return out;
        }
      }
      else
        error(in,"Tried to compile the first element of the form '",in,"' to see if it was a callable function pointer, but an object of type '",latest_type(),"' was found.");
    }
    return out;
  }