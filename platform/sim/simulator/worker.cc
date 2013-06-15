#include <iostream>

#include <simulator/worker.hpp>

void *worker_init(void *_cpu)
{
	std::cout << "Starting thread %d" << std::endl;

	return NULL;
}
