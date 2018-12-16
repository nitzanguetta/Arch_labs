#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <elf.h>
#include <sys/stat.h>
#include <sys/mman.h>


int Currentfd = -1;
struct stat fd_stat;
char* map_start = NULL;


void examineELFFile(){
	char buff [100] = {0};
   	Elf64_Ehdr *header;
	printf("Please enter <file name>\n");
	fgets(buff,100,stdin);
	buff[strlen(buff)-1]=0;
	if (Currentfd!=-1)
		close(Currentfd);
   if((Currentfd = open(buff, O_RDWR)) < 0 ) {
    	exit(-1);
   }
   if(fstat(Currentfd, &fd_stat) != 0 ) {
	    close(Currentfd);
	    exit(-1);
   }
   if ((map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, Currentfd, 0)) == MAP_FAILED ) {
		munmap(map_start, fd_stat.st_size);
		close(Currentfd);
		exit(-4);
   }
   header = (Elf64_Ehdr *) map_start;
   char magic_bit_1 = header->e_ident[1];
   char magic_bit2 = header->e_ident[2];
   char magic_bit2 = header->e_ident[3];
   printf("\nELF header:\nMagic (bytes 1, 2 and 3 only):\t\t%c %c %c\n",magic_bit_1,magic_bit2,magic_bit2);
   if (magic_bit_1=='E' && magic_bit2=='L' && magic_bit2=='F'){
   		if(header->e_ident[EI_DATA]==1)
	   		printf("Data:\t\t\t\t\t2's complement, little endian\n");
	   	else if (header->e_ident[EI_DATA]==2)
	   		printf("Data:\t\t\t\t\t2's complement, big endian\n");
	   	else
	   		printf("Data:\t\t\t\t\tinvalid data encoding\n");
	   printf("Entry point:\t\t\t\t%#x\n",header->e_entry);
	   printf("Start of section headers:\t\t%d (bytes into file)\n",header->e_shoff);
	   printf("Number of section headers:\t\t%d\n",header->e_shnum);
	   printf("Size of section headers:\t\t%d\n",header->e_shentsize);
	   printf("Start of program headers:\t\t%d (bytes into file)\n",header->e_phoff);
	   printf("Number of program headers:\t\t%d\n",header->e_phnum);
	   printf("Size of program headers:\t\t%d\n",header->e_phentsize);
	}
	else
		fprintf(stderr,"not an ELF file");
}

void printSectionNames(){
	if (Currentfd!=-1){
		Elf64_Ehdr *header = (Elf64_Ehdr *) map_start;
		void *addr_abs = map_start+ header->e_shoff;
		Elf64_Shdr *sectoin_start = (Elf64_Shdr *) addr_abs;
		addr_abs = sectoin_start+header->e_shstrndx;
		char  *str_table_s = (char*) (map_start +((Elf64_Shdr *) addr_abs)->sh_offset);
		int i;
		printf("\nSection Headers:\n[index]\t\tsection_name\t\tsection_address\t\tsection_offset\t\tsection_size\t\tsection_type\n");
		for (i=0; i<header->e_shnum; i++){
	        char *type; 
	        switch (sectoin_start->sh_type){
	            case 0:
	                type="NULL";
	                break;
	            case 1:
	                type="PROGBITS";
	                break;
	            case 2:
	                type="SYMTAB";
	                break;
	            case 3:
	                type="STRTAB";
	                break;
	            case 4:
	                type="RELA";
	                break;
	            case 5:
	                type="HASH";
	                break;
	            case 6:
	                type="DYNAMIC";
	                break;
	            case 7:
	                type="NOTE";
	                break;
	            case 8:
	                type="NOBITS";
	                break;
	            case 9:
	                type="REL";
	                break;   
	            case 10:
	                type="SHLIB";
	                break;   
	            case 11:
	                type="DYNSYM";
	                break;
	            case 0x60000000:
	            	type="LOOS";
	            	break;
	            case 0x6fffffff:
	            	type="HIOS";
	            	break;
	            case  0x70000000:
	                type="LOPROC";
	                break;
	            case 0x7fffffff:
	                type="HIPROC";
	                break;
	            default:
	                type="HIUSER";
	                break;    
	        }
	        printf("[%2d]\t\t%-12s\t\t%08x\t\t%06x\t\t\t%06x\t\t\t%-12s\n",i,str_table_s+sectoin_start->sh_name,sectoin_start->sh_addr,sectoin_start->sh_offset,sectoin_start->sh_size,type);
	        sectoin_start++;
		}	
	}
}

