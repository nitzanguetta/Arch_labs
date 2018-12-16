#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>



int unit_size = 1;
char file_name[100] = {0};

void setFileName(){
	printf("enter file name:\n");
	fgets(file_name,100,stdin);
	file_name[strlen(file_name)-1] = 0;
}

void setUnitSize(){
	int new_size;
	printf("enter unit size:\n");
	new_size = fgetc(stdin) - '0';
	fgetc(stdin);
	if (size==1 || size==2 || size==4){
		unit_size = new_size;
	}
	else
		fprintf(stderr,"invalid size.\n");
}

void fileDisplay(){
	if (!file_name){
		fprintf(stderr,"no file entered.\n");
		return
	}else{
		FILE * input = fopen(file_name,"rb");
		if(!input){
			printf(stderr,"could not open file.\n");
			return;
		}else{
			char buff [16] = {0};
			unsigned int location;
			int length;
			int i, j;
			unsigned char* input_read = malloc(unit_size*length);
			printf("Please enter <location> <length>\n");
			fgets(buff,16,stdin);
			sscanf(buff,"%x %d",&location,&length);
			fseek(input,location,SEEK_SET);
			fread(input_read,unit_size,length,input);
			fclose(input);
                        switch(unit_size){
                        	case 1:
                        		printf("Hexadecimal Representation:\n");
                                for (i=0; i<length*unit_size-1; i++)
                                        printf("%02hX ", input_read[i]);
                                printf("%02hX\n", input_read[i]);
                                printf("Decimal Representation:\n");
                                for (i=0; i<length*unit_size-1; i++)
                                        printf("%d ", input_read[i]);
                                printf("%d\n", input_read[i]);
                                break;
                            case 2:
                                printf("Hexadecimal Representation:\n");
                                for (i=0; i<length*unit_size-2; i+=2)
                                        printf("%04hX ", input_read[i] | input_read[i+1] << 8);
                                printf("%04hX\n", input_read[i] | input_read[i+1] << 8);
                                printf("Decimal Representation:\n");
                                for (i=0; i<length*unit_size-2; i+=2)
                                        printf("%d ", input_read[i] | input_read[i+1] << 8);
                                printf("%d\n", input_read[i] | input_read[i+1] << 8);
                                break;
                            case 4:
                                printf("Hexadecimal Representation:\n");
                                for (i=0; i<length*unit_size; i+=4){
                                    for (j=4; j>0; j--)
                                        printf("%02hX", input_read[i+j-1]);
                                    printf(" ");
                                }
                                printf("\n");
                                printf("Decimal Representation:\n");
                                for (i=0; i<length*unit_size-4; i+=4)
                                        printf("%hu ", input_read[i] | input_read[i+1] << 8 | input_read[i+2] << 16 | input_read[i+3] << 24);
                                printf("%hu\n", input_read[i] | input_read[i+1] << 8 | input_read[i+2] << 16 | input_read[i+3] << 24);
                                break;
                        }
			free(input_read);
		}			
	}
}

void fileModify(){
	if (!file_name){
		fprintf(stderr,"no file entered.\n");
		return;
	}else{
		FILE * output = fopen(file_name,"r+b");
		if(!output){
			printf(stderr,"could not open file.\n");
			return;
		}else{
			char buff [16] = {0};
			unsigned int location, val;
			fseek(output,0,SEEK_END);
			size_t fileSize = ftell(output);
			printf("Please enter <location> <val>\n");
			fgets(buff,16,stdin);
			sscanf(buff,"%x %x",&location,&val);
			if (fileSize>=location+unit_size){
				fseek(output,location,SEEK_SET);
				fwrite((char*)&val,1,unit_size,output);
				fclose(output);
				printf("Modified a unit of size %d in %#x\n",unit_size,location);
			}
			else
				fprintf(stderr,"request exceeds file size.\n");			
		}	
	}		
}

void copyFromFile(){
	if (!file_name){
		fprintf(stderr,"no file entered.\n");
		return;
	}else{
		char buff [124] = {0};
		unsigned int src_offset,dst_offset, length;
		char src_file[100] = {0};
		printf("Please enter <src_file> <src_offset> <dst_offset> <length>\n");
		fgets(buff,124,stdin);
		sscanf(buff,"%s %x %x %d",src_file,&src_offset,&dst_offset,&length);
		FILE * input = fopen(src_file,"r+b");
		if(!input){
			printf(stderr,"could not open file.\n");
			return
		}else{
			FILE * output = fopen(file_name,"r+b");
			if(!output){
				fprintf(stderr,"no file entered or could not open file.\n");
				return;
			}else{
				fseek(input,0,SEEK_END);
				size_t file_size = ftell(input);
				if (file_size>=src_offset+unit_size*length){
					unsigned char* input_read = malloc(unit_size*length);
					fseek(input,src_offset,SEEK_SET);
					fread(input_read,unit_size,length,input);
					fclose(input);
					file_size = ftell(output);

					if (file_size>=dst_offset+unit_size*length){
						fseek(output,dst_offset,SEEK_SET);
						fwrite(input_read,1,unit_size*length,output);
						fclose(output);
						printf("Copied %d bytes FROM %s at %x TO %s at %x",length,src_file,src_offset,file_name,dst_offset);	
					}
					else
						fprintf(stderr,"request exceeds file size.\n");
					free(input_read);
				}
				else
					fprintf(stderr,"request exceeds file size.\n");
			}
		}
	}		
}

void quit(){
	exit(0);
}

struct func_struct {
	char *name;
	void (*func_ptr)();
};

int main(int argc, char **argv){
	struct func_struct menu[] = {{"Set File Name", setFileName}, {"Set Unit Size", setUnitSize}, {"File Display", fileDisplay}, {"File modify", fileModify},{"Copy From File",copyFromFile}, {"Quit", quit}, {NULL, NULL}};
	int i, input, bound = 7;
	char input_array[2];
	while (1){
		printf("\nChoose action:\n");

		for (i=0; menu[i].name!=NULL; i++)
			printf("%d-%s\n", i+1, menu[i].name);
		fread(input_array,1,2,stdin);
		input = atoi(input_array);
		
		if (input-1>=0 && input-1<bound){
			menu[input-1].func_ptr();
		}
		else
			fprintf(stderr,"invalid option.\n");	
	}
}
