#include <stdio.h>
#include <stdint.h>

int main(void) {
	uint64_t cid, pid;
	asm volatile (
			"int $0x80;\n\t" 
			: "=a"(cid)
			: "a"(0x100)
            : "memory");
	asm volatile (
			"int $0x80;\n\t" 
			: "=a"(pid)
			: "a"(0x101)
            : "memory");
			
	printf("fork() = %d\ngetpid() = %d\n", cid, pid);
	
	return 0;
}
