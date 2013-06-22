#ifndef HIVE_SIM_WORKER_HPP_
#define HIVE_SIM_WORKER_HPP_

#include <event.h>
#include <sched.h>

#include <map>

#include <platform.h>
#include <timer.h>

class worker {
public:
	worker();
	~worker();
	void addTimer(callback_t *callback, int timeout);
	void control();
	void start();
	void command(int sock);
	void acceptControlConn();
private:
	int m_control_sk;
	struct event_base *m_ev_base;
	struct event *m_keep_alive;
	struct event *m_control_event;

	void cmd_load(char *);
	void cmd_unload(char *);
};

typedef void(worker::*command_handler)(char*);
typedef std::map<const char*, command_handler> command_handlers_t;

static const int max_num_workers = 4;
extern worker *workers[max_num_workers];

static inline worker *get_worker()
{
	return workers[sched_getcpu()];
}

extern void *worker_init(void *_cpu);

#endif /* end of include guard: HIVE_SIM_WORKER_HPP_ */
