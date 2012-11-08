@str0 = global [4 x i8] c"\41\42\43\00"
@str1 = global [4 x i8] c"\44\45\46\00"
@str2 = global [4 x i8] c"\47\48\49\00"
define i8* @entry(){
%res.version23 = getelementptr [4 x i8]* @str0, i64 0, i64 0
%res.version24 = getelementptr [4 x i8]* @str1, i64 0, i64 0
%res.version25 = getelementptr [4 x i8]* @str2, i64 0, i64 0

ret i8* %res.version25
}
