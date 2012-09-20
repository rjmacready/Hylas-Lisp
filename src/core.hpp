  typedef string (*hFuncPtr)(Form* code);
  
  map<string,hFuncPtr> TopLevel;
  map<string,hFuncPtr> Core;
  
  vector<string> allowedIntComparisons;
  vector<string> allowedFloatComparisons;
  
  string def_local(Form* form)
  {
    string varname = val(nth(form,1));
    Variable* result = lookup(varname);
    if(result != NULL)
    {
      printf("ERROR: Symbol already defined.");
      Unwind();
    }
    string out = emitCode(nth(form,2));
    string type = latest_type();
    string fullname = "%"+varname;
    out += allocate(fullname,latest_type());
    out += store(type,get_current_res(),fullname);
    SymbolTable[ScopeDepth][varname].sym = type;
    SymbolTable[ScopeDepth][varname].constant = false;
    SymbolTable[ScopeDepth][varname].global = false;
    return out;
  }
  
  string def_global(Form* form)
  {
    
  }
  
  string set(Form* form)
  {
    string varname = val(nth(form,1));
    Variable* tmp = lookup(varname);
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
    string cmp_code = val(nth(form,1));
    bool member;
    for(unsigned long i = 0; i < allowedIntComparisons.size(); i++)
    {
      if(allowedIntComparisons[i] == cmp_code)
      { member = true; break; }
    }
    if(!member)
      error(form,"The integer comparison code'",cmp_code,"' does not exist.");
    out += emitCode(nth(form,2));
    out += emitCode(nth(form,3));
    return out + get_unique_res("i1") + " = icmp " + cmp_code + " "
    + latest_type() + " " + get_res(res_version-1) + ", " + get_res(res_version);
  }
  
  string fcmp(Form* form)
  {
    string out;
    string cmp_code = val(nth(form,1));
    bool member;
    for(unsigned long i = 0; i < allowedFloatComparisons.size(); i++)
    {
      if(allowedFloatComparisons[i] == cmp_code)
      { member = true; break; }
    }
    if(!member)
      error(form,"The integer comparison code'",cmp_code,"' does not exist.");
    out += emitCode(nth(form,2));
    out += emitCode(nth(form,3));
    return out + get_unique_res("i1") + " = fcmp " + cmp_code + " "
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
    out += allocate(get_unique_tmp(),"i1");
    unsigned long address = tmp_version;
    if(latest_type() != "i1")
      error(form,"The test (Second argument) of an (if) form must evaluate to a boolean (i1) value.");
    else
      out += "br i1 " + get_current_res() + ",";
    out += " label " + get_unique_label();
    out += ", label " + get_unique_label() + "\n";
    out += functional_label(get_label(label_version-1));
    out += emitCode(nth(form,2));
    out += store("i1","true",get_tmp(address));
    out += "br label " + get_label(label_version+1) + "\n";
    out += functional_label(get_label(label_version));
    out += emitCode(nth(form,3));
    out += store("i1","false",get_tmp(address));
    out += "br label " + get_label(label_version+1) + "\n";
    out += functional_label(get_unique_label());
    out += load(get_unique_res("i1"),"i1",get_tmp(address));
    return out;
  }
  
  string flow(Form* form)
  {
    string out = emitCode(nth(form,1));
    string true_branch_type, false_branch_type;
    string address = gensym();
    string tmp;
    if(latest_type() != "i1")
      error(form,"The test (Second argument) of a (flow) form must evaluate to a boolean (i1) value.");
    else
      out += "br i1 " + get_current_res() + ",";
    out += " label " + get_unique_label() + ", label ";
    out += get_unique_label() + "\n";
    out += functional_label(get_label(label_version-1));
    out += emitCode(nth(form,2));
    true_branch_type = latest_type();
    out += store(true_branch_type,get_current_res(),address);
    out += "br label " + get_unique_label() + "\n";
    out += functional_label(get_label(label_version-1));
    out += emitCode(nth(form,3));
    false_branch_type = latest_type();
    out += store(true_branch_type,get_current_res(),address);
    out += "br label " + get_label(label_version) + "\n";
    out += functional_label(get_label(label_version));
    if(true_branch_type != false_branch_type)
      error(form,"Both branches of a (flow) statement must evaluate to the same type. Here, the true branch returns a '",
            true_branch_type,"', while the false branch returns a '",false_branch_type,"'.");
    else
    tmp = allocate(address,true_branch_type);
    out += load(get_unique_res(true_branch_type),true_branch_type,address);
    return tmp+out;
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
    string out = "declare " + val(nth(form,2)) + " @" + val(nth(form,1)) + "(";
    for(unsigned int i = 3; i <= length(form)-1; i++)
    {
      out += val(nth(form,i)) + ", ";
    }
    out = cutlast(out) + ")\n";
    //...
  }
  
  string embed_llvm(Form* form)
  {
    string out = print(cdr(form));
    return string(out,1,out.length()-2);
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
    out += (string)"call " + ret_type + " " + c_conv + " @" + name + "(";
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
          map<long,string>::iterator aide = inputs.begin();
          for(map<string,pair<long,string> >::iterator finder = seeker->second.members.begin();
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
        map<string,map<string,pair<long,string> > >::iterator finder = Generics[i].second.specializations.find(type);
        if(finder != Generics[i].second.specializations.end())
        {
          //Found the type in the Generic's specializations
          if(nargs == finder->second.size())
          {
            //Iterate over members and compare it to inputs
            //Check types
            map<long,string>::iterator aide = inputs.begin();
            for(map<string,pair<long,string> >::iterator looker = finder->second.begin();
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
    vector<long> inputs;
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
    array_type = "[" + to_string<long>(length(form)-1) + " x " + type + "]";
    string address = (string)(global ? "@" : "%") + "array" + to_string<unsigned long>(++array_version);
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
    long address_location = res_version;
    //Code for array
    out += emitCode(nth(in,2));
    out += get_unique_tmp() + " = getelementptr inbounds " + latest_type() + "* " + get_current_res()
        + ", i64 " + get_res(address_location);
    out += load(get_unique_res(cutlast(latest_type())),latest_type(),get_current_tmp());
    return out;
  }
  
  string mem_allocate(Form* in)
  {
    //%x = allocate [type]
    string type = printTypeSignature(nth(in,1));
    string out = allocate(get_unique_tmp(),type);
    out += allocate(get_unique_res(type+"*"),type);
    return out;
  }
  
  string mem_store(Form* in)
  {
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
  
  string pointer(Form* in)
  {
    string out = emitCode(nth(in,1));
    return out + get_unique_res(latest_type()+"*") + " = " + get_current_tmp();
  }
  
  string toplevel_asm(Form* in)
  {
    if(cdr(in) == NULL)
      error(in,"No Assembly code provided.");
    return (string)"module asm \"" + cdr(in) + "\"\n";
  }
  
  string inline_asm(Form* in)
  {
    string out;
    string type = val(nth(in,1));
    string dialect = val(nth(in,2));
    string code = print(cdr(cdr(cdr(in))));
    if(dialect != "Intel" && dialect != "ATT")
      error(in,"Unknown assembly dialect: Only 'Intel' and 'ATT' are supported, '"
      ,dialect,"' was given.");
    out = allocate(get_unique_tmp(),type);
    out += get_unique_tmp() + " = call " + type + " asm "
        + ((dialect == "Intel" ? "inteldailect" : "")) + " \"" + code + "\", ""()";
    return out;
  }
  
  void init_stdlib()
  {
    Scope new_scope;
    SymbolTable.push_back(new_scope);
    //Init Toplevel
    TopLevel["def"]              = &def_global;
    TopLevel["main"]             = &main_fn;
    TopLevel["LLVM"]             = &embed_llvm;
    TopLevel["function"]         = &define_function;
    TopLevel["recursive"]        = &define_recursive;
    TopLevel["fast"]             = &define_fast;
    TopLevel["inline"]           = &define_inline;
    TopLevel["inline-recursive"] = &define_inline_recursive;
    TopLevel["inline-fast"]      = &define_inline_fast;
    TopLevel["foreign"]          = &foreign;
    TopLevel["type"]             = &makeType;
    TopLevel["structure"]        = &makeStructure;
    TopLevel["generic"]          = &genericInterface;
    TopLevel["asm"]              = &toplevel_asm;
    //Init Core
    Core["def"]         = &def_local;
    Core["set"]         = &set;
    Core["ret"]         = &ret;
    Core["add"]         = &add;
    Core["fadd"]        = &fadd;
    Core["sub"]         = &sub;
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
    Core["begin"]       = &begin;
    Core["if"]          = &simple_if;
    Core["flow"]        = &flow;
    Core["construct"]   = &construct;
    Core["access"]      = &access;
    Core["array"]       = &local_array;
    Core["nth"]         = &nth_array;
    Core["call"]        = &direct_call;
    Core["allocate"]    = &mem_allocate;
    Core["store"]       = &mem_store;
    Core["load"]        = &mem_load;
    Core["asm"]         = &inline_asm;
    //Word macros
    addWordMacro("bool","i1");
    addWordMacro("char","i8");
    addWordMacro("short","i16");
    addWordMacro("int","i32");
    addWordMacro("long","i64");
    //Allowed comparisons for numerical operations
    allowedIntComparisons = {"eq", "ne", "ugt", "uge", "ult", "ule", "sgt"
                             "sge", "slt", "sle"};
    allowedFloatComparisons = {"false", "oeq", "ogt", "oge", "olt", "ole", "one",
                               "ord", "ueq", "ugt", "uge", "ult", "ule", "une",
                               "uno", "true"};
  }
