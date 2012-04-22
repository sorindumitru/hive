#ifndef HIVE_SCHEDULER_H_
#define HIVE_SCHEDULER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*callback_t)(void *arg);

struct scheduler {
	void (*schedule_timer)(struct scheduler *scheduler,
			int timeout, 
			callback_t *cb, 
			void *arg);
	void *priv;
};

extern struct scheduler *scheduler;

int register_scheduler(struct scheduler *sched);
struct scheduler *get_default_scheduler(void);	

#ifdef __cplusplus
}
#endif

#endif /* HIVE_SCHEDULER_H_ */
