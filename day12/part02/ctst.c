#include <stdio.h>

typedef unsigned long long ull;

void main(void) {
  int a = 0;
  ull i;

  for (i = 1; i != 0; ++i) {
    a = i;
  }

  printf("%llu\n", i - 1);
}
