#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
  int input, last=0, wordcounter=0,charcounter=0,longestcounter=0, currlen=0, linecounter=0;
  int wflag=0, cflag=0, lflag=0, nflag=0, iflag=0;
  FILE * stream;
  
  if (argc==1) wflag=1;
  else
      for (int i=1;i<argc;i++){
          if(strcmp(argv[i],"-w")==0) wflag=1;
          else if(strcmp(argv[i],"-c")==0) cflag=1;
          else if(strcmp(argv[i],"-l")==0) lflag=1;
          else if(strcmp(argv[i],"-n")==0) nflag=1;
          else if(strcmp(argv[i],"-i")==0){ iflag=1;
              if(argc-1==i){
                  printf("no file was entered\n"); 
                  return 1;
              }
              else stream=fopen(argv[++i],"r");
          }
          else {
              printf("invalid parameter %s\n",argv[i]); 
              return 1;
          }
      }

  if(!stream && iflag){
      printf("file didnt exists\n"); 
      return 1;
  }
  else if (!stream) stream=stdin;

  while((input=fgetc(stream))!=EOF){
      if (nflag && input=='\n') linecounter++;
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
  fclose(stream);
  
  if(wflag) printf("%i \n",wordcounter);
  if(cflag) printf("%i \n",charcounter);
  if(lflag) printf("%i \n",longestcounter);
  if(nflag) printf("%i \n",linecounter);
  
  return 0;
}
 
