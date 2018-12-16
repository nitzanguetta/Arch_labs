#include "toy_stdio.h"


int main(int argc, char *argv[]) {
  	int chars_printed = toy_printf("%d, %u \n", -1, -1);
  	chars_printed += toy_printf("%x, %X \n", -1, -1);
  	int integers_array[] = {-1,2,3,4,5};
	char * strings_array[] = {"This", "is", "array", "of", "strings"};
	char chars_array[] = {'c', 'h', 'a', 'r', 's'};
	int array_size = 5;
	chars_printed += toy_printf("Print array of integers: %Ad\n", integers_array, array_size);
	chars_printed += toy_printf("Print array of integers: %Ab\n", integers_array, array_size);
	chars_printed += toy_printf("Print array of integers: %Ao\n", integers_array, array_size);
	chars_printed += toy_printf("Print array of integers: %Au\n", integers_array, array_size);
	chars_printed += toy_printf("Print array of integers: %Ax\n", integers_array, array_size);
	chars_printed += toy_printf("Print array of integers: %AX\n", integers_array, array_size);
	chars_printed += toy_printf("Print array of strings: %As\n", strings_array, array_size);
	chars_printed += toy_printf("Print array of chars: %Ac\n", chars_array, array_size);
  //chars_printed += toy_printf("Welcome to \\c\\n");
  //chars_printed += toy_printf("Support for explicit\\N");
  toy_printf("Printed %d chars\n", chars_printed); 
}
