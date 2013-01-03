#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//Prototyping additions to the Hylas std lib

////Print functions

////// Integers

  char* print(char in)
  {
    char* str = (char*)malloc(sizeof(char)+1);
    sprintf(str,"%c",in);
    return str;
  }
  
  /*
   * (function print cstring ((in byte))
   *    (def tmp (bitcast cstring (malloc (add (size byte) 1))))
   *    (sprintf str "%c" in)
   *    str)
   */

  char* print(short in)
  {
    char buf[200];
    int size = sprintf(buf,"%i",in) + 1;
    char* str = (char*)malloc(sizeof(char)*size);
    strcpy(str,buf);
    return str;
  }

  char* print(int in)
  {
    char buf[200];
    int size = sprintf(buf,"%i",in) + 1;
    char* str = (char*)malloc(sizeof(char)*size);
    strcpy(str,buf);
    return str;
  }

  char* print(long long in)
  {
    char buf[200];
    int size = sprintf(buf,"%lli",in) + 1;
    char* str = (char*)malloc(sizeof(char)*size);
    strcpy(str,buf);
    return str;
  }

////// Floats

  char* print(float in)
  {
    char buf[200];
    int size = sprintf(buf,"%E",in) + 1;
    char* str = (char*)malloc(sizeof(char)*size);
    strcpy(str,buf);
    return str;
  }

  char* print(double in)
  {
    char buf[200];
    int size = sprintf(buf,"%E",in) + 1;
    char* str = (char*)malloc(sizeof(char)*size);
    strcpy(str,buf);
    return str;
  }
  
////// Aggregates

  char* print(char* in)
  {
    return in;
  }
  
inline char* base_cat(char* a, char* b)
{
  size_t length = strlen(a) + strlen(b) + 1;
  char* str = (char*)malloc(sizeof(char)*length);
  strcat(str,a);
  strcat(str,b);
  return str;
}

struct Foo
{
  int bar; double baz;
  Foo(int a, double b){ bar = a; baz = b; }
};

char* print(Foo in)
{
  return base_cat("{",base_cat(print(in.bar),base_cat(", ",base_cat(print(in.baz),"}"))));
}
  
int main()
{
  printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
         print('a'),
         print((short)56),
         print((int)288),
         print((long long)53433345),
         print(3.14e-4f),
         print(3.14e-1),
         print("derp herp!!!"),
         base_cat("Herp ",base_cat("Derp ","Berp")),
         print(Foo(1,3.14))
          );
  return 0;
}