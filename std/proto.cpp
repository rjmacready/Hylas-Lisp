//Prototyping additions to the Hylas std lib
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Tuple
{ unsigned long size; void* data; };

Tuple* make_tuple(void* data, unsigned long size)
{ Tuple* tmp; tmp->data = data; tmp->size = size; return tmp; }

int main()
{
  char* tmp = (char*)malloc(4);
  tmp[0] = 'a'; tmp[1] = 'b'; tmp[2] = 'c'; tmp[3] = '\0';
  void* tmp2 = (void*)tmp;
  Tuple* a = make_tuple(tmp2,4);
  return 0;
}