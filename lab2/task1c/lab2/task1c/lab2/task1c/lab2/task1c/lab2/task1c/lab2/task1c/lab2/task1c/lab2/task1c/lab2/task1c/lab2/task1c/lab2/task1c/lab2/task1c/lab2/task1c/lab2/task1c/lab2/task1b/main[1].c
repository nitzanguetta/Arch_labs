#include "toy_stdio.h"


int main(int argc, char *argv[]) {
  int chars_printed = toy_printf("%d, %u \n", -1, -1);
  chars_printed = toy_printf("%x, %X \n", -1, -1);
  //chars_printed += toy_printf("Welcome to \\c\\n");
  //chars_printed += toy_printf("Support for explicit\\N");
  toy_printf("Printed %d chars\n", chars_printed); 
}
