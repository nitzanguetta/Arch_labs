/* toy-io.c
 * Limited versions of printf and scanf
 *
 * Programmer: Mayer Goldberg, 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* the states in the printf state-machine */
enum printf_state {
  st_printf_init,
  st_printf_percent,
  st_printf_octal2,
  st_printf_octal3
};

#define MAX_NUMBER_LENGTH 64
#define is_octal_char(ch) ('0' <= (ch) && (ch) <= '7')

int toy_printf(char *fs, ...);

const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";

int get_value_length(int value){
	int i=0;
	if(value<0){
		i++;
		value=value*(-1);
	}
	while(value>=1){
		i++;
		value= value/10;
	}
	return i;
}
int get_value_length_string(char * value){
	int i=0;
 	while(*value){
	      i++;
	      value++;
	    }
	return i;
}


void fill_with_char(int width, int value_length, char char_to_add){
	for(int i=0;i<(width-value_length);i++)
		putchar(char_to_add);
}

int print_int_helper(unsigned int n, int radix, const char *digit) {
  int result;
  if (n < radix) {
    putchar(digit[n]);
    return 1;
  }
  else {
    result = print_int_helper(n / radix, radix, digit);
    putchar(digit[n % radix]);
    return 1 + result;
  }
}

int print_int(int n, int radix, const char * digit, int signed_format) {
  if (radix < 2 || radix > 16) {
    toy_printf("Radix must be in [2..16]: Not %d\n", radix);
    exit(-1);
  }
  if (n > 0) {
    return print_int_helper(n, radix, digit);
  }
  if (n == 0) {
    putchar('0');
    return 1;
  } 
  else {
     if(signed_format){
    	putchar('-');
    	return 1 + print_int_helper(-n, radix, digit);
     }else{
	unsigned int p=n;
    	return 1 + print_int_helper(p, radix, digit);
     }
  }
}

/* SUPPORTED:
 *   %b, %d, %o, %x, %X -- 
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */

