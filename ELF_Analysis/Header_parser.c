#include "Header_parser.h"


int parse(char* path){

	// ELF_Header header;	
	// Trying to open the file provided to the debugger
	// To get informations about it.
	FILE* file = fopen(path, "rb");

	// Check if the file exist
	// Later we will check if this file is 
	// an executable ELF.
	if(file==NULL){
		printf("Can't open the file at : %s\n", path);
		return -1;
	}
	
	// I need the pointer to the beginning of the file
	// so I can return to it later
	// after fseek()
	int start = ftell(file);

	// Using fseek to set the current position indicator
	// to the end, to extract the size of the file.
	fseek(file, 0, SEEK_END);

	// Get the size of that file.
	int size = ftell(file);

	// return to the start of the file.
	fseek(file, start, SEEK_SET);

	printf("Size %db\n", size);
	printf("\n[ELF header]\n");

	// Get the header here.
	fread(&header, 0x1, sizeof(header), file);
	
	// Exit if the binary provided is not an ELF.
	if(	header.e_ident[1] != 'E'||
		header.e_ident[2] != 'L'||
		header.e_ident[3] != 'F'
			){
		printf("This is not an ELF binary.\n");
		return -1;
	}
	
	// Detect the architecture and print it to the user.
	char ident = header.e_ident[EI_CLASS];
	if(ident == ELFCLASS32)
		printf("Arch 32\n");
	else if (ident == ELFCLASS64)
		printf("Arch 64\n");
	else
		printf("Invalid architecture.");

	// Detect the endianness and show it.
	ident = header.e_ident[EI_DATA];
	if(ident == ELFDATA2LSB)
		printf("Little-endian\n");
	else if(ident == ELFDATA2MSB)
		printf("Big-endian\n");
	else
		printf("Invalid encoding.");

	// Now let's detect the type of the ELF.
	switch(header.e_type)
	{
		case(ET_REL):
			printf("A relocatable file.\n");
			break;
		case(ET_EXEC):
			printf("An executable file.\n");
			break;
		case(ET_DYN):
			printf("A shared object.\n");
			break;
		case(ET_CORE):
			printf("A core file.\n");
			break;
		default:
			printf("An unknown type.\n");
	}

	// Where the ELF should run.
	// Which architecture.
	// This debugger is for AMD x86_64
	// so we will check for that.
	if(header.e_machine == EM_X86_64){
		printf("Designed for AMD x86_64\n");
	}else
		return -1;

	// If we make it until here
	// We can provide the entry point
	// Where the CPU should start executing this file.
	printf("Text starts at offset : %p\n", header.e_entry);


	// If the ELF has a program header table show its offset.
	if(header.e_phoff)
		printf("The program header table offset : %p\n", header.e_phoff);
	
	// If the ELF has a section header table show its offset.
	if(header.e_shoff)
		printf("The section header table : %p\n", header.e_shoff);
	
	// Try to fseek into the start of the program header
	// and parse it in our struct, then analyze it.
	fseek(file, header.e_phoff, SEEK_SET);

	// Now we're ready to parse it
	printf("\n[Program header]\n");
	printf("There are %d program headers.\n", header.e_phnum);
	printf("The size of each entry is %p\n", header.e_phentsize);
	int i;

	// We will be taking information from every element
	// of the array.
	// program_header[i]
	//
	for(i=0; i<header.e_phnum; i++){
		fread(&program_h, 0x1, sizeof(program_h), file);
		switch(program_h.p_type){
			case PT_LOAD:
				printf("LOADABLE SEG\n");
				break;
			case PT_DYNAMIC:
				printf("DYNAMIC LINKING\n");
				break;
			case PT_INTERP:
				printf("INTERPRETER\n");
				break;
			case PT_NOTE:
				printf("AUXILIARY\n");
				break;
			case PT_PHDR:
				printf("PROGRAM HEADER SEG\n");
				break;
			case PT_TLS:
				printf("THREAD LOCAL STORAGE\n");
				break;
		
		}

	}

	fclose(file);
	return 0;
}

int main(){
	parse("/home/maher/Projects/reggubeD/ELF_Analysis/Header_parser");
	return 0;
}
