#include <iostream>
#include <pthread.h>
#include <unistd.h>

#include <worker.hpp>
#include <control.hpp>

static struct timeval keep_alive_tv = {
	.tv_sec = 60,
};

static void keep_alive_cb(int fd, short int event, void *arg)
{
}

worker::worker()
{
	m_ev_base = event_base_new();
	if (!m_ev_base)
		perror("Could not allocate new event base");
	m_keep_alive = event_new(m_ev_base, -1, EV_PERSIST,
				 keep_alive_cb, NULL);
}

worker::~worker()
{
	if (m_control)
		delete m_control;
}

void worker::start()
{
	extern event_base *m_plat_base[16];
	std::cout << "Starting worker " << sched_getcpu() << std::endl;
	evtimer_add(m_keep_alive, &keep_alive_tv);

	m_plat_base[sched_getcpu()] = m_ev_base;	

	if (sched_getcpu() == 0)
		m_control = new control;

	event_base_dispatch(m_ev_base);
}

worker *workers[max_num_workers];

void *worker_init(void *_cpu)
{
	int cpu = (long)_cpu;
	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);
	CPU_SET(cpu, &cpuset);

	workers[cpu] = new worker();

	if (pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset))
		perror("Coult not set thread affinity");

	sleep(1);

	workers[cpu]->start();

	return NULL;
}
