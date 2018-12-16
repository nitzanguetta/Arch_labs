#include "lab4_util.h"

#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_LSEEK 8
#define FILE 0x1420
#define SIHRA_LOC 0x1015



extern int system_call();


int main (int argc , char* argv[], char* envp[])
{
  char * override_spaces ="      ";
  int override_spaces_location= SIHRA_LOC+simple_strlen(argv[2]);

  int desc=system_call(SYS_OPEN,argv[1],0x002,0x0777);
  
  if (desc<0)
      return 0x55;
  
  if (system_call(SYS_LSEEK,desc,SIHRA_LOC,0) < 0)
      return 0x55;
  
  if (system_call(SYS_WRITE,desc,override_spaces,simple_strlen(override_spaces))< 0)
      return 0x55;
 
  if (system_call(SYS_LSEEK,desc,SIHRA_LOC,0) < 0 )
      return 0x55;
  
  if (system_call(SYS_WRITE,desc,argv[2],simple_strlen(argv[2])) < 0)
      return 0x55;
  
  if (system_call(SYS_LSEEK,desc,override_spaces_location,0)< 0)
      return 0x55;
  
  if(system_call(SYS_WRITE,desc,".",simple_strlen(".")) < 0)
      return 0x55;

  if(system_call(SYS_CLOSE,desc) < 0)
      return 0x55;

  return 0;
}
