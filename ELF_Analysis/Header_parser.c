#include "Header_parser.h"

// This function to parse the shstrtab, into an array of pointers.
char** parse_shstrtab(FILE* file, int offset) {
	int i;
	assert(fseek(file, offset, SEEK_SET) == 0);
}

int parse(char* path){

	// ELF_Header header;	
	// Trying to open the file provided to the debugger
	// To get informations about it.
	FILE* file = fopen(path, "rb");
	
	int current;

	// Check if the file exist
	// Later we will check if this file is 
	// an ELF executable.
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
	printf(RED "\n[ELF header]\n" DEFAULT);

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
	printf(RED "\n[Program header table]\n" DEFAULT);
	printf("There are %d program headers.\n", header.e_phnum);
	printf("The size of each entry is %p\n", header.e_phentsize);
	int i;

	// Print a nice line seperator, I can't do that using printf so excuse
	// the loop xD
	SHOW_LINE(0x58)


	// We will be taking information from every element of the array.
	// program_header[i]
	
	char interpreter_path[PATH_LEN];
	memset(interpreter_path, 0, PATH_LEN);
	printf(STRING_PADD "%-12s%-10s%-10s%-10s%-10s%-10s%s\n"
					, "SEG_TYPE", "PRMS", "OFFSET"
					, "VirtAddr", "PhysAddr"
					,"SegSize", "MemSz", "Align");
	
	SHOW_LINE(0x58)

	for(i=0; i<header.e_phnum; i++){
		fread(&program_h, 0x1, sizeof(program_h), file);
		switch(program_h.p_type){
			case PT_LOAD:
				printf(STRING_PADD, "LOADABLE SEG");
				break;
			case PT_DYNAMIC:
				printf(STRING_PADD, "DYNAMIC LINKING");
				break;
			case PT_INTERP:
				printf(STRING_PADD, "INTERPRETER");
				current = ftell(file);
				fseek(file, program_h.p_offset, SEEK_SET);
				fscanf(file, "%256s", interpreter_path);
				fseek(file, current, SEEK_SET);
				break;
			case PT_NOTE:
				printf(STRING_PADD, "NOTE");
				break;
			case PT_PHDR:
				printf(STRING_PADD, "PROGRAM HEADER SEG");
				break;
			case PT_TLS:
				printf(STRING_PADD, "THREAD LOCAL STORGE");
				break;
			case PT_GNU_STACK:
				printf(STRING_PADD, "GNU STACK");
				break;
			case PT_GNU_RELRO:
				printf(STRING_PADD, "GNU RELRO");
				break;
			case PT_GNU_EH_FRAME:
				printf(STRING_PADD, "GNU EH FRAME");
				break;
			case PT_GNU_PROPERTY:
				printf(STRING_PADD, "GNU PROPERTY");
				break;
		}
		
		// Show the permissions first.
		printf("%c%c%-10c", program_h.p_flags & PF_R ? 'R' : '-',
						 program_h.p_flags & PF_W ? 'W' : '-',
						 program_h.p_flags & PF_X ? 'X' : '-');
		// Show the offset of the segment in the file.
		printf(POINTER_PADD, program_h.p_offset);
		
		// Show the virtual address of the segment in memory.
		// if ASLR is enabled you will receive only the offset.
		printf(POINTER_PADD, program_h.p_vaddr);

		// Show the physical address.
		printf(POINTER_PADD, program_h.p_paddr);
	
		// Show the size of the segment in the file.
		printf(POINTER_PADD, program_h.p_filesz);

		// Show the size of the segment in memory
		printf(POINTER_PADD, program_h.p_memsz);

		// Show the alignement, 0 and 1 means no alignement.
		printf(POINTER_PADD, program_h.p_align);
		putchar('\n');
	}
	
	SHOW_LINE(0x58)

	// At the end print the interpreter path, usefull in pwn challenges (or re-
	// al life binaries to know which ld are you linked to)
	if(interpreter_path[0])
		printf("The interpreter : %.256s\n", interpreter_path);
	else
		puts("Statically linked binary, no interpreter found.");
	
	
	/* Now we want to parse the section headers.
	 * We want to provide the name of each header, its offset and the permissi-
	 * ons, as that will be important in exploit development (for example know
	 * if .fini_array is read only, or RW) and many more.
	 */
	
	puts(RED "\n[Section header table]" DEFAULT);
	printf("%d section headers to parse.\n", header.e_shnum);
	
	// Let's first fseek to the start of the section header table.
	fseek(file, header.e_shoff, SEEK_SET);
	current = ftell(file);
	
	printf("shstrtab is at index %d (section header string table)\n", 
					header.e_shstrndx);
	
	// Parse the shstrtab first as it contains the name of each section header.
	fseek(file, (header.e_shstrndx)*sizeof(section_h), SEEK_CUR);
	fread(&section_h, 0x1, sizeof(section_h), file);

	// section_h now contains shstrtab.
	// Get the offset to the names.
	fseek(file, section_h.sh_offset, SEEK_SET);
	char shstrtab_names[1024];
	
	// read the names here.
	fread(shstrtab_names, 1, sizeof(shstrtab_names), file);
	
	// return to the start of the section headers array.	
	fseek(file, current, SEEK_SET);	
	
	SHOW_LINE(0x5a)
	printf("%-20s%-12s%-12s%-12s%-12s%-12s%-12s\n"
					, "NAME", "VirtAddr", "Offset", "Size"
					, "Link", "Align", "Flags");
	SHOW_LINE(0x5a)
	for(i = 0; i < header.e_shnum; i++) {
		fread(&section_h, 0x1, sizeof(section_h), file);

		S_PADD(shstrtab_names+section_h.sh_name);
		P_PADD12(section_h.sh_addr);
		P_PADD12(section_h.sh_offset);
		P_PADD12(section_h.sh_size);
		P_PADD12(section_h.sh_link);
		P_PADD12(section_h.sh_addralign);
		

		printf("%c%c%c%c%c%c%c%c%c%c"
					 , section_h.sh_flags & SHF_WRITE ? 'W' : '-'
					 , section_h.sh_flags & SHF_EXECINSTR ? 'X' : '-'
					 , section_h.sh_flags & SHF_ALLOC ? 'A' : '-'
					 , section_h.sh_flags & SHF_MERGE ? 'M' : '-'
					 , section_h.sh_flags & SHF_STRINGS ? 'S' : '-'
					 , section_h.sh_flags & SHF_INFO_LINK ? 'L' : '-'
					 , section_h.sh_flags & SHF_LINK_ORDER ? 'P' : '-'
					 , section_h.sh_flags & SHF_OS_NONCONFORMING ? 'O' : '-'
					 , section_h.sh_flags & SHF_GROUP ? 'G' : '-'
					 , section_h.sh_flags & SHF_TLS ? 'T' : '-'
			  );
		putchar('\n');
	}
	SHOW_LINE(0x5a);

	fclose(file);
	return 0;
}

int main(){
	parse("/home/maher/Projects/reggubeD/ELF_Analysis/Header_parser");
	return 0;
}
