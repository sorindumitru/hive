#include <scheduler.h>

struct scheduler *scheduler;

int register_scheduler(struct scheduler *sched)
{
	scheduler = sched;
}

struct scheduler *get_default_scheduler(void)
{
	return scheduler;
}
