#include <assert.h>
#include <iostream>
#include <sched.h>

#include <scheduler.hpp>

Scheduler::Scheduler()
{
	m_cpu = sched_getcpu();

	/* set up libevent */
	m_ebase = event_base_new();
}

Scheduler::~Scheduler()
{
	event_base_free(m_ebase);
}

void dummy_cb(evutil_socket_t fd, short what, void *arg)
{
	std::cerr << "[SCHEDULER] dummy event called; panic" << std::endl;
	assert(1);
}

void Scheduler::run(void)
{
	/* Add dummy event so the loop doesn't end
	 * even if we don'y have events registered */
	struct event *dummy = event_new(m_ebase, -1,
			EV_WRITE, dummy_cb, NULL);
	event_add(dummy, NULL);
	event_base_loop(m_ebase, 0);
}

void sched_event_cb(evutil_socket_t fd, short what, void *arg)
{
	event_arg *event = static_cast<event_arg *>(arg);
	(*event->cb)(event->args);
	event_free(event->event);
	delete event;
}

void Scheduler::schedule(int timeout, event_arg *args)
{
	struct event *event = event_new(m_ebase, -1,
			EV_WRITE, sched_event_cb, static_cast<void *>(args));
	struct timeval expire;
	expire.tv_sec = timeout/1000000;
	expire.tv_usec = (timeout - expire.tv_sec*1000000);
	args->event = event;
	event_add(event, &expire);
}
