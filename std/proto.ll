; ModuleID = 'proto.cpp'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64"
target triple = "x86_64-pc-linux-gnu"

@.str1 = private constant [3 x i8] c"%i\00"
@.str2 = private constant [5 x i8] c"%lli\00"
@.str3 = private constant [3 x i8] c"%E\00"
@.str5 = private constant [3 x i8] c", \00"
@.str7 = private constant [28 x i8] c"%s\0A%s\0A%s\0A%s\0A%s\0A%s\0A%s\0A%s\0A%s\0A\00"
@.str8 = private constant [13 x i8] c"derp herp!!!\00"
@.str9 = private constant [6 x i8] c"Herp \00"
@.str10 = private constant [6 x i8] c"Derp \00"
@.str11 = private constant [5 x i8] c"Berp\00"

define noalias i8* @_Z5printc(i8 signext %in) nounwind {
  %1 = tail call i8* @malloc(i64 2)
  store i8 %in, i8* %1, align 1
  %nul = getelementptr i8* %1, i64 1
  store i8 0, i8* %nul, align 1
  ret i8* %1
}

declare noalias i8* @malloc(i64) nounwind

declare i32 @sprintf(i8* nocapture, i8* nocapture, ...) nounwind

define i8* @_Z5prints(i16 signext %in) nounwind {
  %buf = alloca [200 x i8], align 16
  %1 = getelementptr inbounds [200 x i8]* %buf, i64 0, i64 0
  %2 = sext i16 %in to i32
  %3 = call i32 (i8*, i8*, ...)* @sprintf(i8* %1, i8* getelementptr inbounds ([3 x i8]* @.str1, i64 0, i64 0), i32 %2)
  %4 = add nsw i32 %3, 1
  %5 = sext i32 %4 to i64
  %6 = call i8* @malloc(i64 %5)
  %7 = call i8* @strcpy(i8* %6, i8* %1)
  ret i8* %6
}

declare i8* @strcpy(i8*, i8* nocapture) nounwind

define i8* @_Z5printi(i32 %in) nounwind {
  %buf = alloca [200 x i8], align 16
  %1 = getelementptr inbounds [200 x i8]* %buf, i64 0, i64 0
  %2 = call i32 (i8*, i8*, ...)* @sprintf(i8* %1, i8* getelementptr inbounds ([3 x i8]* @.str1, i64 0, i64 0), i32 %in)
  %3 = add nsw i32 %2, 1
  %4 = sext i32 %3 to i64
  %5 = call i8* @malloc(i64 %4)
  %6 = call i8* @strcpy(i8* %5, i8* %1)
  ret i8* %5
}

define i8* @_Z5printx(i64 %in) nounwind {
  %buf = alloca [200 x i8], align 16
  %1 = getelementptr inbounds [200 x i8]* %buf, i64 0, i64 0
  %2 = call i32 (i8*, i8*, ...)* @sprintf(i8* %1, i8* getelementptr inbounds ([5 x i8]* @.str2, i64 0, i64 0), i64 %in)
  %3 = add nsw i32 %2, 1
  %4 = sext i32 %3 to i64
  %5 = call i8* @malloc(i64 %4)
  %6 = call i8* @strcpy(i8* %5, i8* %1)
  ret i8* %5
}

define i8* @_Z5printf(float %in) nounwind {
  %buf = alloca [200 x i8], align 16
  %1 = getelementptr inbounds [200 x i8]* %buf, i64 0, i64 0
  %2 = fpext float %in to double
  %3 = call i32 (i8*, i8*, ...)* @sprintf(i8* %1, i8* getelementptr inbounds ([3 x i8]* @.str3, i64 0, i64 0), double %2)
  %4 = add nsw i32 %3, 1
  %5 = sext i32 %4 to i64
  %6 = call i8* @malloc(i64 %5)
  %7 = call i8* @strcpy(i8* %6, i8* %1)
  ret i8* %6
}

