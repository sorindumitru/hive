#ifndef HIVE_SIM_WORKER_HPP_
#define HIVE_SIM_WORKER_HPP_

class worker {
public:
	worker();
private:
};

static const int max_num_workers = 4;
extern void *worker_init(void *_cpu);

#endif /* end of include guard: HIVE_SIM_WORKER_HPP_ */