int toy_printf(char *fs, ...) {
	int left_flag=0;
	int zero_flag=0;
	int width=0;
	int value_length=0;
	char char_to_add=' ';

  int* int_arr;
  char** string_arr;
  char* char_arr;
  int arr_size;
  int A_flag=0;
  int chars_printed = 0;
  int int_value = 0;
  char *string_value;
  char char_value;
  char octal_char;
  va_list args;
  enum printf_state state;

  va_start(args, fs);

  state = st_printf_init; 

  for (; *fs != '\0'; ++fs) {
    switch (state) {
    case st_printf_init:
      switch (*fs) {
      case '%':
	state = st_printf_percent;
	break;

      default:
	putchar(*fs);
	++chars_printed;
      }
      break;

    case st_printf_percent:
      switch (*fs) {
      case '%':
	putchar('%');
	++chars_printed;
	state = st_printf_init;
	break;

      case 'A':
		A_flag=1;
		break;

	case '-':
		left_flag=1;
		break;

      case 'd':
		if(A_flag){
			int_arr = va_arg(args, int *);
			arr_size = va_arg(args, int);
			printf("{");
	  		for(int i=0; i<arr_size;i++){
	    		chars_printed += print_int(int_arr[i], 10, digit,1);
	    		if(i!=arr_size-1) printf(",");
	 	 	}
			printf("}");
		}
		else{
			int_value = va_arg(args, int);
			if(int_value<0){
				int_value=int_value*(-1);
				putchar('-');
			} 
			if(width){
				value_length=get_value_length(int_value);
				if(zero_flag)
					char_to_add='0';
				if(left_flag)
					fill_with_char(width,value_length,char_to_add);
			}
	  		chars_printed += print_int(int_value, 10, digit,1);
	  		if(!left_flag&& width){
	  			fill_with_char(width,value_length,char_to_add);
	  			putchar('#');
	  		}
		}
		state = st_printf_init;
		break;

      case 'b':
	if(A_flag){
	int_arr = va_arg(args, int *);
	arr_size = va_arg(args, int);
	printf("{");
	  for(int i=0; i<arr_size;i++){
	    chars_printed += print_int(int_arr[i], 2, digit,0);
	    if(i!=arr_size-1) printf(",");
	  }
	printf("}");
	}
	else{ 
	  int_value = va_arg(args, int);
	  chars_printed += print_int(int_value, 2, digit,0);
	}
	state = st_printf_init;
	break;

      case 'o':
	if(A_flag){
	int_arr = va_arg(args, int *);
	arr_size = va_arg(args, int);
	printf("{");
	  for(int i=0; i<arr_size;i++){
	    chars_printed += print_int(int_arr[i], 8, digit,0);
	    if(i!=arr_size-1) printf(",");
	  }
	printf("}");
	}
	else{ 
	  int_value = va_arg(args, int);
	  chars_printed += print_int(int_value, 8, digit,0);
	}
	state = st_printf_init;
	break;

      case 'u':
	if(A_flag){
	int_arr = va_arg(args, int *);
	arr_size = va_arg(args, int);
	printf("{");
	  for(int i=0; i<arr_size;i++){
	    chars_printed += print_int(int_arr[i], 10, digit,0);
	    if(i!=arr_size-1) printf(",");
	  }
	printf("}");
	}
	else{ 
	  int_value = va_arg(args, int);
	  chars_printed += print_int(int_value, 10, digit,0);
	}
	state = st_printf_init;
	break;
	
      case 'x':
	if(A_flag){
	int_arr = va_arg(args, int *);
	arr_size = va_arg(args, int);
	printf("{");
	  for(int i=0; i<arr_size;i++){
	    chars_printed += print_int(int_arr[i],16, digit,0);
	    if(i!=arr_size-1) printf(",");
	  }
	printf("}");
	}
	else{ 
	  int_value = va_arg(args, int);
	  chars_printed += print_int(int_value, 16, digit,0);
	}
	state = st_printf_init;
	break;

      case 'X':
	if(A_flag){
	int_arr = va_arg(args, int *);
	arr_size = va_arg(args, int);
	printf("{");
	  for(int i=0; i<arr_size;i++){
	    chars_printed += print_int(int_arr[i], 16, DIGIT,0);
	    if(i!=arr_size-1) printf(",");
	  }
	printf("}");
	}
	else{ 
	  int_value = va_arg(args, int);
	  chars_printed += print_int(int_value, 16, DIGIT,0);
	}
	state = st_printf_init;
	break;

      case 's':
	if(A_flag){
		string_arr = va_arg(args, char **);
		arr_size = va_arg(args, int);
		printf("{");
	  	for(int i=0; i<arr_size;i++){
	    	string_value = string_arr[i];
	    	putchar('\"');
	    	while(*string_value){
	      	chars_printed++;
	      	putchar(*string_value);
	      	string_value++;
	    	}
	    	putchar('\"');
	    	if(i!=arr_size-1) putchar(',');
	  	}
		putchar('}');
	}
	else{ 
      		printf("in\n");		
	  		string_value = va_arg(args, char *);
	  		if(width>0){
	  			value_length=get_value_length_string(string_value);
	  			if(left_flag)
	  				fill_with_char(width,value_length,char_to_add);
	  		}
	  		while(*string_value){
				chars_printed++;
				putchar(*string_value);
				string_value++;
	  		}
	  		if(width && !left_flag){
	  			fill_with_char(width,value_length,char_to_add);
	  			putchar('#');
	  		}
	}
	state = st_printf_init;
	break;

      case 'c':
       if(A_flag){
	char_arr = va_arg(args, char *);
	arr_size = va_arg(args, int);
	printf("{");
	  for(int i=0; i<arr_size;i++){
	    putchar('\'');
	    char_value = char_arr[i];
	    putchar(char_value);
	    ++chars_printed;
	    printf("\'");
	    if(i!=arr_size-1) printf(",");
	  }
	printf("}");
	}
	else{ 
	  char_value = (char)va_arg(args, int);
	  putchar(char_value);
	  ++chars_printed;
	}
	state = st_printf_init;
	break;

      default:
      	if('0'==*fs && !zero_flag && !width) zero_flag=1;
      	else if('0'<=*fs && *fs<='9'){
      		width=(width*10)+(*fs-'0');
      	}else{
      		toy_printf("Unhandled format %%%c...\n", *fs);
			exit(-1);
      	}
      }
      break;

    default:
      toy_printf("toy_printf: Unknown state -- %d\n", (int)state);
      exit(-1);
    }
  }

  va_end(args);

  return chars_printed;
}


