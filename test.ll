declare i8* @malloc(i64)
declare i32 @printf(i8*,...)
@str0 = global [44 x i8] c"\3c\30\78\25\58\20\70\6f\69\6e\74\65\72\20\74\6f\20\66\70\31\32\38\2a\20\77\69\74\68\20\69\6e\64\69\72\65\63\74\69\6f\6e\20\31\3e\00"
define i1 @entry(){
%res.version160 = select i1 true, i64 512, i64 512
%tmp.version35 = mul i64 %res.version160, 16
%tmp.version36 = call i8* @malloc(i64 %tmp.version35)
%res.version161 = bitcast i8* %tmp.version36 to fp128*

%res.version162 = getelementptr [44 x i8]* @str0, i64 0, i64 0
%uniqueness_guaranteed_450501987 = call i32 (i8*,...)* @printf(i8* %res.version162, fp128* %res.version161)
%res.version163 = select i1 true, i1 true, i1 true

ret i1 %res.version163
}