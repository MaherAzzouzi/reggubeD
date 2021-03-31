#include <elf.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>


#if defined(__linux)
	#if defined(__x86_64)
		Elf64_Ehdr header;
		Elf64_Phdr program_h;
	#elif defined(__i386)
		Elf32_Ehdr header;
		Elf32_Phdr program_h;
	#endif
#endif

#define POINTER_PADD "%-10p"
#define STRING_PADD "%-20s"
