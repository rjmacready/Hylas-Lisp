/*!
 * @file core.cpp
 * @brief Implementation of the functions that make up the Hylas language core.
 * @author Eudoxia
 */
  
namespace Hylas
{
  using namespace std;
  using namespace llvm;
  
  string def(Form* form, bool global, bool typeonly)
  {
    string varname = val(nth(form,1));
    //cout << "Var to be def'd: " << varname << endl;
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
      out += emitCode(nth(form,2));
      type = latest_type();
    }
    string fullname = (global?"@":"%")+varname+to_string(ScopeDepth);
    if(global && !typeonly)
      push(fullname + " = global " + type + " zeroinitializer");
    else if(!global && !typeonly)
      out += allocate(fullname,latest_type());
    if(!typeonly)
    {
      out += store(type,get_current_res(),fullname);
      master.SymbolTable[ScopeDepth].vars[varname].type = type;
      master.SymbolTable[ScopeDepth].vars[varname].constant = false;
      master.SymbolTable[ScopeDepth].vars[varname].global = global;
      master.SymbolTable[ScopeDepth].vars[varname].address = fullname;
      master.SymbolTable[ScopeDepth].vars[varname].regtype = SymbolicRegister;
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
  
  string add(Form* form)                { return generic_math(form,"add");   }
  string fadd(Form* form)               { return generic_math(form,"fadd"); }
  string sub(Form* form)                { return generic_math(form,"sub");  }
  string fsub(Form* form)               { return generic_math(form,"fsub"); }
  string mul(Form* form)                { return generic_math(form,"mul");  }
  string fmul(Form* form)               { return generic_math(form,"fmul"); }
  string udiv(Form* form)               { return generic_math(form,"udiv"); }
  string sdiv(Form* form)               { return generic_math(form,"sdiv"); }
  string fdiv(Form* form)               { return generic_math(form,"fdiv"); }
  string urem(Form* form)               { return generic_math(form,"urem"); }
  string srem(Form* form)               { return generic_math(form,"srem"); }
  string frem(Form* form)               { return generic_math(form,"frem"); }
  
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
      error(form,"The floating point comparison code'",cmp_code,"' does not exist.");
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
  
  string generic_bitop(Form* form, string opcode)
  {
    string out = emitCode(nth(form,1));
    unsigned long address = res_version;
    out += emitCode(nth(form,2));
    if(latest_type() != res_type(get_res(address)))
      error(form,"Both arguments to (",opcode,") must be of the same type.");
    return out + get_unique_res(latest_type()) + " = " + opcode + " " + latest_type()
    + " " + get_res(address) + ", " + get_current_res();
  }
  
  string shl(Form* form)                { return generic_bitop(form,"shl");  }
  string lshr(Form* form)               { return generic_bitop(form,"lshr"); }
  string ashr(Form* form)               { return generic_bitop(form,"ashr"); }
  string bit_and(Form* form)            { return generic_bitop(form,"and");  }
  string bit_or(Form* form)             { return generic_bitop(form,"or");   }
  string bit_xor(Form* form)            { return generic_bitop(form,"xor");  }
  
  
  string byte_swap(Form* form)
  {
    string out = emitCode(nth(form,1));
    string type = latest_type();
    if(!isInteger(type))
      error(form,"The argument to (byte-swap) must be an integer with an even \
number of bytes, but an object of type '",type,"' was given.");
    if(!(width(type) % 16 == 0))
      error(form,"The argument to (byte-swap) must be an integer with an even \
number of bytes, but an integer with a width of ",to_string(width(type))," was given.");
    push("declare " + type + " @llvm.bswap." + type + "(" + type + " %n)");
    return out + get_unique_res(type) + " = call " + type + " @llvm.bswap." + type + "(" + type
           + " " + get_res(res_version-1);
  }
  
  string count_ones(Form* form)
  {
    string out = emitCode(nth(form,1));
    string type = latest_type();
    if(!isInteger(type))
      error(form,"The argument to (count-ones) must be an integer, but an \
object of type '",type,"' was given.");
    push("declare " + type + " = call " + type + " @llvm.ctpop." + type + "(" + type + " %n)");
    return out + get_unique_res(type) + " = @llvm.ctpop." + type + "(" + type
           + " " + get_res(res_version-1);
  }
  
  /*
  string count-leading-zeros(Form* form)
  { }
  string count-trailing-zeros(Form* form)
  { }
  */
  
  string truncate(Form* form)
  {
    string out = emitCode(nth(form,1));
    if(isInteger(latest_type()))
    {
      //Truncate in integer mode
      string to = printTypeSignature(nth(form,2));
      if(!isInteger(to))
        error(form,"The first argument to (truncate) was an integer, so the \
second argument must be an integer of a lesser bit width. Here, the second \
argument was '",to,"'.");
      if(!(width(latest_type()) > width(to)))
        error(form,"The bit width of the first argument to (truncate) must be \
greater than the bit width of the second argument. In this case, they were ",
width(latest_type())," and ",width(to),", respectively.");
      return out + get_unique_res(to) + " = trunc " + latest_type() + " "
            + get_current_res() + " to " + to;
    }
    else if(isCoreType(latest_type())) //Automatically a float
    {
      //Truncate in FP mode
      string to = printTypeSignature(nth(form,2));
      if(!isCoreType(to))
        error(form,"The first argument to (truncate) was a float, so the \
second argument must be a float of a lesser bit width. Here, the second \
argument was '",to,"'.");
      if(!(fpwidth(latest_type()) > fpwidth(to)))
        error(form,"The bit width of the first argument to (truncate) must be \
greater than the bit width of the second argument. In this case, they were ",
width(latest_type())," and ",width(to),", respectively.");
      return out + get_unique_res(to) + " = fptrunc " + latest_type() + " "
            + get_current_res() + " to " + to;
    }
    else
      error(form,"The first argument to truncate must be an integer or a \
floating point type, but an object of type ",latest_type()," was given.");
    return "";
  }
  
  string extend(Form* form)
  {
    string out = emitCode(nth(form,1));
    if(isInteger(latest_type()))
    {
      //Extend in integer mode
      string to = printTypeSignature(nth(form,2));
      if(!isInteger(to))
        error(form,"The first argument to (extend) was an integer, so the \
second argument must be an integer of a lesser bit width. Here, the second \
argument was '",to,"'.");
      if(!(width(latest_type()) < width(to)))
        error(form,"The bit width of the first argument to (extend) must be \
greater than the bit width of the second argument. In this case, they were ",
width(latest_type())," and ",width(to),", respectively.");
      return out + get_unique_res(to) + " = sext " + latest_type() + " "
            + get_current_res() + " to " + to;
    }
    else if(isCoreType(latest_type())) //Automatically a float
    {
      //Extend in FP mode
      string to = printTypeSignature(nth(form,2));
      if(!isCoreType(to))
        error(form,"The first argument to (truncate) was a float, so the \
second argument must be a float of a lesser bit width. Here, the second \
argument was '",to,"'.");
      if(!(fpwidth(latest_type()) < fpwidth(to)))
        error(form,"The bit width of the first argument to (truncate) must be \
greater than the bit width of the second argument. In this case, they were ",
width(latest_type())," and ",width(to),", respectively.");
      return out + get_unique_res(to) + " = fpext " + latest_type() + " "
            + get_current_res() + " to " + to;
    }
    else
      error(form,"The first argument to truncate must be an integer or a \
floating point type, but an object of type ",latest_type()," was given.");
    return "";
  }
  
  string zextend(Form* form)
  {
    string out = emitCode(nth(form,1));
    string to = printTypeSignature(nth(form,2));
    if(!isInteger(latest_type()) || !isInteger(to))
      error(form,"Both arguments to (zextend) must be integers.");
    if(!(width(latest_type()) < width(to)))
      error(form,"The bit width of the first argument to (zextend) must be \
lesser than the bit width of the second argument. In this case, they were ",
width(latest_type())," and ",width(to),", respectively.");
    return out + get_unique_res(to) + " = zext " + latest_type() + " "
           + get_current_res() + " to " + to;
  }
  
  string sextend(Form* form)
  {
    string out = emitCode(nth(form,1));
    string to = printTypeSignature(nth(form,2));
    if(!isInteger(latest_type()) || !isInteger(to))
      error(form,"Both arguments to (sextend) must be integers.");
    if(!(width(latest_type()) < width(to)))
      error(form,"The bit width of the first argument to (sextend) must be \
lesser than the bit width of the second argument. In this case, they were ",
width(latest_type())," and ",width(to),", respectively.");
    return out + get_unique_res(to) + " = sext " + latest_type() + " "
           + get_current_res() + " to " + to;
  }
  
  /*
  string floattoint(Form* form, bool to_float, bool signed)
  { }
  string inttofloat(Form* form)
  { }
  string floattohalf(Form* form)
  { }
  string halftofloat(Form* form)
  { }
  */
  
  string ptrtoint(Form* form)
  {
    string out = emitCode(nth(form,1));
    string to = printTypeSignature(nth(form,2));
    if(latest_type()[latest_type().length()-1] != '*')
      error(form,"The first argument to (pointer->integer) must be a pointer,\
but an object of type '",latest_type(),"' was given.");
    if(!isInteger(to))
      error(form,"The first second to (pointer->integer) must be an integer,\
but an object of type '",latest_type(),"' was given.");
    return out + get_unique_res(to) + " = ptrtoint " + latest_type() + " "
           + get_current_res() + " to " + to;
  }

  string inttoptr(Form* form)
  {
    string out = emitCode(nth(form,1));
    string to = printTypeSignature(nth(form,2));
    if(!isInteger(latest_type()))
      error(form,"The first second to (integer->pointer) must be a pointer,\
but an object of type '",latest_type(),"' was given.");
    if(to[to.length()-1] != '*')
      error(form,"The first argument to (integer->pointer) must be an integer,\
but an object of type '",latest_type(),"' was given.");
    return out + get_unique_res(to) + " = inttoptr " + latest_type() + " "
           + get_current_res() + " to " + to;
  }
  
  string bitcast(Form* form)
  {
    string out = emitCode(nth(form,1));
    string to = printTypeSignature(nth(form,2));
    return out + get_unique_res(to) + " = bitcast " + latest_type() + " "
           + get_current_res() + " to " + to;
  }
  
  string size(Form* form)
  {
    string type = printTypeSignature(nth(form,1));
    unsigned long size = typeSize(type);
    return constant(get_unique_res(mword),mword,to_string(size));
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
      //Found matching type name, check if it's a structure
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
    out += load(get_unique_res_address(member_type,to_string(tmp_version)),member_type,get_current_tmp());
    return out;
  }
  
  string flow(Form* form)
  {
    string out = emitCode(nth(form,1));
    if(latest_type() != "i1")
      error(form,"The test (Second argument) of a (flow) form must evaluate to a boolean (i1) value.");
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
  
  string simple_if(Form* form)
  {
    string out = emitCode(nth(form,1));
    string true_branch_type;
    unsigned long cond_address, true_address, false_address;
    cond_address = res_version;
    if(latest_type() != "i1")
      error(form,"The test (Second argument) of an (if) form must evaluate to a boolean (i1) value.");
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
      error(form,"Both branches of an (if) statement must evaluate to the same type. Here, the true branch returns a '",
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
    string out = "define i32 @main(i32 %argc, i8** %argv) nounwind {\n";
    for(unsigned int i = 1; i < length(form); i++)
      out += emitCode(nth(form,i));
    push(out + "\nret " + latest_type() + " " + get_current_res()+ "\n}");
    return constant(get_unique_res("i1"),"i1","true");
  }
  
  string foreign(Form* form)
  {
    Lambda newfn;
    string name_style = val(nth(form,1));
    string raw_name = val(nth(form,2));
    if(name_style == "C")
    {
      //Unimplemented
    }
    else if(name_style == "C++")
    {
      //Unimplemented
    }
    else
      error(form,"The first argument to foreign must either be 'C' (For no name mangling) or 'C++' (For C++ compatible mangling of function names).");
    newfn.name = raw_name;
    if(isatom(nth(form,3)))
    {
      if(val(nth(form,3)) == "void")
        newfn.ret_type = "void";
      else
        newfn.ret_type = printTypeSignature(nth(form,3));
      //FIXME repetition
    }
    else
      newfn.ret_type = printTypeSignature(nth(form,3));
    newfn.fn_ptr_type += newfn.ret_type + "(";
    newfn.nargs = length(form)-1;
    newfn.fastcc = false;
    newfn.tco = false;
    newfn.lining = false;
    for(unsigned long i = 4; i < length(form); i++)
    {
      if(isatom(nth(form,i)))
      {
        if(val(nth(form,i)) == "...")
        {
          newfn.fn_ptr_type += "...,";
          continue;
        }
      }
      newfn.fn_ptr_type += printTypeSignature(nth(form,i)) + ",";
    }
    //cout << length(form) << endl;
    if(length(form) == 4)
      newfn.fn_ptr_type += ")*";
    else
      newfn.fn_ptr_type = cutlast(newfn.fn_ptr_type) + ")*";
    string rem = removeReturn(newfn.fn_ptr_type);
    map<string,MetaFunction>::iterator seeker = FunctionTable.find(newfn.name);
    if(seeker != FunctionTable.end())
    {
      for(unsigned int i = 0; i < seeker->second.versions.size(); i++)
      {
        if(removeReturn(seeker->second.versions[i].fn_ptr_type) == rem) //Compare prototypes without comparing return types
          error(form,"A function with the same prototype (",cutlast(rem),") has already been defined.");
      }
      //newfn.name += to_string(seeker->second.versions.size()-1);
      seeker->second.versions.push_back(newfn);
    }
    else
    {
      //newfn.name = newfn.name + "0";
      MetaFunction new_metafn;
      new_metafn.versions.push_back(newfn);
      FunctionTable[newfn.name] = new_metafn;
    }
    push("declare " + newfn.ret_type + " @" + raw_name + cutlast(rem));
    return constant(get_unique_res(newfn.fn_ptr_type),newfn.fn_ptr_type,"@"+newfn.name);
  }
  
  string embed_llvm(Form* form)
  {
    string out = val(nth(form,1));
    push(cutfirst(cutlast(out)));
    return constant(get_unique_res("i1"),"i1","true");
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
  
  string make_structure(Form* form)
  {
    string out;
    string type = "{";
    vector<unsigned long> results;
    vector<string> type_list;
    for(unsigned long i = 1; i < length(form); i++)
    {
      out += emitCode(nth(form,i));
      type += latest_type() + ",";
      type_list.push_back(latest_type());
      results.push_back(res_version);
    }
    type = cutlast(type) + "}";
    out += allocate(get_unique_tmp(),type);
    unsigned long allocation_point = tmp_version;
    for(unsigned long i = 0; i < results.size(); i++)
    {
      out += get_unique_tmp() + " = getelementptr inbounds " + type + "* " + get_tmp(allocation_point)
          + ", i32 0, i32 " + to_string<unsigned long>(i) + "\n";
      out += store(type_list[i],get_res(results[i]),get_current_tmp());
    }
    return out;
  }
  
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
    out += load(get_unique_res_address(cutlast(latest_type()),to_string(tmp_version)),latest_type(),get_current_tmp());
    return out;
  }
  
  string mem_allocate(Form* in)
  {
    //%x = allocate [type]
    string type = printTypeSignature(nth(in,1));
    string out = allocate(get_unique_res(type+"*"),type);
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
    Variable* latest = lookup(get_current_res());
    if(latest == NULL)
      error(in,"Can't find the variable.");
    //Is the input an LValue? That is, does it have an address?
    if(latest->regtype == LValue)
      out += constant(get_unique_res(latest_type()+"*"),latest_type()+"*",get_tmp(from_string<unsigned long>(latest->address)));
    else if(latest->regtype == SymbolicRegister)
      out += constant(get_unique_res(latest_type()+"*"),latest_type()+"*",latest->address);
    else
      error(in,"The input to (address) must be an lvalue (A symbol or the result of (access) or (nth))");
    return out;
  }
  
  string toplevel_asm(Form* in)
  {
    if(cdr(in) == NULL)
      error(in,"No Assembly code provided.");
    push((string)"module asm \"" + cutfirst(cutlast(val(cadr(in)))) + "\"\n");
    return constant(get_unique_res("i1"),"i1","true");
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
    map<string,Form*>::iterator checker = WordMacros.find(word);
    if(checker != WordMacros.end())
    {
      if(master.allow_RedefineWordMacros)
        warn(in,"Redefining word macro '",word,"'.");
      else
        error(in,"A word macro with that name ('",word,"') has already been defined.");
    }
    WordMacros[word] = nth(in,2);
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
  
  string link_with_library(Form* form)
  {
    string filename = cutfirst(cutlast(val(nth(form,1))));
    llvm::Linker L("Hylas Lisp", master.Program, llvm::Linker::QuietWarnings
                  | llvm::Linker::QuietErrors);
    L.addSystemPaths();
    bool Native = true;
    if (L.LinkInLibrary(filename, Native)) {
      // that didn't work, try bitcode:
      llvm::sys::Path FilePath(filename);
      std::string Magic;
      if (!FilePath.getMagicNumber(Magic, 64)) {
        // filename doesn't exist...
        L.releaseModule();
        error(form,"Filename doesn't exist");
      }
      if (llvm::sys::IdentifyFileType(Magic.c_str(), 64)
          == llvm::sys::Bitcode_FileType) {
        // We are promised a bitcode file, complain if it fails
        L.setFlags(0);
        if (L.LinkInFile(llvm::sys::Path(filename), Native)) {
          L.releaseModule();
          error(form,"I dunno, something");
        }
      } else {
        // Nothing the linker can handle
        L.releaseModule();
        error(form,"Linker error, not a shared library or bc file");
      }
    } else if (Native) {
      // native shared library, load it!
      llvm::sys::Path SoFile = L.FindLib(filename);
      assert(!SoFile.isEmpty() && "We know the shared lib exists but can't find it back!");
      std::string errMsg;
      if(llvm::sys::DynamicLibrary::LoadLibraryPermanently(SoFile.str().c_str(), &errMsg)) {
        L.releaseModule();
        error(form,"Failed to load shared library");
        L.releaseModule();
      }
    }
    L.releaseModule();
    /*bool isnative = true;
    if(master.Loader.LinkInLibrary(libname,isnative))
    {
      //Couldn't find the lib
      sys::Path filepath(libname);
    }
    else
    {
      //Native lib
      llvm::sys::Path filepath = master.Loader.FindLib(libname);
      assert(!SoFile.isEmpty() && "We know the shared lib exists but can't find it back!");
      string errMsg;
      if(DynamicLibrary::LoadLibraryPermanently(filepath.str().c_str(), &errMsg))
        error(form,"Failed to load shared library.");
    }
    bool derp = true;
    master.Program->addLibrary(libname);
    if(master.Loader->LinkInLibrary(libname,derp))
      nerror(master.Loader->getLastError());
    bool herp = sys::DynamicLibrary::LoadLibraryPermanently(libname.c_str());
    if(derp == false || herp == false)
      error(form,"Error loading the library '",libname,"'.");
    vector<string> libs = master.Program->getLibraries();
    for(unsigned long i = 0; i < libs.size(); i++)
    {
      cerr << "Using library: " << libs[i] << endl;
    }*/
    return constant(get_unique_res("i1"),"i1","true");
  }
  
  /*!
   * @brief Maps core function names to their corresponding functions.
   */
  void init_stdlib()
  {
    Scope new_scope;
    master.SymbolTable.push_back(new_scope);
    //Init Core
    TopLevel["main"]         = &main_fn;
    TopLevel["def"]          = &def_global;
    TopLevel["def-as"]       = &def_as_global;
    TopLevel["type"]         = &makeType;
    TopLevel["structure"]    = &makeStructure;
    TopLevel["generic"]      = &genericInterface;
    Core["asm"]              = &toplevel_asm;
    Core["inline-asm"]       = &inline_asm;
    TopLevel["word"]         = &word;
    TopLevel["prefix"]       = &prefix;
    TopLevel["postfix"]      = &postfix;
    Core["LLVM"]             = &embed_llvm;
    Core["inline-LLVM"]      = &embed_llvm;
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
    //Core["ret"]              = &ret;
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
    Core["shl"]              = &shl;
    Core["lshr"]             = &lshr;
    Core["ashr"]             = &ashr;
    Core["bitwise-and"]      = &bit_and;
    Core["bitwise-or"]       = &bit_or;
    Core["bitwise-xor"]      = &bit_xor;
    Core["byte-swap"]        = &byte_swap;
    Core["count-ones"]       = &count_ones;
    Core["truncate"]         = &truncate;
    Core["extend"]           = &extend;
    Core["zextend"]          = &zextend;
    Core["sextend"]          = &sextend;
    Core["pointer->integer"] = &ptrtoint;
    Core["integer-pointer"]  = &inttoptr;
    Core["size"]             = &size;
    Core["bitcast"]          = &bitcast;
    Core["begin"]            = &begin;
    Core["if"]               = &simple_if;
    Core["flow"]             = &flow;
    Core["make-structure"]   = &make_structure;
    Core["construct"]        = &construct;
    Core["access"]           = &access;
    Core["array"]            = &local_array;
    Core["nth"]              = &nth_array;
    Core["allocate"]         = &mem_allocate;
    Core["store"]            = &mem_store;
    Core["load"]             = &mem_load;
    Core["address"]          = &address;
    Core["import"]           = &import;
    Core["link"]             = &link_with_library;
    //Word macros
    addWordMacro("bool","i1");
    addWordMacro("byte","i8");
    addWordMacro("octet","i8");
    addWordMacro("short","i16");
    addWordMacro("int","i32");
    addWordMacro("long","i64");
    addWordMacro("octet","i8");
    addWordMacro("word",mword);
    //Architectures
    #if defined(__alpha__) || defined(__alpha) || defined(_M_ALPHA)
    addWordMacro("System.architecture","Alpha");
    #elif defined(__amd64__) || defined(_M_AMD64)
    addWordMacro("System.architecture","AMD64");
    #elif defined(__arm__) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) \
           || defined(_ARM) || defined(_M_ARM) || defined(_M_ARMT)
    addWordMacro("System.architecture","ARM");
    #elif defined(__convex__)
    addWordMacro("System.architecture","Convex");
    #elif defined(__hppa__) || defined(__HPPA__)
    addWordMacro("System.architecture","HP/PA RISC");
    #elif defined(i386) || defined(__i386) || defined(_M_I86) || \
          defined(__X86__) || defined(_X86_) || defined(__THW_INTEL__) || \
          defined(__I86__) || defined(__INTEL__)
    addWordMacro("System.architecture","Intel x86");
    #elif defined(__ia64__) || defined(__ia64) || defined(_M_IA64) || \
          defined(__itanium__)
    addWordMacro("System.architecture","Intel Itanium");
    #endif
    /*addWordMacro("eq","seq");
    addWordMacro("<","slt");
    addWordMacro("u<","ult");*/
    //Allowed comparisons for numerical operations
    allowedIntComparisons = {"eq", "ne", "ugt", "uge", "ult", "ule", "sgt"
                             "sge", "slt", "sle"};
    allowedFloatComparisons = {"false", "oeq", "ogt", "oge", "olt", "ole", "one",
                               "ord", "ueq", "ugt", "uge", "ult", "ule", "une",
                               "uno", "true"};
  }
}