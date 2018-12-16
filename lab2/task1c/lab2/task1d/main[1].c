#include "toy_stdio.h"


int main(int argc, char *argv[]) {
  toy_printf("With numeric placeholders: %-05d\n", -2);	
  toy_printf("With numeric placeholders: %-5d\n", -2);	
  toy_printf("With numeric placeholders: %-5s\n", "bla");
  toy_printf("With numeric placeholders: %05d\n", -2);	
  toy_printf("With numeric placeholders: %5d\n", -2);	
  toy_printf("With numeric placeholders: %5s\n", "bla");
}