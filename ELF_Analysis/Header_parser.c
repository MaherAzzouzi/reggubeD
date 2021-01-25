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

	// Get the header here.
	fread(&header, 0x1, sizeof(Elf64_Ehdr), file);
	
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

	// Detect the endianness.
	ident = header.e_ident[EI_DATA];
	if(ident == ELFDATA2LSB)
		printf("Little-endian\n");
	else if(ident == ELFDATA2MSB)
		printf("Big-endian\n");
	else
		printf("Invalid encoding.");


	return 0;
}

int main(){
	parse("/home/maher/Projects/reggubeD/ELF_Analysis/Header_parser");
	return 0;
}
