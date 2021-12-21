#include <time.h>
#include <stdio.h>
#include "general_functions.h"

unsigned int powerOfTwo(int exp)
{
  int i;
  unsigned int product = 1;
  for (i = 0; i < exp; i++)
    product *= 2;
  return product;
}

void display_time(double start, double end, char *label)
{
  double totalTime;

  totalTime = (double)((end - start) / CLOCKS_PER_SEC);
  printf("Runtime %s: %lf\n", label, totalTime);

  return;
}
