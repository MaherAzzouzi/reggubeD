#include <elf.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>


#if defined(__linux)
	#if defined(__x86_64)
		Elf64_Ehdr header;
	#elif defined(__i386)
		Elf32_Ehdr header;
	#endif
#endif
