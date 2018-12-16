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
  st_printf_octal3,
  st_printf_exit
};

#define MAX_NUMBER_LENGTH 64
#define is_octal_char(ch) ('0' <= (ch) && (ch) <= '7')

//-------------------------TYPEDEF_STRUCT----------------------------------

typedef struct state_result {
	int printed_chars;
	enum printf_state new_state;
} state_result;

typedef struct state_args{
    char* fs;
    state_result* result;
    int left_flag;
	int zero_flag;
	int A_flag;
	int width;
	int value_length;
	char char_to_add;

} state_args;

//----------------------INIT_STATES_FUNC------------------------------------

void init_flags(state_args* state_args){
	state_args->left_flag=0;
	state_args->zero_flag=0;
	state_args->A_flag=0;
	state_args->width=0;
	state_args->value_length=0;
	state_args->char_to_add=' ';
}

void init_state_args(state_args* state_args,char* fs){
	state_args->fs=fs;
	init_flags(state_args);
}
void init_state_result(state_result* result, enum printf_state state){
	result->printed_chars=0;
	result->new_state=state;
}

//-------------------------------------------------------------

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


void fill_with_char(int width, int value_length, char char_to_add){
	for(int i=0;i<(width-value_length);i++)
		putchar(char_to_add);
}

//--------------------------------------------------------------------------

state_result d_state_result(va_list args, state_args* state){
	int* int_arr;
	int arr_size;
	int int_value = 0;
        state_result result;
         result.new_state=st_printf_percent;
    result.printed_chars=0;

	if(state->A_flag){
		int_arr = va_arg(args, int *);
		arr_size = va_arg(args, int);
		result.printed_chars += print_int_array(int_arr,arr_size,10,digit,1);
	}
	else{
		int_value = va_arg(args, int);
		if(int_value<0){
			int_value=int_value*(-1);
			putchar('-');
		} 
		if(state->width){
                    
			state->value_length=get_value_length(int_value);
                        result.printed_chars+=state->width-state->value_length;
			if(state->zero_flag)
				state->char_to_add='0';
			if(state->left_flag)
				fill_with_char(state->width,state->value_length,state->char_to_add);
			}
			result.printed_chars += print_int(int_value, 10, digit,1);
		  	if(!state->left_flag&& state->width){
		  		fill_with_char(state->width,state->value_length,state->char_to_add);
		  		putchar('#');
		  	}
		}
	init_flags(state);
	result.new_state = st_printf_init;
	return result;
}

state_result b_state_result(va_list args, state_args* state){
	int* int_arr;
	int arr_size;
	int int_value = 0;
        state_result result;
         result.new_state=st_printf_percent;
    result.printed_chars=0;

	if(state->A_flag){
		int_arr = va_arg(args, int *);
		arr_size = va_arg(args, int);
		result.printed_chars+=print_int_array(int_arr,arr_size,2,digit,0);
	}
	else{ 
		int_value = va_arg(args, int);
		result.printed_chars+= print_int(int_value, 2, digit,0);
	}
	init_flags(state);
	result.new_state = st_printf_init;
	return result;
}

state_result o_state_result(va_list args, state_args* state){
	int* int_arr;
	int arr_size;
	int int_value = 0;
        state_result result;
         result.new_state=st_printf_percent;
    result.printed_chars=0;

	if(state->A_flag){
		int_arr = va_arg(args, int *);
		arr_size = va_arg(args, int);
		result.printed_chars +=print_int_array(int_arr,arr_size,8,digit,0);
	}
	else{ 
		int_value = va_arg(args, int);
		result.printed_chars += print_int(int_value, 8, digit,0);
	}
	init_flags(state);
	result.new_state = st_printf_init;
	return result;
}
state_result x_state_result(va_list args, state_args* state){
	int* int_arr;
	int arr_size;
	int int_value = 0;
        state_result result;
         result.new_state=st_printf_percent;
    result.printed_chars=0;

	if(state->A_flag){
		int_arr = va_arg(args, int *);
		arr_size = va_arg(args, int);
		result.printed_chars +=print_int_array(int_arr,arr_size,16,digit,0);
	}
	else{ 
		int_value = va_arg(args, int);
		result.printed_chars += print_int(int_value, 16, digit,0);
	}
	init_flags(state);
	result.new_state = st_printf_init;
	return result;
}

state_result X_state_result(va_list args, state_args* state){
	int* int_arr;
	int arr_size;
	int int_value = 0;
        state_result result;
         result.new_state=st_printf_percent;
    result.printed_chars=0;

	if(state->A_flag){
		int_arr = va_arg(args, int *);
		arr_size = va_arg(args, int);
		result.printed_chars +=print_int_array(int_arr,arr_size,16,DIGIT,0);
	}
	else{ 
		int_value = va_arg(args, int);
		result.printed_chars += print_int(int_value, 16, DIGIT,0);
	}
	init_flags(state);
	result.new_state = st_printf_init;
	return result;
}
state_result u_state_result(va_list args, state_args* state){
	int* int_arr;
	int arr_size;
	int int_value = 0;
        state_result result;
         result.new_state=st_printf_percent;
    result.printed_chars=0;


	if(state->A_flag){
		int_arr = va_arg(args, int *);
		arr_size = va_arg(args, int);
		result.printed_chars+=print_int_array(int_arr,arr_size,10,digit,0);
	}
	else{ 
		int_value = va_arg(args, int);
		result.printed_chars += print_int(int_value, 10, digit,0);
	}
	init_flags(state);
	result.new_state = st_printf_init;
	return result;
}

