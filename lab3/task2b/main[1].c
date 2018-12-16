#include "toy_stdio.h"


int main(int argc, char *argv[]) {
  //toy_printf("With numeric placeholders: %-05d\n", -2);	
  int integers_array[] = {1,-2,3,4,5};
 char * strings_array[] = {"This", "is", "array", "of", "strings"};
 char chars_array[] = {'C', 'h', 'a', 'r', 's'};
  int array_size = 5;

  int chars_printed=0;

chars_printed += toy_printf("s\n", integers_array,array_size);
chars_printed += toy_printf("%As\n", strings_array,array_size);
chars_printed += toy_printf("%Ac\n", chars_array,array_size);

chars_printed += toy_printf("%c, %s\n", 'b', "nitzan");
  chars_printed += toy_printf("%x, %X\n", 12, -12);
  chars_printed += toy_printf("%b, %b\n", 12, -12);
  chars_printed += toy_printf("%o, %o\n", 12, -12);
chars_printed += toy_printf("%u, %u\n", 12, -12);
chars_printed += toy_printf("%d, %d\n", 12, -12);
chars_printed += toy_printf("%-07d, %7d\n", 12, -12);
chars_printed += toy_printf("%-3s, %3s\n", "aa", "aa");
  toy_printf("Printed %d chars\n", chars_printed); 

}
 
