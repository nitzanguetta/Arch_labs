#include "lab4_util.h"

#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define STDOUT 1
#define BUF_SIZE 1024
#define SYS_GETDENTS 78
#define DIR_TYPE 4
#define ERROR_CODE 0x55

struct linux_dirent {
    unsigned long  d_ino;
    long  d_off;    
    unsigned short d_reclen;  
    char           d_name[BUF_SIZE]; 
    char           pad;      
    char           d_type; 
};

extern int system_call();

int print_dir_rec(char path[],char name[], char command []){
  int nread;
  char buf[BUF_SIZE];
  struct linux_dirent *d;
  int bpos;
  char d_type;
  int fd;
  int i;
  int path_size= simple_strlen(path);
  int command_size= simple_strlen(command);
    
  fd = system_call(SYS_OPEN,path,0,0x0777);
           if (fd == -1)
               return ERROR_CODE;

        for(;;){
               nread = system_call(SYS_GETDENTS,fd,buf,BUF_SIZE);
               if (nread == -1)
                   return ERROR_CODE;
               if (nread == 0)
                   break;
           
               for (bpos = 0; bpos < nread;) {
                   d = (struct linux_dirent *) (buf + bpos);
                   d_type = *(buf + bpos + d->d_reclen - 1);
                   if(d->d_name[0] != '.'){

                   		int d_name_size= simple_strlen(d->d_name);
                      	char buf2[path_size+d_name_size+2];
                      	for(i=0;i<path_size;i++)
                        	buf2[i]=path[i];
                        buf2[path_size]='/';
                        for(i=0;i<d_name_size;i++)
                          buf2[path_size+1+i]=d->d_name[i];   
                        buf2[path_size+d_name_size+1]='\0';

                        if(simple_strcmp(name,d->d_name)==0 || simple_strcmp(name,"-")==0){
                          if(simple_strcmp(command,"-")==0){
                          system_call(SYS_WRITE,STDOUT,buf2,simple_strlen(buf2));
                          system_call(SYS_WRITE,STDOUT,"\n",simple_strlen("\n")); 
                          }else{
                              int buf2_size=simple_strlen(buf2);
                              char buf[command_size+buf2_size+2];
                              for(i=0;i<command_size;i++)
                                buf[i]=command[i];
                              buf[command_size]=' ';
                              for(i=0;i<buf2_size;i++)
                                buf[command_size+1+i]=buf2[i];   
                              buf[command_size+buf2_size+1]='\0';

                              simple_system(buf);
                          }
                        } 
                        if(d_type==DIR_TYPE)
                   		    print_dir_rec(buf2,name,command); 
                   }
                    bpos += d->d_reclen;
               }
           }
           system_call(SYS_CLOSE,fd);
           return 0;
}

int length_concat(int arr_len,char* arr[]){
  int acc=arr_len-1, i;
    for(i=3;i<arr_len;i++){
      acc=acc+simple_strlen(arr[i]);
    }
    return acc+1;
}

int main (int argc , char* argv[]){
  if (argc>1){
    if (simple_strcmp(argv[1],"-e")==0){
      int i,s;
      int j=0;
      char buf[length_concat(argc-1,argv)];

      for(i=3;i<argc;i++){
        for(s=0;s<simple_strlen(argv[i]);s++){
          buf[j]=argv[i][s];
          j++;
        }
        buf[j]=' ';
        j++;
      }
       buf[j]='\0';
      print_dir_rec(".",argv[2],buf);

    }else if(simple_strcmp(argv[1],"-n")==0 && argc==3)
      print_dir_rec(".",argv[2],"-");
  }else if (argc == 1) {  
  	system_call(SYS_WRITE,STDOUT,".",simple_strlen("."));
  	system_call(SYS_WRITE,STDOUT,"\n",simple_strlen("\n"));
	  print_dir_rec(".","-","-");
  }
  	return 0;
}


