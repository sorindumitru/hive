#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <pthread.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>

#include <scheduler.h>
#include <scheduler.hpp>
#include <module.hpp>

void *scheduler_init(void *arg);

static void sim_schedule_timer(struct scheduler *sched,
		int timeout,
		callback_t *cb,
		void *arg);

Scheduler **schedulers;

int main(int argc, char **argv)
{
	struct scheduler sim_sched = {
		sim_schedule_timer,
		NULL,
	};
	int nrcpus = sysconf(_SC_NPROCESSORS_ONLN);

	register_scheduler(&sim_sched);

	schedulers = new Scheduler*[nrcpus];
	if (!schedulers)
		exit(-ENOMEM);

	pthread_t *pschedulers = (pthread_t *) calloc(nrcpus, sizeof(pschedulers));
	if (!pschedulers)
		exit(-ENOMEM);

	/* start schedulers */
	for (int cpu = 0; cpu < nrcpus; cpu++) {
		int err = pthread_create(&pschedulers[cpu],
		                         NULL,
		                         scheduler_init,
		                         reinterpret_cast<void *>(cpu));
		if (err != 0) {
			perror("pthread_create");
			exit(-1);
		}
	}

	/* wait for schedulers to finish */
	for (int cpu = 0; cpu < nrcpus; cpu++) {
		if (pthread_join(pschedulers[cpu], NULL) != 0) {
			perror("pthread_join");
			exit(-1);
		}
	}

	delete[] schedulers;
	free(pschedulers);

	return 0;
}

static void sim_schedule_timer(struct scheduler *sched,
		int timeout,
		callback_t *cb,
		void *arg)
{
	int cpu = sched_getcpu();
	event_arg *earg = new event_arg;
	earg->cb = cb;
	earg->args = arg;
	schedulers[cpu]->schedule(timeout, earg);
}

void *scheduler_init(void *arg)
{
	int err;
	size_t cpu = reinterpret_cast<size_t>(arg);
	cpu_set_t cpu_set;

	CPU_ZERO(&cpu_set);
	CPU_SET(cpu, &cpu_set);

	if ((err = pthread_setaffinity_np(pthread_self(),
	                                  sizeof(cpu_set_t),
	                                  &cpu_set)) != 0) {
		fprintf(stderr, "pthread_setaffinity_np: %s\n", strerror(err));
		pthread_exit(NULL);
	}

	sleep(1);

	Scheduler *sched = new Scheduler();
	schedulers[cpu] = sched;

	std::cout << "Started scheduler " << (cpu + 1) << std::endl;
	if (cpu == 0)
		/* TODO: start command thread */
	sched->run();

	delete sched;
	pthread_exit(NULL);
}
