#include <event.h>
#include <sched.h>
#include <stdlib.h>
#include <time.h>

#include <timer.h>

struct event_base *m_plat_base[16];

#define NANO_IN_SEC 1000000000ULL

static inline void timer_debug(struct timespec *set, struct timespec *triggered, int expires)
{
	time_t seconds = triggered->tv_sec - set->tv_sec;
	long nano = triggered->tv_nsec - set->tv_nsec;
	long long difference = seconds * NANO_IN_SEC + nano - expires * NANO_IN_SEC;

	printf("%lld\n", difference/100);
}

void timer_cb(int fd, short int event, void *arg)
{
	struct timer *timer = (struct timer *)arg;
	struct timespec triggered;

	if (timer->debug) {
		clock_gettime(CLOCK_MONOTONIC, &triggered);
		timer->count++;
		timer_debug(&timer->set, &triggered, timer->expires * timer->count);
	}

	timer->cb(timer->arg);
}

struct timer *timer_new(callback_t cb, unsigned long expires)
{
	struct timer *timer = malloc(sizeof(*timer));

	timer->expires = expires;
	timer->cb = cb;
	timer->plat_priv = NULL;
	timer->count = 0;

	return timer;
}

void timer_add(struct timer *timer)
{
	struct event *ev = event_new(m_plat_base[sched_getcpu()], -1,
			timer->recursive ? EV_PERSIST : 0,
			timer_cb, timer);
	struct timeval timeout = {
		.tv_sec = timer->expires,
		.tv_usec = 0,
	};

	timer->plat_priv = ev;

	clock_gettime(CLOCK_MONOTONIC, &timer->set);

	evtimer_add(ev, &timeout);
}

void timer_del(struct timer *timer)
{
	struct event *ev = timer->plat_priv;
	if (ev != NULL)
		evtimer_del(ev);
}

void timer_free(struct timer *timer)
{
	struct event *ev = timer->plat_priv;
	if (ev != NULL) {
		evtimer_del(ev);
		event_free(ev);
	}
	free(timer);
}
