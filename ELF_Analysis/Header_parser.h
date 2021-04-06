#include <elf.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>

#if defined(__linux)
	#if defined(__x86_64)
		Elf64_Ehdr header;
		Elf64_Phdr program_h;
		Elf64_Shdr section_h;
	#elif defined(__i386)
		Elf32_Ehdr header;
		Elf32_Phdr program_h;
		Elf32_Shdr section_h;
	#endif
#endif

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define DEFAULT "\033[0m"

#define POINTER_PADD "%-10p"
#define STRING_PADD "%-20s"
#define PATH_LEN 256

#define S_PADD(x) printf(STRING_PADD, x)
#define P_PADD(x) printf(POINTER_PADD, x)
#define P_PADD12(x) printf("%-12p", x)


#define SHOW_LINE(n) \
		printf(YELLOW); \
		for(i=0; i<n; i++) putchar('-'); \
		putchar('\n'); \
		printf(DEFAULT);


