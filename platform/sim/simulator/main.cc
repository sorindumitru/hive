#include <pthread.h>

#include <iostream>
#include <vector>

#include <simulator/worker.hpp>

int main(int argc, const char *argv[])
{
	std::cout << "Starting hive simulator" << std::endl;
	std::vector<pthread_t> workers;

	for (int i = 0; i < max_num_workers; i++) {
		pthread_t thread;
		pthread_create(&thread, NULL, worker_init, (void *) i);
		workers[i] = thread;
	}

	for (auto &worker : workers)
		pthread_join(worker, NULL);

	return 0;
}
