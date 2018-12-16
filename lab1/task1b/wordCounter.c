#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
  int input, last=0, wordcounter=0,charcounter=0,longestcounter=0, currlen=0;
  int wflag=0, cflag=0, lflag=0;
  
  if (argc==1) wflag=1;
  else
      for (int i=1;i<argc;i++){
          if(strcmp(argv[i],"-w")==0) wflag=1;
          else if(strcmp(argv[i],"-c")==0) cflag=1;
          else if(strcmp(argv[i],"-l")==0) lflag=1;
          else {
              printf("invalid parameter %s\n",argv[i]); 
              return 1;
          }
      }

  while((input=fgetc(stdin))!=EOF){
      if (input>32&& input!=127){   //characters
          last=1;
          if (cflag) charcounter++;
          if(lflag) currlen++;
          
      }
      else if(last){                //spaces at end of word
          if (lflag){
              if(longestcounter<currlen)
                  longestcounter=currlen;
              currlen=0;
          }
          if (wflag) wordcounter++;
          last=0;
      }
      
  }
  if(wflag) printf("%i \n",wordcounter);
  if(cflag) printf("%i \n",charcounter);
  if(lflag) printf("%i \n",longestcounter);
  return 0;
}
 