state_result c_state_result(va_list args, state_args* state){
	char* char_arr;
	int arr_size;
	char char_value;
        state_result result;
         result.new_state=st_printf_percent;
    result.printed_chars=0;

	if(state->A_flag){
		char_arr = va_arg(args, char *);
		arr_size = va_arg(args, int);
		result.printed_chars+=print_char_array(char_arr,arr_size);
	}
	else{ 
		char_value = (char)va_arg(args, int);
		putchar(char_value);
		++result.printed_chars;
		}
		init_flags(state);
		result.new_state = st_printf_init;
	return result;
}

state_result s_state_result(va_list args, state_args* state){
	char** string_arr;
	int arr_size;
	char *string_value;
        state_result result;
 result.new_state=st_printf_percent;
    result.printed_chars=0;

	if(state->A_flag){
		string_arr = va_arg(args, char **);
		arr_size = va_arg(args, int);
		result.printed_chars +=print_string_array(string_arr,arr_size);
	}
	else{ 	
		string_value = va_arg(args, char *);
		if(state->width>0){
		  	state->value_length=get_value_length_string(string_value);
                        result.printed_chars+=state->width-state->value_length;
		  	if(state->left_flag)
		  		fill_with_char(state->width,state->value_length,state->char_to_add);
		  	}
		while(*string_value){
			result.printed_chars++;
			putchar(*string_value);
			string_value++;
		}
		if(state->width && !state->left_flag){
		  	fill_with_char(state->width,state->value_length,state->char_to_add);
		  	putchar('#');
		}
	}
	init_flags(state);
	result.new_state = st_printf_init;
	return result;
}
state_result A_state_result(va_list args, state_args* state){
    state_result result;
     result.new_state=st_printf_percent;
    result.printed_chars=0;
	state->A_flag=1;
	return result;
}
state_result left_state_result(va_list args, state_args* state){
        state_result result;
        result.new_state=st_printf_percent;
        result.printed_chars=0;
	state->left_flag=1;
	return result;
}

state_result precent_state_result(va_list args, state_args* state){
    state_result result;
    result.new_state=st_printf_percent;
    result.printed_chars=0;

		putchar('%');
		++result.printed_chars;
		result.new_state = st_printf_init;

	return result;
}
state_result precent_init_state_result(va_list args, state_args* state){
    state_result result;
    result.new_state=st_printf_percent;
    result.printed_chars=0;

		result.new_state = st_printf_percent;

	return result;
}

state_result default_state_init_args(va_list args, state_args* state){
    state_result result;
    result.new_state=st_printf_init;
    result.printed_chars=0;
		putchar(*state->fs);
		++result.printed_chars;
	return result;
}
state_result default_state_args(va_list args, state_args* state){
    state_result result;
    result.new_state=st_printf_percent;
    result.printed_chars=0;

		if('0'==*state->fs && !state->zero_flag && !state->width) state->zero_flag=1;
		else if('0'<=*state->fs && *state->fs<='9'){
	    	state->width=(state->width*10)+(*state->fs-'0');
		}else{
	    	toy_printf("Unhandled format %%%c...\n", *state->fs);
			exit(-1);
		}
	return result;
}


//--------------------------------------------------------------------------
 
state_result init_state_handler(va_list args, state_args* state){
    state_result result;
	state_result (*handler_init_result[128])(va_list, state_args*);
	for(int i=0;i<128;i++){
		handler_init_result[i]=default_state_init_args;
	}
	handler_init_result['%']=precent_init_state_result;
	result=handler_init_result[(unsigned char)*state->fs](args,state);
	return result;
}


state_result percent_state_handler(va_list args, state_args* state){
    state_result (*handler_precent_result[128])(va_list, state_args*);
	state_result result;
	
	for(int i=0;i<128;i++){
		handler_precent_result[i]=default_state_args;
	}

	handler_precent_result['d']=d_state_result;
	handler_precent_result['b']=b_state_result;
	handler_precent_result['o']=o_state_result;
	handler_precent_result['x']=x_state_result;
	handler_precent_result['X']=X_state_result;
	handler_precent_result['u']=u_state_result;
	handler_precent_result['c']=c_state_result;
	handler_precent_result['s']=s_state_result;
	handler_precent_result['%']=precent_state_result;
	handler_precent_result['A']=A_state_result;
	handler_precent_result['-']=left_state_result;
	result=handler_precent_result[(unsigned char)*state->fs](args,state);
	return result;
}

state_result default_state_handler(va_list args, state_args* state){
    state_result result;
    result.new_state=st_printf_init;
    result.printed_chars=0;
	toy_printf("toy_printf: Unknown state -- %d\n", (int)result.new_state);
	result.new_state=st_printf_exit;
	exit(-1);
	return result;
}


//-----------------------------------------------------------

/* SUPPORTED:
 *   %b, %d, %o, %x, %X -- 
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */

int toy_printf(char *fs, ...) {
  	state_result (*handler_state_args[3])(va_list, state_args*)={init_state_handler,percent_state_handler,default_state_handler};
  	state_args* state_arg= malloc(1*sizeof(state_args));
  	state_result result;
        result.new_state=st_printf_init;
        result.printed_chars=0;
  	int printed_chars = 0;

  	va_list args;
	va_start(args, fs);
	//init_state_result(result,st_printf_init);
	init_state_args(state_arg,fs);

	for (; result.new_state != st_printf_exit &&*state_arg->fs != '\0'; ++state_arg->fs) {
   		result=handler_state_args[result.new_state](args,state_arg);
                printed_chars+=result.printed_chars;
	}

        
 	free(state_arg);
 	va_end(args);

  	return printed_chars;
}


