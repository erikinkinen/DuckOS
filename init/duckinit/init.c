#include <stdio.h>

int main(void) {
	printf("Hello, world!\n===========================================================================\n", 0xDEADBEEF);
	printf("Once upon a time there was a lonely program which lived inside an initial ramdisk. \nIts name was \"/init\" and it had nothing to do.\n\n");

	printf("//=========\\\\\n|| TESTING ||\n\\\\=========//\n\n");
	printf("%%x: %x\n%%X: %X\n%%d: %d\n%%u: %u\n", 0xCAFEBABE, 0xDEADBEEF, -123, 321);
	return 0;
}
