#ifndef HIVE_TIMER_HPP_
#define HIVE_TIMER_HPP_

typedef void (*callback_t)(void *arg);

struct timer {
	bool		recursive;
	unsigned long	expires;
	callback_t	*cb;
};

struct timer *timer_new(callback_t *cb, unsigned long expires);

static inline struct timer *timer_new_recursive(callback_t *cb, unsigned long expires)
{
	struct timer *t = timer_new(cb, expires);

	t->recursive = true;

	return t;
}

#endif /* end of include guard: HIVE_TIMER_HPP_ */
