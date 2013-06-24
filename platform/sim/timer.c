#include <event.h>
#include <sched.h>
#include <stdlib.h>

#include <timer.h>

struct event_base *m_plat_base[16];

void timer_cb(int fd, short int event, void *arg)
{
	struct timer *timer = (struct timer *)arg;

	timer->cb(timer->arg);
}

struct timer *timer_new(callback_t cb, unsigned long expires)
{
	struct timer *timer = malloc(sizeof(*timer));

	timer->expires = expires;
	timer->cb = cb;
	timer->plat_priv = NULL;

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