void printSymbols(){
	if (Currentfd!=-1){
		Elf64_Ehdr *header = (Elf64_Ehdr *) map_start;
		void *addr_abs = map_start+ header->e_shoff;
		Elf64_Shdr *sectoin_start = (Elf64_Shdr *) addr_abs;
		addr_abs = sectoin_start+header->e_shstrndx;
		char  *str_table_s = (char*) (map_start +((Elf64_Shdr *) addr_abs)->sh_offset);
		addr_abs = sectoin_start+header->e_shstrndx+2;  																                         
    	char * strTbl=(char*) (map_start +(Elf64_Shdr* addr_abs)->sh_offset);       
		int i;
		for (i=0; i<header->e_shnum; i++){
			if (sectoin_start->sh_type == SHT_DYNSYM || sectoin_start->sh_type == SHT_SYMTAB){
				unsigned int sym_number = sectoin_start->sh_size / sectoin_start->sh_entsize;
	            addr_abs = map_start + sectoin_start->sh_offset;
	            Elf64_Sym *sym_tbl = (Elf64_Sym*) addr_abs;
	            if (sectoin_start->sh_type == SHT_DYNSYM)
	            	printf("\nSymbol table '.dynsym' contains %d entries:\n[index]\t\tvalue\t\tsection_index\t\tsection_name\t\tsymbol_name\n",sym_number);
	            else
	            	printf("\nSymbol table '.symtab' contains %d entries:\n[index]\t\tvalue\t\tsection_index\t\tsection_name\t\tsymbol_name\n",sym_number);
	            int j;
	            for (j=0; j<sym_number; j++){
	            	char * secName="";
	            	char* temp ="";
	            	addr_abs = map_start+ header->e_shoff;	                	                
	                Elf64_Shdr *symSec = (Elf64_Shdr *) addr_abs;        
	                symSec = symSec+sym_tbl->st_shndx;
	                
	                if (sym_tbl->st_shndx != SHN_ABS && sym_tbl->st_shndx != SHN_COMMON && sym_tbl->st_shndx != SHN_UNDEF)
	                    secName = str_table_s+ symSec->sh_name;	                
	                printf("[%2d]\t\t%08x\t",j,sym_tbl->st_value);
	                switch (sym_tbl->st_shndx){
	                	case 0:
	                		temp = "UNDEF";
	                		break;
	                	case 65521: /*0xfff1*/
	                		temp = "ABS";
	                		break;
	                }
	                if (strcmp(temp,"")==0)
	                	printf("%3d",sym_tbl->st_shndx);
	                else
	                	printf("%s",temp);
	                printf("\t\t\t%-12s\t\t%-12s\n",secName,strTbl+sym_tbl->st_name );
	                sym_tbl++;
	            }
	        }
	        sectoin_start++;
		}	
	}
}



void quit(){
	munmap(map_start, fd_stat.st_size);
	close(Currentfd);
	exit(0);
}

struct func_struct {
	char *name;
	void (*func_ptr)();
};

int main(int argc, char **argv){
	struct func_struct menu[] = {{"Examine ELF file", examineELFFile}, {"Print Section Names", printSectionNames}, {"Print Symbols", printSymbols}, {"Quit", quit}, {NULL, NULL}};
	int i, input, bound = 5;
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




 
