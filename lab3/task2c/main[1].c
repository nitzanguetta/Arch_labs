#include "toy_stdio.h"


int main(int argc, char *argv[]) {
int integers_array[] = {1,12,123};

  
int x= toy_printf("%x,%X\n",-1,-1);
int dl= toy_printf("%08d\n", 12);
int s= toy_printf("%s\n","too many tests!");
int spr= toy_printf("%10s\n", "abc");
int spl= toy_printf("%-10s\n", "abc");
int A= toy_printf("%Ad\n", integers_array,3);

toy_printf("x= %d\n", x);
toy_printf("dl= %d\n", dl); 
toy_printf("s= %d\n", s); 
toy_printf("spr= %d\n", spr); 
toy_printf("spl= %d\n", spl); 
toy_printf("A= %d\n", A); 



}
 
