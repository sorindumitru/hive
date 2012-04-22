#ifndef HIVE_SIM_SCHEDULER_HPP_
#define HIVE_SIM_SCHEDULER_HPP_

#include <event.h>

#include <scheduler.h>

struct event_arg {
	struct event *event;
	callback_t *cb;
	void *args;
};

class Scheduler {
public:
	Scheduler();
	~Scheduler();
	void run();
	void schedule(int timeout, event_arg *arg);
private:
	int m_cpu;
	struct event_base *m_ebase;
};

#endif /* HIVE_SIM_SCHEDULER_HPP_ */
