#include <stdio.h>
#include "befunge.c"

int main(int argc, char**argv) {
  bfinit();
  bfload("64+\"!dlroW ,olleH\">:#,_@");

  while (bfsync() == 1);
  printf("Success!\n");

  return 0;
}
