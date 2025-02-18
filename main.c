#include <stdio.h>
#include "src/ppalloc.h"
#include <stdlib.h>
int main() {
	int *ptr1, *ptr2;
	
	ptr1 = ppalloc(20);
	ptr2 = malloc(20);
	
	printf("ppalloc %p\n", ptr1);
	printf("malloc %p", ptr2);


	return 69;
}
