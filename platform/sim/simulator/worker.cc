#include <iostream>
#include <pthread.h>
#include <unistd.h>

#include <simulator/worker.hpp>

void *worker_init(void *_cpu)
{
	int cpu = (long)_cpu;
	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);
	CPU_SET(cpu, &cpuset);

	if (pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset))
		perror("Coult not set thread affinity");

	sleep(1);
	std::cout << "Started thread " << cpu << std::endl;

	return NULL;
}
