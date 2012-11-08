@str0 = global [3 x i8] c"%i\00"

declare i32 @puts(i8*)

declare i32 @printf(i8*, ...)

declare i64 @strlen(i8*)

declare i8* @strcat(i8*, i8*)

declare i32 @sprintf(i8*, i8*, ...)

declare i8* @strcpy(i8*, i8*)

declare i8* @malloc(i64)

declare void @free(i8*)

declare i8* @realloc(i8*, i64)

define i8* @print0(fp128 %in1_base) {
  %in1 = alloca fp128
  store fp128 %in1_base, fp128* %in1
  %res.version9 = select i1 true, i64 200, i64 200
  %tmp.version0 = mul i64 %res.version9, 1
  %tmp.version1 = call i8* @malloc(i64 %tmp.version0)
  %res.version10 = bitcast i8* %tmp.version1 to i8*
  %buf1 = alloca i8*
  store i8* %res.version10, i8** %buf1
  %res.version11 = load i8** %buf1
  %res.version12 = getelementptr [3 x i8]* @str0, i64 0, i64 0
  %res.version13 = load fp128* %in1
  %res.version14 = call i32 (i8*, i8*, ...)* @sprintf(i8* %res.version11, i8* %res.version12, fp128 %res.version13)
  %res.version15 = select i1 true, i64 1, i64 1
  %res.version16 = trunc i64 %res.version15 to i32
  %res.version17 = add i32 %res.version14, %res.version16
  %res.version18 = sext i32 %res.version17 to i64
  %size1 = alloca i64
  store i64 %res.version18, i64* %size1
  %res.version19 = load i64* %size1
  %tmp.version2 = mul i64 %res.version19, 1
  %tmp.version3 = call i8* @malloc(i64 %tmp.version2)
  %res.version20 = bitcast i8* %tmp.version3 to i8*
  %str1 = alloca i8*
  store i8* %res.version20, i8** %str1
  %res.version21 = load i8** %buf1
  %tmp.version4 = bitcast i8* %res.version21 to i8*
  call void @free(i8* %tmp.version4)
  %res.version22 = select i1 true, i1 true, i1 true
  %res.version23 = load i8** %str1
  %res.version24 = load i8** %buf1
  %res.version25 = call i8* @strcpy(i8* %res.version23, i8* %res.version24)
  ret i8* %res.version25
}

define i32 @main()
{
%derp = call i8* @print0(ppc_fp128 4.324e+3342)
%derp = call i32 puts %derp
ret i32 0
}