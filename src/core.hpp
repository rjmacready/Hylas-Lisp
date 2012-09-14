  string def_local(Form* form)
  {
    string varname = val(nth(form,1));
    string* result = lookup(varname);
    if(result != NULL)
    {
      printf("ERROR: Symbol already defined.");
      Unwind();
    }
    string out = emitCode(nth(form,2));
    out += allocate("%"+varname,latest_type());
    out += store(latest_type(),get_current_res(),"%"+varname);
    string str = latest_type();
    SymbolTable[ScopeDepth][varname] = str;
    return out;
  }
  
  string set(Form* form)
  {
    string varname = val(nth(form,1));
    string* tmp = lookup(varname);
    if(tmp == NULL)
    {
      error_unbound(nth(form,1));
    }
    string out = emitCode(nth(form,2));
    out += store(latest_type(),get_current_res(),"%"+varname);
    return out;
  }
  
  string ret(Form* form)
  {
    unsigned long l = length(form);
    if(l != 2)
    {
      printf("ERROR: (return) takes exactly one argument, but %li were provided.", l);
      Unwind();
    }
    string out = emitCode(nth(form,1));
    out += "ret " + latest_type() + " " + get_current_res();
    return out;
  }
  
  string generic_math(Form* form, string opcode)
  {
    string out = emitCode(nth(form,1));
    out += emitCode(nth(form,2));
    return out + get_unique_res(latest_type()) + " = " + opcode + " " + latest_type()
    + " " + get_res(tmp_version - 1) + ", " + get_res(tmp_version);
  }
  
  string add(Form* form)                {return generic_math(form,"add");}
  string fadd(Form* form)		{return generic_math(form,"fadd");}
  string sub(Form* form)                {return generic_math(form,"sub");}
  string fsub(Form* form)               {return generic_math(form,"fsub");}
  string mul(Form* form)		{return generic_math(form,"mul");}
  string fmul(Form* form)		{return generic_math(form,"fmul");}
  string udiv(Form* form)		{return generic_math(form,"udiv");}
  string sdiv(Form* form)		{return generic_math(form,"sdiv");}
  string fdiv(Form* form)		{return generic_math(form,"fdiv");}
  string urem(Form* form)		{return generic_math(form,"urem");}
  string srem(Form* form)		{return generic_math(form,"srem");}
  string frem(Form* form)		{return generic_math(form,"frem");}
  
  string icmp(Form* form)
  {
    string out;
    string cmp_code = val(nth(form,1));
    out += emitCode(nth(form,2));
    out += emitCode(nth(form,3));
    return out + get_unique_res(latest_type()) + " = icmp " + cmp_code + " "
    + latest_type() + " " + get_res(res_version-1) + ", " + get_res(res_version);
  }
  
  string fcmp(Form* form)
  {
    string out;
    string cmp_code = val(nth(form,1));
    out += emitCode(nth(form,2));
    out += emitCode(nth(form,3));
    return out + get_unique_res(latest_type()) + " = fcmp " + cmp_code + " "
    + latest_type() + " " + get_res(res_version-1) + ", " + get_res(res_version);
  }
  
  string access(Form* form)
  {
    string out = emitCode(nth(form,1));
    string type = latest_type();
    string type_cdr = string(type,1);
    string member = val(nth(form,2));
    string member_type;
    string member_loc;
    //Find in BasicTypes
    map<string,Type>::iterator seeker = BasicTypes.find(type_cdr);
    if(seeker != BasicTypes.end())
    {
      //Found matching type name, check if it's a structur
      if(seeker->second.id == typeStructure)
      {
        //Passed, check if it has that member
        map<string,pair<long,string> >::iterator checker = seeker->second.members.find(member);
        if(checker != seeker->second.members.end())
        {
          //Passed, record the member position
          member_loc = to_string<long>(checker->second.first);
          member_type = checker->second.second;  
        }
        else
	{
          printf("ERROR: Type '%s' does not have the member '%s'",type_cdr.c_str(),member.c_str());
	  Unwind();
	}
      }
      else
      {
        printf("ERROR: Type '%s' not a structure.",type_cdr.c_str());
	Unwind();
      }
    }
    //Find in Generics
    for(unsigned long i = 0; i < Generics.size(); i++)
    {
      map<string,map<string,pair<long,string> > >::iterator finder = Generics[i].second.specializations.find(type);
      if(finder != Generics[i].second.specializations.end())
      {
        map<string,pair<long,string> >::iterator checker = finder->second.find(member);
        if(checker != Generics[i].second.members.end())
        {
          //Passed, record the member position
          member_loc = to_string<long>(checker->second.first);
          member_type = checker->second.second;
        }
      }
      if(i == Generics.size()-1)
      {
        printf("ERROR: Improper type passed to (access).");
        Unwind();
      }
    }
    //Emit code
    out += get_unique_tmp() + " = getelementptr inbounds " + type + " " + get_current_res() + ", i32 0, i32" + member_loc + "\n";
    out += load(get_unique_res(member_type),member_type,get_current_tmp());
    return out;
  }
  
  string simple_if(Form* form)
  {
    string out = emitCode(nth(form,1));
    if(latest_type() != "i1")
    {
      out += "\nbr i1 true,";
    }
    else
      out += "\nbr i1 " + get_current_res() + ",";
    out += " label " + get_unique_label() + ", label " + get_unique_label();
    out += string(get_label(label_version - 1),1) + "\n";
    out += emitCode(nth(form,2));
    out += "br " + get_label(label_version+1) + "\n";
    out += string(get_label(label_version - 2),1) + "\n";
    out += emitCode(nth(form,3));
    out += get_unique_label();
    out += get_unique_res(latest_type()) + " = phi " + latest_type() + " [true, " + get_label(label_version - 2) + "], [false, " + get_label(label_version - 3) + "]";
    return out;
  }
  
  string flow(Form* form)
  {
    string out = emitCode(nth(form,1));
    if(latest_type() != "i1")
    {
      out += "br i1 true,";
    }
    else
    {
      out += "br i1 " + get_current_res() + ",";
    }
    out += " label " + get_unique_label() + ", label " + get_unique_label() + "\n";
    out += string(get_label(label_version - 1),1) + "\n";
    out += emitCode(nth(form,2));
    unsigned long save = res_version;
    out += "br " + get_label(label_version+1) + "\n";
    out += string(get_label(label_version - 2),1) + "\n";
    out += emitCode(nth(form,3));
    out += get_unique_label();
    out += get_unique_res(latest_type()) + " = phi " + latest_type() + " [" + get_res(save) + ", " + get_label(label_version - 1) + "], [" + get_res(res_version - 2) + ", " + get_label(label_version - 3) + "]";
    return out;
  }
  
  string begin(Form* form)
  {
    string out;
    for(unsigned int i = 1; i < length(form); i++)
      out += emitCode(nth(form,i));
    return out;
  }
  
  string main_fn(Form* form)
  {
    string out = "define i32 @main(i32 %argc, i8** %argv) nounwind";
    for(unsigned int i = 1; i < length(form); i++)
      out += emitCode(nth(form,i));
    return out + "\n}";
  }
  
  string declare(Form* form)
  {
    string out = "declare " + val(nth(form,2)) + " @" + val(nth(form,1)) + "(";
    for(unsigned int i = 3; i <= length(form)-1; i++)
    {
      out += val(nth(form,i)) + ", ";
    }
    return cutlast(out) + ")\n";		
  }
  
  string embed_llvm(Form* form)
  {
    return preprint(cdr(form));
  }
  
  string define_function(Form* form)
  {
    defineFunction(form,Function,noinline);
    string out = fn_code;
    fn_code = "";
    return out;
  }
  
  string define_recursive(Form* form)
  {
    defineFunction(form,Recursive,noinline);
    string out = fn_code;
    fn_code = "";
    return out;
  }
  
  string define_fast(Form* form)
  {
    defineFunction(form,Fast,noinline);
    string out = fn_code;
    fn_code = "";
    return out;
  }
  
  string define_inline(Form* form)
  {
    defineFunction(form,Function,doinline);
    string out = fn_code;
    fn_code = "";
    return out;
  }
  
  string define_inline_recursive(Form* form)
  {
    defineFunction(form,Recursive,doinline);
    string out = fn_code;
    fn_code = "";
    return out;
  }
  
  string define_inline_fast(Form* form)
  {
    defineFunction(form,Fast,doinline);
    string out = fn_code;
    fn_code = "";
    return out;
  }
  
  string direct_call(Form* form)
  {
    string out;
    string name = val(nth(form,1));
    string ret_type = val(nth(form,2));
    string c_conv = val(nth(form,3));
    map<long,string> inputs;
    for(unsigned long i = 0; i < length(form); i++)
    {
      out += emitCode(nth(form,i));
      inputs[res_version] = latest_type();
    }
    out += (string)"call " + ret_type + " " + c_conv + (string)" @" + name + (string)"(";
    for(map<long,string>::iterator seeker = inputs.begin(); seeker != inputs.end(); seeker++)
    {
      out += seeker->second + " " + get_res(seeker->first) + ",";
    }
    return cutlast(out) + ")";
  }
  
  string construct(Form* form)
  {
    //(construct [structure or generic] a_1 a_2 ... a_n)
    string out;
    string collection = "{";
    string type = printTypeSignature(nth(form,1));
    string type_cdr = string(type,1);
    long nargs = length(form)-2;
    map<long,string> inputs;
    unsigned long i;
    for(i = 2; i < length(form); i++)
    {
      out += emitCode(nth(form,i));
      inputs[res_version] = latest_type();
    }
    out += allocate(get_unique_tmp(),type);
    //Find in BasicTypes
    map<string,Type>::iterator seeker = BasicTypes.find(type_cdr);
    if(seeker != BasicTypes.end())
    {
      //Found matching type name, check if it's a structure
      if(seeker->second.id == typeStructure)
      {
        //Passed
        if(nargs == seeker->second.members.size())
        {
          //Iterate over members and compare it to inputs
          map<long,string>::iterator aide = inputs.begin();
          for(map<string,pair<long,string> >::iterator finder = seeker->second.members.begin();
              finder != seeker->second.members.end(); finder++)
          {
            //Check types
            if(finder->second.second == aide->second)
            {
              //Passed, write to out
              collection += aide->second + " " + to_string<long>(aide->first) + ",";
            }
            else
            {
              printf("ERROR: Type mismatch.");
              Unwind();
            }
            aide++;
          }
        }
        else
        {
          printf("ERROR: Wrong number of args.");
          Unwind();
        }
      }
      else
      {
        printf("ERROR: Type '%s' not a structure.",type_cdr.c_str());
        Unwind();
      }
    }
    //Find in Generics
    for(unsigned long i = 0; i < Generics.size(); i++)
    {
      if(Generics[i].second.id == typeStructure)
      {
        map<string,map<string,pair<long,string> > >::iterator finder = Generics[i].second.specializations.find(type);
        if(finder != Generics[i].second.specializations.end())
        {
          //Passed
          if(nargs == finder->second.size())
          {
            //Iterate over members and compare it to inputs
            //Check types
            map<long,string>::iterator aide = inputs.begin();
            for(map<string,pair<long,string> >::iterator looker = finder->second.begin();
              looker != finder->second.end(); looker++)
            {
              //Check types
              if(looker->second.second == aide->second)
              {
                //Passed, write to out
                collection += aide->second + " " + to_string<long>(aide->first) + ",";
              }
              else
              {
                printf("ERROR: Type mismatch.");
                Unwind();
              }
              aide++;
            }
            //Passed, write to out
          }
          else
          {
            printf("ERROR: Wrong number of args.");
            Unwind();
          }
        }
        if(i == Generics.size()-1)
        {
          printf("ERROR: Improper type passed to (construct).");
          Unwind();
        }
      }
    }
    //Emit code
    collection = cutlast(collection) + "}";
    out += store(type,collection,get_current_tmp());
    out += load(get_unique_res(type),type,get_current_tmp());
    return out;
  }
  
  void init_stdlib()
  {
    Scope new_scope;
    SymbolTable.push_back(new_scope);
    Core["def"]         = &def_local;
    Core["set"]         = &set;
    Core["ret"]      = &ret;
    Core["add"]         = &add;
    Core["fadd"]        = &fadd;
    Core["sub"]         = &fsub;
    Core["fsub"]        = &fsub;
    Core["mul"]         = &mul;
    Core["fmul"]        = &fmul;
    Core["udiv"]        = &udiv;
    Core["sdiv"]        = &sdiv;
    Core["fdiv"]        = &fdiv;
    Core["urem"]        = &urem;
    Core["srem"]        = &srem;
    Core["frem"]        = &frem;
    Core["icmp"]        = &icmp;
    Core["fcmp"]        = &fcmp;
    Core["acess"]       = &access;
    Core["begin"]       = &begin;
    Core["main"]        = &main_fn;
    Core["LLVM"]        = &embed_llvm;
    Core["if"]          = &simple_if;
    Core["flow"]        = &flow;
    //Function definitions
    Core["function"]    = &define_function;
    Core["recursive"]   = &define_recursive;
    Core["fast"]        = &define_fast;
    Core["inline"]      = &define_inline;
    Core["inline-recursive"] = &define_inline_recursive;
    Core["inline-fast"] = &define_inline_fast;
    Core["declare"]     = &declare;
    Core["call"]        = &direct_call;
    //Type stuff
    Core["construct"]   = &construct;
    //FFI
    //Word macros
    addWordMacro("bool","i1");
    addWordMacro("char","i8");
    addWordMacro("short","i16");
    addWordMacro("int","i32");
    addWordMacro("long","i64");
  }
