#include <stdio.h>

#include <node.h>

struct node node;

void *simple_init(void)
{
	printf("Powering up node\n");
}

struct node *simple_getnode(void *priv)
{
	return &node;
}

void simple_exit(void *_)
{
	printf("Powering down node\n");
}

void simple_start(void *_)
{
	printf("Starting node\n");
}

void simple_stop(void *_)
{
	printf("Stopping node\n");
}
