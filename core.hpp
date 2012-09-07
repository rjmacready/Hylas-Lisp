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
      error_unbound(varname);
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
  
  string gep(Form* form)
  {
    return "[[GEP PLACEHOLDER]]";
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
  
  string foreign(Form* form)
  {
    string out = "declare " + val(nth(form,1)) + " @" + val(nth(form,2)) + "(";
    for(unsigned int i = 3; i <= length(form)-1; i++)
    {
      out += val(nth(form,i)) + ", ";
    }
    return string(out,0,out.length()-2) + ")";
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
  
  string construct(Form* form)
  {
    return "[[CONSTRUCT PLACEHOLDER]]";
  }
  
  void init_stdlib()
  {
    Scope new_scope;
    SymbolTable.push_back(new_scope);
    Core["def"]         = &def_local;
    Core["set"]         = &set;
    Core["return"]      = &ret;
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
    Core["gep"]         = &gep;
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
    //Type stuff
    Core["construct"]   = &construct;
    //FFI
    Core["foreign"]     = &foreign;
    //Word macros
    addWordMacro("bool","i1");
    addWordMacro("char","i8");
    addWordMacro("short","i16");
    addWordMacro("int","i32");
    addWordMacro("long","i64");
  }
