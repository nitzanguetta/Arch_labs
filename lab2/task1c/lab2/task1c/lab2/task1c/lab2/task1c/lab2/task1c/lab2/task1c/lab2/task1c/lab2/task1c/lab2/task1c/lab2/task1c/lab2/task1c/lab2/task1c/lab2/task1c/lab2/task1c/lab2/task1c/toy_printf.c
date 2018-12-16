/* toy-io.c
 * Limited versions of printf and scanf
 *
 * Programmer: Mayer Goldberg, 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

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
//-------------------------------------------------------
int print_array(char char_case, void** arr, int arr_size){
    int chars_printed=2;
    char char_arr[3];
    char_arr[0]='%';
    char_arr[1]=char_case;
    char_arr[2]='\0';
  putchar('{');
  for(int i=0; i<arr_size;i++){
    if(char_case=='s'){
        chars_printed += toy_printf(char_arr, (char*)arr[i]);
    }else if(char_case=='c'){
        chars_printed += toy_printf(char_arr, (char)arr[i]);
    }else if (char_case=='d' || char_case=='b' || char_case=='o' || char_case=='x' || char_case=='X'){
        chars_printed += toy_printf(char_arr, (int)arr[i]);
      }
    if(i!=arr_size-1){
      putchar(',');
      chars_printed++;
    } 
  }
  putchar('}');
  return chars_printed;
}
//-------------------------------------------------------
int print_int_array(int *int_arr,int arr_size,int radix,const char *digit,int signed_format){
  int chars_printed=2;
  putchar('{');
  for(int i=0; i<arr_size;i++){
    chars_printed += print_int(int_arr[i], radix, digit,signed_format);
    if(i!=arr_size-1){
      putchar(',');
      chars_printed++;
    } 
  }
  putchar('}');
  return chars_printed;
}

int print_string_array(char **string_arr,int arr_size){
  int chars_printed=2;
  char *string_value;
  putchar('{');
  for(int i=0; i<arr_size;i++){
    string_value = string_arr[i];
      putchar('\"');
      while(*string_value){
          chars_printed++;
          putchar(*string_value);
          string_value++;
      }
      putchar('\"');
      chars_printed+=2;
      if(i!=arr_size-1){
      putchar(',');
      chars_printed++;
    } 
  }
  putchar('}');
  return chars_printed;
}

int print_char_array(char *char_arr,int arr_size){
  int chars_printed=2;
  putchar('{');
  for(int i=0; i<arr_size;i++){
      putchar('\'');
      putchar(char_arr[i]);
      ++chars_printed;
      putchar('\'');
      chars_printed+=2;
      if(i!=arr_size-1){
      putchar(',');
      chars_printed++;
    } 
  }
  putchar('}');
  return chars_printed;
}

//-----------------------------------------------------------

/* SUPPORTED:
 *   %b, %d, %o, %x, %X -- 
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */

int toy_printf(char *fs, ...) {
  int* int_arr;
  char** string_arr;
  char* char_arr;
  int arr_size;
  int A_flag=0;
  void** void_arr;

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
          void_arr = va_arg(args, int *);
          arr_size = va_arg(args, int);
          print_array(*++fs,void_arr,arr_size);
          break;

        case 'd':
          //if(A_flag){
            //int_arr = va_arg(args, int *);
            //arr_size = va_arg(args, int);
           //chars_printed +=print_int_array(int_arr,arr_size,10,digit,1);
          //}
          //else{

            int_value = va_arg(args, int);
            printf("%i----\n",int_value );
            chars_printed += print_int(int_value, 10, digit,1);
           // }
            state = st_printf_init;
            break;

        case 'b':
          //if(A_flag){
          //   int_arr = va_arg(args, int *);
          //   arr_size = va_arg(args, int);
          //   chars_printed +=print_int_array(int_arr,arr_size,2,digit,0);
          //   }
          //else{ 
            int_value = va_arg(args, int);
            chars_printed += print_int(int_value, 2, digit,0);
            //}
          state = st_printf_init;
          break;

        case 'o':
          //if(A_flag){
            // int_arr = va_arg(args, int *);
            // arr_size = va_arg(args, int);
            // chars_printed +=print_int_array(int_arr,arr_size,8,digit,0);
            // }
          //else{ 
            int_value = va_arg(args, int);
            chars_printed += print_int(int_value, 8, digit,0);
            //}
          state = st_printf_init;
          break;

        case 'u':
          // if(A_flag){
          //   int_arr = va_arg(args, int *);
          //   arr_size = va_arg(args, int);
          //   chars_printed +=print_int_array(int_arr,arr_size,10,digit,0);
          // }
          //else{ 
            int_value = va_arg(args, int);
            chars_printed += print_int(int_value, 10, digit,0);
          //}
          state = st_printf_init;
          break;
  
        case 'x':
          // if(A_flag){
          //   int_arr = va_arg(args, int *);
          //   arr_size = va_arg(args, int);
          //   chars_printed +=print_int_array(int_arr,arr_size,16,digit,0);
          //   }
          //else{ 
            int_value = va_arg(args, int);
            chars_printed += print_int(int_value, 16, digit,0);
          //}
          state = st_printf_init;
          break;

        case 'X':
          // if(A_flag){
          //   int_arr = va_arg(args, int *);
          //   arr_size = va_arg(args, int);
          //   chars_printed +=print_int_array(int_arr,arr_size,16,DIGIT,0);
          // }
          //else{ 
            int_value = va_arg(args, int);
            chars_printed += print_int(int_value, 16, DIGIT,0);
          //}
          state = st_printf_init;
          break;

        case 's':
          // if(A_flag){
          //   string_arr = va_arg(args, char **);
          //   arr_size = va_arg(args, int);
          //   chars_printed+=print_string_array(string_arr,arr_size);
          // }
          //else{   
            string_value = va_arg(args, char *);
            while(*string_value){
              chars_printed++;
              putchar(*string_value);
              string_value++;
            }
          //}
          state = st_printf_init;
          break;

        case 'c':
          // if(A_flag){
          //   char_arr = va_arg(args, char *);
          //   arr_size = va_arg(args, int);
          //   chars_printed+=print_char_array(char_arr,arr_size);
          // }
          // else{ 
            char_value = (char)va_arg(args, int);
            putchar(char_value);
            ++chars_printed;
          //}
          state = st_printf_init;
          break;

      default:
        toy_printf("Unhandled format %%%c...\n", *fs);
        exit(-1);  
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


