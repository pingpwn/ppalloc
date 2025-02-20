#include <stdio.h>
#include "src/ppalloc.h"

int main() {
	int *ptr1;
	
	ptr1 = ppalloc(20);
	
	printf("ppalloc %p\n", ptr1);

	ptr1 = realloc((void*)ptr1, 40);

	
	printf("realloc %p\n", ptr1);


	free(ptr1);
	

	return 69;
}
