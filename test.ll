deplibs = [ "curses" ]

@str0 = global [6 x i8] c"HELLO\00"

declare i32 @printw(i8*,...)

define i32 @main(i32 %argc, i8** %argv)
{
%res = call i32 (i8*, ...)* @printw(i8* getelementptr inbounds ([6 x i8]* @str0, i64 0, i64 0))
ret i32 0
}