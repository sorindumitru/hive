#include <stdio.h>
#include <stdlib.h>

#include <platform.h>
#include <node.h>
#include <timer.h>

struct timers_data {
	struct node node;
	struct timer *simple;
	struct timer *recursive;
};

void recursive_callback(void *_)
{
	static int count = 0;
}

void *timers_init(void)
{
	struct timers_data *data= plat_alloc(sizeof(*data));
	int rnd = ((float) rand())/RAND_MAX * 10.0; 

	plat_memset(data, 0, sizeof(*data));

	data->recursive = timer_new_recursive(recursive_callback, rnd);
	data->recursive->debug = 1;
	timer_add(data->recursive);

	return data;
}

struct node *timers_getnode(void *priv)
{
	struct timers_data *data = priv;
	return &data->node;
}

void timers_exit(void *priv)
{
	struct timers_data *data = priv;

	timer_free(data->recursive);

	plat_free(data);
}

void timers_start(void *priv)
{
}

void timers_stop(void *priv)
{
}

