#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <pthread.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>

void *scheduler_init(void *arg);

int main(int argc, char **argv)
{
	int nrcpus = sysconf(_SC_NPROCESSORS_ONLN);

	pthread_t *schedulers = (pthread_t *) calloc(nrcpus, sizeof(schedulers));
	if (!schedulers)
		exit(-ENOMEM);

	/* start schedulers */
	for (int cpu = 0; cpu < nrcpus; cpu++) {
		int err = pthread_create(&schedulers[cpu],
		                         NULL,
		                         scheduler_init,
		                         (void *) cpu);
		if (err != 0) {
			perror("pthread_create");
			exit(-1);
		}
	}

	/* wait for schedulers to finish */
	for (int cpu = 0; cpu < nrcpus; cpu++) {
		if (pthread_join(schedulers[cpu], NULL) != 0) {
			perror("pthread_join");
			exit(-1);
		}
	}

	return 0;
}

void *scheduler_init(void *arg)
{
	int err, cpu = (int) arg;
	cpu_set_t cpu_set;

	std::cout << "Started scheduler " << (cpu + 1) << std::endl;

	CPU_ZERO(&cpu_set);
	CPU_SET(cpu, &cpu_set);

	if ((err = pthread_setaffinity_np(pthread_self(),
	                                  sizeof(cpu_set_t),
	                                  &cpu_set)) != 0) {
		fprintf(stderr, "pthread_setaffinity_np: %s\n", strerror(err));
		pthread_exit(NULL);
	}

	sleep(1);

	pthread_exit(NULL);
}
