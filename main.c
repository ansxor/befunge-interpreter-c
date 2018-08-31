#include <stdio.h>
#include "befunge.c"

int main() {
  bfinit();

  while (bfsync() == 1);
  printf("Success!\n");

  return 0;
}
