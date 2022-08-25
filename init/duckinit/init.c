#include <stdio.h>
#include <stdint.h>
#include <arch/x86_64/syscall.h>

int main(void) {
	pid_t cid = sys_fork();
	pid_t pid = sys_getpid();
	if (cid > 0)
		sys_waitpid(cid);
					
	printf("fork() = %d, getpid() = %d\n", cid, pid);

	if (cid == 0)
		sys_exit();
	
	for (;;);
	return 0;
}
