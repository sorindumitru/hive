#ifndef HIVE_SIM_WORKER_HPP_
#define HIVE_SIM_WORKER_HPP_

#include <event.h>
#include <sched.h>

#include <platform.h>
#include <timer.h>

class control;

class worker {
public:
	worker();
	~worker();
	void addTimer(callback_t *callback, int timeout);
	void start();
private:
	struct event_base *m_ev_base;
	struct event *m_keep_alive;
	control *m_control;
};

static const int max_num_workers = 4;
extern worker *workers[max_num_workers];

static inline worker *get_worker()
{
	return workers[sched_getcpu()];
}

extern void *worker_init(void *_cpu);

#endif /* end of include guard: HIVE_SIM_WORKER_HPP_ */
