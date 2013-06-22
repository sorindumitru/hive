#include <stdio.h>

void *simple_init(void)
{
	printf("Powering up node\n");
}

void simple_exit(void *_)
{
	printf("Powering down node\n");
}