define i8* @_Z5printd(double %in) nounwind {
  %buf = alloca [200 x i8], align 16
  %1 = getelementptr inbounds [200 x i8]* %buf, i64 0, i64 0
  %2 = call i32 (i8*, i8*, ...)* @sprintf(i8* %1, i8* getelementptr inbounds ([3 x i8]* @.str3, i64 0, i64 0), double %in)
  %3 = add nsw i32 %2, 1
  %4 = sext i32 %3 to i64
  %5 = call i8* @malloc(i64 %4)
  %6 = call i8* @strcpy(i8* %5, i8* %1)
  ret i8* %5
}

define i8* @_Z5printPc(i8* %in) nounwind readnone {
  ret i8* %in
}

define i8* @_Z5print3Foo(i32 %in.coerce0, double %in.coerce1) nounwind {
  %buf.i = alloca [200 x i8], align 16
  %1 = getelementptr inbounds [200 x i8]* %buf.i, i64 0, i64 0
  %2 = call i32 (i8*, i8*, ...)* @sprintf(i8* %1, i8* getelementptr inbounds ([3 x i8]* @.str1, i64 0, i64 0), i32 %in.coerce0) nounwind
  %3 = add nsw i32 %2, 1
  %4 = sext i32 %3 to i64
  %5 = call i8* @malloc(i64 %4) nounwind
  %6 = call i8* @strcpy(i8* %5, i8* %1) nounwind
  %7 = call i32 (i8*, i8*, ...)* @sprintf(i8* %1, i8* getelementptr inbounds ([3 x i8]* @.str3, i64 0, i64 0), double %in.coerce1) nounwind
  %8 = add nsw i32 %7, 1
  %9 = sext i32 %8 to i64
  %10 = call i8* @malloc(i64 %9) nounwind
  %11 = call i8* @strcpy(i8* %10, i8* %1) nounwind
  %12 = call i64 @strlen(i8* %10) nounwind
  %13 = add i64 %12, 2
  %14 = call i8* @malloc(i64 %13) nounwind
  %15 = call i8* @strcat(i8* %14, i8* %10) nounwind
  %strlen = call i64 @strlen(i8* %14)
  %endptr = getelementptr i8* %14, i64 %strlen
  %16 = bitcast i8* %endptr to i16*
  store i16 125, i16* %16, align 1
  %17 = call i64 @strlen(i8* %14) nounwind
  %18 = add i64 %17, 3
  %19 = call i8* @malloc(i64 %18) nounwind
  %strlen2 = call i64 @strlen(i8* %19)
  %endptr3 = getelementptr i8* %19, i64 %strlen2
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %endptr3, i8* getelementptr inbounds ([3 x i8]* @.str5, i64 0, i64 0), i64 3, i32 1, i1 false)
  %20 = call i8* @strcat(i8* %19, i8* %14) nounwind
  %21 = call i64 @strlen(i8* %5) nounwind
  %22 = call i64 @strlen(i8* %19) nounwind
  %23 = add i64 %21, 1
  %24 = add i64 %23, %22
  %25 = call i8* @malloc(i64 %24) nounwind
  %26 = call i8* @strcat(i8* %25, i8* %5) nounwind
  %27 = call i8* @strcat(i8* %25, i8* %19) nounwind
  %28 = call i64 @strlen(i8* %25) nounwind
  %29 = add i64 %28, 2
  %30 = call i8* @malloc(i64 %29) nounwind
  %strlen4 = call i64 @strlen(i8* %30)
  %endptr5 = getelementptr i8* %30, i64 %strlen4
  %31 = bitcast i8* %endptr5 to i16*
  store i16 123, i16* %31, align 1
  %32 = call i8* @strcat(i8* %30, i8* %25) nounwind
  ret i8* %30
}

