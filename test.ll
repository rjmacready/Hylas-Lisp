@x7 = global i64 zeroinitializer
define i64* @entry(){
%res.version0 = select i1 true, i64 10, i64 10
store i64 %res.version0, i64* @x7
%res.version1 = load i64* @x7
%res.version2 = load i64* @x7
%res.version3 = select i1 true, i64* @x7, i64* @x7
ret i64* %res.version3
}
