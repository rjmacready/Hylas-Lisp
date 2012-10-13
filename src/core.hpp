  typedef string (*hFuncPtr)(Form* code);
  
  map<string,hFuncPtr> TopLevel;
  map<string,hFuncPtr> Core;
  
  vector<string> allowedIntComparisons;
  vector<string> allowedFloatComparisons;
  
  string def(Form* form, bool global, bool typeonly)
  {
    cout << print(form) << endl;
    string varname = val(nth(form,1));
    Variable* tmp = lookup(varname);
    if(tmp != NULL)
      error(form,"Symbol already defined.");
    string type;
    string out;
    if(typeonly)
    {
      type = printTypeSignature(nth(form,2));
    }
    else if(!typeonly)
    {
      emitCode(nth(form,2));
      type = latest_type();
    }
    string fullname = (global?"@":"%")+varname;
    if(global && !typeonly)
      push(fullname + " = global " + type + " zeroinitializer");
    else if(!global && !typeonly)
      out += allocate(fullname,latest_type());
    if(!typeonly)
    {
      out += store(type,get_current_res(),fullname);
      master.SymbolTable[ScopeDepth][varname].type = type;
      master.SymbolTable[ScopeDepth][varname].constant = false;
      master.SymbolTable[ScopeDepth][varname].global = global;
      master.SymbolTable[ScopeDepth][varname].address = tmp_version;
      master.SymbolTable[ScopeDepth][varname].regtype = SymbolicRegister;
    }
    else
      out = constant(get_unique_res("i1"),"i1","true");
    return out;
  }
  
  string def_global(Form* form)
  { return def(form,true,false); }
  
  string def_local(Form* form)
  { return def(form,false,false); }
  
  string def_as_global(Form* form)
  { return def(form,true,true); }
  
  string def_as_local(Form* form)
  { return def(form,false,true); }
  
  string set(Form* form)
  {
    string varname = val(nth(form,1));
    Variable* tmp = lookup(varname);
    if(tmp == NULL)
      error_unbound(nth(form,1));
    string out = emitCode(nth(form,2));
    out += store(latest_type(),get_current_res(),
                 (tmp->global?"@":"%")+varname);
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
    unsigned long address = res_version;
    out += emitCode(nth(form,2));
    if(latest_type() != res_type(get_res(address)))
      error(form,"Basic mathematical operations require boths operands to be of\
 the same type. Here, the first operand was of type '",res_type(get_res(address)),\
"', while the second was of type '",latest_type(),"'.");       
    return out + get_unique_res(latest_type()) + " = " + opcode + " " + latest_type()
    + " " + get_res(address) + ", " + get_current_res();
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
    string cmp_code = val(nth(form,2));
    bool member;
    for(unsigned long i = 0; i < allowedIntComparisons.size(); i++)
    {
      if(allowedIntComparisons[i] == cmp_code)
      { member = true; break; }
    }
    if(!member)
      error(form,"The integer comparison code'",cmp_code,"' does not exist.");
    out += emitCode(nth(form,1));
    unsigned long address = res_version;
    out += emitCode(nth(form,3));
    if(latest_type() != res_type(get_res(address)))
      error(form,"Basic mathematical operations require boths operands to be of\
 the same type. Here, the first operand was of type '",res_type(get_res(address)),\
"', while the second was of type '",latest_type(),"'.");
    return out + get_unique_res("i1") + " = icmp " + cmp_code + " "
    + latest_type() + " " + get_res(address) + ", " + get_current_res();
  }
  
  string fcmp(Form* form)
  {
    string out;
    string cmp_code = val(nth(form,2));
    bool member;
    for(unsigned long i = 0; i < allowedFloatComparisons.size(); i++)
    {
      if(allowedFloatComparisons[i] == cmp_code)
      { member = true; break; }
    }
    if(!member)
      error(form,"The integer comparison code'",cmp_code,"' does not exist.");
    out += emitCode(nth(form,1));
    unsigned long address = res_version;
    out += emitCode(nth(form,3));
    if(latest_type() != res_type(get_res(address)))
      error(form,"Basic mathematical operations require boths operands to be of\
 the same type. Here, the first operand was of type '",res_type(get_res(address)),\
"', while the second was of type '",latest_type(),"'.");
    return out + get_unique_res("i1") + " = fcmp " + cmp_code + " "
    + latest_type() + " " + get_res(address) + ", " + get_current_res();
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
        map<string,pair<unsigned long,string> >::iterator checker = seeker->second.members.find(member);
        if(checker != seeker->second.members.end())
        {
          //Passed, record the member position
          member_loc = to_string<unsigned long>(checker->second.first);
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
      map<string,map<string,pair<unsigned long,string> > >::iterator finder = Generics[i].second.specializations.find(type);
      if(finder != Generics[i].second.specializations.end())
      {
        map<string,pair<unsigned long,string> >::iterator checker = finder->second.find(member);
        if(checker != Generics[i].second.members.end())
        {
          //Passed, record the member position
          member_loc = to_string<unsigned long>(checker->second.first);
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
    out += load(get_unique_res_address(member_type,tmp_version),member_type,get_current_tmp());
    return out;
  }
  
  string simple_if(Form* form)
  {
    string out = emitCode(nth(form,1));
    unsigned long cond_address = res_version;
    if(latest_type() != "i1")
      error(form,"The test (Second argument) of an (if) form must evaluate to a boolean (i1) value.");
    else
      out += "br i1 " + get_current_res() + ",";
    out += " label " + get_unique_label();
    out += ", label " + get_unique_label() + "\n";
    out += functional_label(get_label(label_version-1));
    out += emitCode(nth(form,2));
    out += "br label " + get_label(label_version+1) + "\n";
    out += functional_label(get_current_label());
    out += emitCode(nth(form,3));
    out += "br label " + get_label(label_version+1) + "\n";
    out += functional_label(get_unique_label());
    out += get_unique_res("i1") + " = phi i1 [true," + get_label(label_version-2)
        + "],[false," + get_label(label_version-1) + "]";    
    return out;
  }
  
  string flow(Form* form)
  {
    string out = emitCode(nth(form,1));
    string true_branch_type;
    unsigned long cond_address, true_address, false_address;
    cond_address = res_version;
    if(latest_type() != "i1")
      error(form,"The test (Second argument) of a (flow) form must evaluate to a boolean (i1) value.");
    else
      out += "br i1 " + get_current_res() + ",";
    out += " label " + get_unique_label() + ", label ";
    out += get_unique_label() + "\n";
    out += functional_label(get_label(label_version-1));
    out += emitCode(nth(form,2));
    true_branch_type = latest_type();
    true_address = res_version;
    //out += store(true_branch_type,get_current_res(),address);
    out += "br label " + get_unique_label() + "\n";
    out += functional_label(get_label(label_version-1));
    out += emitCode(nth(form,3));
    false_address = res_version;
    //out += store(true_branch_type,get_current_res(),address);
    out += "br label " + get_label(label_version) + "\n";
    out += functional_label(get_label(label_version));
    if(true_branch_type != latest_type())
      error(form,"Both branches of a (flow) statement must evaluate to the same type. Here, the true branch returns a '",
            true_branch_type,"', while the false branch returns a '",latest_type(),"'.");
    else
      out += get_unique_res(true_branch_type) + " = phi " + true_branch_type + " ["
          + get_res(true_address) + "," + get_label(label_version-2) + "],["
          + get_res(false_address) + "," + get_label(label_version-1) + "]";
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
  
  string foreign(Form* form)
  {
    string name = val(nth(form,1));
    string ret_type = printTypeSignature((nth(form,2)));
    string args = "(";
    string call_args;
    string out = "declare " + ret_type + " @" + name + "(";
    for(unsigned long i = 3; i <= length(form)-1; i++)
    {
      out += printTypeSignature(nth(form,i)) + ",";
      args += "(arg." + to_string(i) + " " + print(nth(form,i)) + ")";
      call_args += " arg." + to_string(i);
    }
    out = cutlast(out) + ")\n";
    push(out);
    args += (string)")";
    out =
      "(inline " + name + " " + ret_type + " " + args
      + "(call " + name + " " + ret_type + " ccc " + call_args + "))";
    cout << out << endl;
    Form* code = readString(out);
    return emitCode(code);
  }
  
  string embed_llvm(Form* form)
  {
    string out = val(nth(form,1));
    return cutfirst(cutlast(out));
  }
  
  string define_function(Form* form)
  { return defineFunction(form,Function,noinline); }
  
  string define_recursive(Form* form)
  { return defineFunction(form,Recursive,noinline); }
  
  string define_fast(Form* form)
  { return defineFunction(form,Fast,noinline); }
  
  string define_inline(Form* form)
  { return defineFunction(form,Function,doinline); }
  
  string define_inline_recursive(Form* form)
  { return defineFunction(form,Recursive,doinline); }
  
  string define_inline_fast(Form* form)
  { return defineFunction(form,Fast,doinline); }
  
  string construct(Form* form)
  {
    //(construct [structure or generic] a_1 a_2 ... a_n)
    string out;
    string type = printTypeSignature(nth(form,1));
    string type_cdr = string(type,1);
    unsigned long nargs = length(form)-2;
    map<unsigned long,string> inputs;
    unsigned long i;
    for(i = 2; i < length(form); i++)
    {
      out += emitCode(nth(form,i));
      inputs[res_version] = latest_type();
    }
    out += allocate(get_unique_tmp(),type);
    unsigned long allocation_point = tmp_version;
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
          map<unsigned long,string>::iterator aide = inputs.begin();
          for(map<string,pair<unsigned long,string> >::iterator finder = seeker->second.members.begin();
              finder != seeker->second.members.end(); finder++)
          {
            //Check types
            unsigned long pos = 0;
            if(finder->second.second == aide->second)
            {
              //Passed, write to out
              out += get_unique_tmp() + " = getelementptr inbounds " + type + "* " + get_tmp(allocation_point)
                  + ", i32 0, i32 " + to_string<unsigned long>(pos) + "\n";
              out += store(aide->second,get_res(aide->first),get_current_tmp());
              pos++;
            }
            else
              error(form,"Wrong type: ",finder->second.second," does not match ",
                      aide->second,".");
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
        //Found a structure
        map<string,map<string,pair<unsigned long,string> > >::iterator finder = Generics[i].second.specializations.find(type);
        if(finder != Generics[i].second.specializations.end())
        {
          //Found the type in the Generic's specializations
          if(nargs == finder->second.size())
          {
            //Iterate over members and compare it to inputs
            //Check types
            map<unsigned long,string>::iterator aide = inputs.begin();
            for(map<string,pair<unsigned long,string> >::iterator looker = finder->second.begin();
              looker != finder->second.end(); looker++)
            {
              //Check types
              unsigned long pos = 0;
              if(looker->second.second == aide->second)
              {
                //Passed, write to out
                out += get_unique_tmp() + " = getelementptr inbounds " + type + "* " + get_tmp(allocation_point)
                    + ", i32 0, i32 " + to_string<unsigned long>(pos) + "\n";
                out += store(aide->second,get_res(aide->first),get_current_tmp());
                pos++;
              }
              else
              {
                error(form,"Wrong type: ",looker->second.second," does not match ",
                      aide->second,".");
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
    out += load(get_unique_res(type),type,get_tmp(allocation_point));
    return out;
  }
  
  string make_array(Form* form, bool global)
  {
    string out;
    string type;
    vector<unsigned long> inputs;
    unsigned long i = 1;
    for(; i < length(form); i++)
    {
      out += emitCode(nth(form,i));
      if(!type.empty())
        if(latest_type() != type)
          error(form,"When building an array, the types of all the elements must match.",
                " The first type mismatch occured at '",nth(form,i),"'.");
      inputs.push_back(res_version);
      type = latest_type();
    }
    string array_type;
    array_type = "[" + to_string(length(form)-1) + " x " + type + "]";
    string address = (string)(global ? "@" : "%") + "array" + to_string(++array_version);
    unsigned long address_pos;
    if(global)
      push(address + " = global " + array_type + " zeroinitializer");
    else
    {
      out += allocate(address,array_type);
      address_pos = tmp_version;
    }
    for(i = 0; i < inputs.size(); i++)
    {
      out += get_unique_tmp() + " = getelementptr inbounds " + array_type + "* " + address + ", i64 0, i64 " + to_string<unsigned long>(i) + "\n";
      out += store(type,get_res(inputs[i]),get_current_tmp());
    }
    out += get_unique_res(type + "*") + " = getelementptr inbounds " + array_type + "* " + address
        + ", i64 0, i64 0";
    return out;
  }
  
  string global_array(Form* in)
  {
    return make_array(in,true);
  }
  
  string local_array(Form* in)
  {
    return make_array(in,false);
  }
  
  string nth_array(Form* in)
  {
    string out;
    //Code for address
    out += emitCode(nth(in,1));
    unsigned long address_location = res_version;
    //Code for array
    out += emitCode(nth(in,2));
    out += get_unique_tmp() + " = getelementptr inbounds " + latest_type() + "* " + get_current_res()
        + ", i64 " + get_res(address_location);
    out += load(get_unique_res_address(cutlast(latest_type()),tmp_version),latest_type(),get_current_tmp());
    return out;
  }
  
  string mem_allocate(Form* in)
  {
    //%x = allocate [type]
    string type = printTypeSignature(nth(in,1));
    string out = allocate(get_unique_tmp(),type);
    out += constant(get_unique_res(type+"*"),type,get_current_tmp());
    return out;
  }
  
  string mem_store(Form* in)
  {
    //(store [value] [address])
    //%x = store [type] [value], [type]* [address]
    string out;
    //emit value
    out += emitCode(nth(in,1));
    string type = latest_type();
    unsigned long version = res_version;
    //emit address
    out += emitCode(nth(in,2));
    if(cutlast(latest_type()) != type)
      error(in,"The type of the value (",type,") does not equal the type of the address (",latest_type(),").");
    out += store(type,get_res(version),get_current_res());
    out += constant(get_unique_res(type),type,get_res(version));
    return out;
  }
  
  string mem_load(Form* in)
  {
    //%x = load [type] [address]; Don't add an asterisk to the type!
    //The type of the address must already be a pointer!
    //emit address
    string out = emitCode(nth(in,1));
    string type = latest_type();
    if(type[type.length()-1] != '*')
      error(in,"Address is not a pointer.");
    out += load(get_unique_res(cutlast(type)),type,get_current_res());
    return out;
  }
  
  string address(Form* in)
  {
    if(length(in) != 2)
      error(in,"(address) takes a single argument");
    string out = emitCode(nth(in,1));
    Variable* latest = lookup(cutfirst(get_current_res()));
    //Is the input an LValue? That is, does it have an address?
    if(latest->regtype == LValue)
      out += get_unique_res(latest_type()+"*") + " = select i1 true, " + latest_type() + " " + get_tmp(latest->address) + ", " + latest_type() + " " + get_tmp(latest->address);
    else
      error(in,"The input to (address) must be an lvalue (A symbol or the result of (access) or (nth))");
    return out;
  }
  
  string toplevel_asm(Form* in)
  {
    if(cdr(in) == NULL)
      error(in,"No Assembly code provided.");
    return (string)"module asm \"" + cutfirst(cutlast(val(cadr(in)))) + "\"\n";
  }
  
  string inline_asm(Form* in)
  {
    string out;
    string type = printTypeSignature(nth(in,1));
    string dialect = val(nth(in,2));
    string code = print(cdr(cdr(cdr(in))));
    if(dialect != "Intel" && dialect != "ATT")
      error(in,"Unknown assembly dialect: Only 'Intel' and 'ATT' are supported, '"
      ,dialect,"' was given.");
    out += get_unique_res(type) + " = call " + type + " asm "
        + ((dialect == "Intel" ? "inteldailect" : "")) + " \"" + code + "\", ""()";
    return out;
  }
  
  string word(Form* in)
  {
    if(length(in) < 3)
      error(in,"(word) takes at least three arguments: An atom to be replaced and code to replace it with.");
    else if(islist(nth(in,1)))
      error(in,"The first argument to (word) must be a symbolic atom.");
    else if(analyze(val(nth(in,1))) != Symbol)
      error(in,"The first argument to (word) must be a symbolic atom.");
    string word = val(nth(in,1));
    if(islist(nth(in,2)))
      error(in,"The second argument to (word) must be a string.");
    else if(analyze(val(nth(in,2))) != String)
      error(in,"The second argument to (word) must be a string.");
    map<string,string>::iterator checker = WordMacros.find(word);
    if(checker != WordMacros.end())
    {
      if(master.allow_RedefineWordMacros)
        warn(in,"Redefining word macro '",word,"'.");
      else
        error(in,"A word macro with that name ('",word,"') has already been defined.");
    }
    WordMacros[word] = cutboth(val(nth(in,2)));
    return "";
  }
    
  string fixes(Form* in, bool pre)
  {
    if(length(in) < 3)
      error(in,(pre?"(prefix)":"(postfix)")," takes at least three arguments: An atom to be replaced and code to replace it with.");
    else if(islist(nth(in,1)))
      error(in,"The first argument to ",(pre?"(prefix)":"(postfix)")," must be a symbolic atom.");
    else if(analyze(val(nth(in,1))) != Symbol)
      error(in,"The first argument to ",(pre?"(prefix)":"(postfix)")," must be a symbolic atom.");
    else if(val(nth(in,1)).length() != 1)
      error(in,"The first argument to ",(pre?"(prefix)":"(postfix)")," must be a single character long.");
    char name = val(nth(in,1))[0];
    if(islist(nth(in,2)))
      error(in,"The second argument to ",(pre?"(prefix)":"(postfix)")," must be a string.");
    else if(analyze(val(nth(in,2))) != String)
      error(in,"The second argument to ",(pre?"(prefix)":"(postfix)")," must be a string.");
    string code = cutboth(val(nth(in,2)));
    map<char,string>::iterator checker = (pre?Prefixes:Postfixes).find(name);
    if(checker != (pre?Prefixes:Postfixes).end())
    {
      if(master.allow_RedefinePrePostfixes)
        warn(in,"Redefining ",(pre?"prefix":"postfix")," '",name,"'.");
      else
        error(in,"A ",(pre?"prefix":"postfix")," with that name ('",name,"' has already been defined.");
    }
    if(pre)
      Prefixes[name] = code;
    else
      Postfixes[name] = code;
    return "";
  }
  
  string prefix(Form* in)
  { return fixes(in,true); }
  
  string postfix(Form* in)
  { return fixes(in,false); }
  
  string import(Form* in)
  {
    string filepath = cutfirst(cutlast(print(cdr(in))));
    if(filepath.find(".hylas") == string::npos)
      filepath = filepath + ".hylas";
    return "";
  }
  
  void init_stdlib()
  {
    Scope new_scope;
    master.SymbolTable.push_back(new_scope);
    //Init Core
    TopLevel["main"]         = &main_fn;
    TopLevel["LLVM"]         = &embed_llvm;
    TopLevel["def"]          = &def_global;
    TopLevel["def-as"]       = &def_as_global;
    TopLevel["type"]         = &makeType;
    TopLevel["structure"]    = &makeStructure;
    TopLevel["generic"]      = &genericInterface;
    TopLevel["asm"]          = &toplevel_asm;
    TopLevel["word"]         = &word;
    TopLevel["prefix"]       = &prefix;
    TopLevel["postfix"]      = &postfix;
    Core["function"]         = &define_function;
    Core["recursive"]        = &define_recursive;
    Core["fast"]             = &define_fast;
    Core["inline"]           = &define_inline;
    Core["inline-recursive"] = &define_inline_recursive;
    Core["inline-fast"]      = &define_inline_fast;
    Core["foreign"]          = &foreign;
    Core["def"]              = &def_local;
    Core["def-as"]           = &def_as_local;
    Core["set"]              = &set;
    Core["ret"]              = &ret;
    Core["add"]              = &add;
    Core["fadd"]             = &fadd;
    Core["sub"]              = &sub;
    Core["fsub"]             = &fsub;
    Core["mul"]              = &mul;
    Core["fmul"]             = &fmul;
    Core["udiv"]             = &udiv;
    Core["sdiv"]             = &sdiv;
    Core["fdiv"]             = &fdiv;
    Core["urem"]             = &urem;
    Core["srem"]             = &srem;
    Core["frem"]             = &frem;
    Core["icmp"]             = &icmp;
    Core["fcmp"]             = &fcmp;
    Core["begin"]            = &begin;
    Core["if"]               = &simple_if;
    Core["flow"]             = &flow;
    Core["construct"]        = &construct;
    Core["access"]           = &access;
    Core["array"]            = &local_array;
    Core["nth"]              = &nth_array;
    Core["allocate"]         = &mem_allocate;
    Core["store"]            = &mem_store;
    Core["load"]             = &mem_load;
    Core["address"]          = &address;
    Core["asm"]              = &inline_asm;
    Core["import"]           = &import;
    //Word macros
    addWordMacro("bool","i1");
    addWordMacro("char","i8");
    addWordMacro("byte","i8");
    addWordMacro("octet","i8");
    addWordMacro("short","i16");
    addWordMacro("int","i32");
    addWordMacro("long","i64");
    addWordMacro("octet","i8");
    addWordMacro("eq","seq");
    addWordMacro("<","slt");
    addWordMacro("u<","ult");
    //Allowed comparisons for numerical operations
    allowedIntComparisons = {"eq", "ne", "ugt", "uge", "ult", "ule", "sgt"
                             "sge", "slt", "sle"};
    allowedFloatComparisons = {"false", "oeq", "ogt", "oge", "olt", "ole", "one",
                               "ord", "ueq", "ugt", "uge", "ult", "ule", "une",
                               "uno", "true"};
  }