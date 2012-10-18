declare i32 @printf(i8*,...)
define i32(i8*,...)* @entry(){
%res.version0 = select i1 true, i32(i8*,...)* @printf, i32(i8*,...)* @printf
ret i32(i8*,...)* %res.version0
}

declare i1 @exit(i32)
define i1(i32)* @entry1(){
%res.version0 = select i1 true, i1(i32)* @exit, i1(i32)* @exit
ret i1(i32)* %res.version0
}

declare i8* @malloc(i32,i32)
define i8*(i32,i32)* @entry2(){
%res.version0 = select i1 true, i8*(i32,i32)* @malloc, i8*(i32,i32)* @malloc
ret i8*(i32,i32)* %res.version0
}