define i32 @main() nounwind {
  %buf.i = alloca [200 x i8], align 16
  %1 = call i8* @malloc(i64 2) nounwind
  store i8 97, i8* %1, align 1
  %nul.i = getelementptr i8* %1, i64 1
  store i8 0, i8* %nul.i, align 1
  %2 = getelementptr inbounds [200 x i8]* %buf.i, i64 0, i64 0
  %3 = call i32 (i8*, i8*, ...)* @sprintf(i8* %2, i8* getelementptr inbounds ([3 x i8]* @.str1, i64 0, i64 0), i32 56) nounwind
  %4 = add nsw i32 %3, 1
  %5 = sext i32 %4 to i64
  %6 = call i8* @malloc(i64 %5) nounwind
  %7 = call i8* @strcpy(i8* %6, i8* %2) nounwind
  %8 = call i32 (i8*, i8*, ...)* @sprintf(i8* %2, i8* getelementptr inbounds ([3 x i8]* @.str1, i64 0, i64 0), i32 288) nounwind
  %9 = add nsw i32 %8, 1
  %10 = sext i32 %9 to i64
  %11 = call i8* @malloc(i64 %10) nounwind
  %12 = call i8* @strcpy(i8* %11, i8* %2) nounwind
  %13 = call i32 (i8*, i8*, ...)* @sprintf(i8* %2, i8* getelementptr inbounds ([5 x i8]* @.str2, i64 0, i64 0), i64 53433345) nounwind
  %14 = add nsw i32 %13, 1
  %15 = sext i32 %14 to i64
  %16 = call i8* @malloc(i64 %15) nounwind
  %17 = call i8* @strcpy(i8* %16, i8* %2) nounwind
  %18 = call i32 (i8*, i8*, ...)* @sprintf(i8* %2, i8* getelementptr inbounds ([3 x i8]* @.str3, i64 0, i64 0), double 0x3F34940BC0000000) nounwind
  %19 = add nsw i32 %18, 1
  %20 = sext i32 %19 to i64
  %21 = call i8* @malloc(i64 %20) nounwind
  %22 = call i8* @strcpy(i8* %21, i8* %2) nounwind
  %23 = call i32 (i8*, i8*, ...)* @sprintf(i8* %2, i8* getelementptr inbounds ([3 x i8]* @.str3, i64 0, i64 0), double 3.140000e-01) nounwind
  %24 = add nsw i32 %23, 1
  %25 = sext i32 %24 to i64
  %26 = call i8* @malloc(i64 %25) nounwind
  %27 = call i8* @strcpy(i8* %26, i8* %2) nounwind
  %28 = call i8* @malloc(i64 10) nounwind
  %strlen = call i64 @strlen(i8* %28)
  %endptr = getelementptr i8* %28, i64 %strlen
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %endptr, i8* getelementptr inbounds ([6 x i8]* @.str10, i64 0, i64 0), i64 6, i32 1, i1 false)
  %strlen5 = call i64 @strlen(i8* %28)
  %endptr6 = getelementptr i8* %28, i64 %strlen5
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %endptr6, i8* getelementptr inbounds ([5 x i8]* @.str11, i64 0, i64 0), i64 5, i32 1, i1 false)
  %29 = call i64 @strlen(i8* %28) nounwind
  %30 = add i64 %29, 6
  %31 = call i8* @malloc(i64 %30) nounwind
  %strlen7 = call i64 @strlen(i8* %31)
  %endptr8 = getelementptr i8* %31, i64 %strlen7
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %endptr8, i8* getelementptr inbounds ([6 x i8]* @.str9, i64 0, i64 0), i64 6, i32 1, i1 false)
  %32 = call i8* @strcat(i8* %31, i8* %28) nounwind
  %33 = call i8* @_Z5print3Foo(i32 1, double 3.140000e+00)
  %34 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([28 x i8]* @.str7, i64 0, i64 0), i8* %1, i8* %6, i8* %11, i8* %16, i8* %21, i8* %26, i8* getelementptr inbounds ([13 x i8]* @.str8, i64 0, i64 0), i8* %31, i8* %33)
  ret i32 0
}

declare i32 @printf(i8* nocapture, ...) nounwind

declare i64 @strlen(i8* nocapture) nounwind readonly

declare i8* @strcat(i8*, i8* nocapture) nounwind

declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture, i8* nocapture, i64, i32, i1) nounwind
