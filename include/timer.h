#ifndef HIVE_TIMER_HPP_
#define HIVE_TIMER_HPP_

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*callback_t)(void *arg);

struct timer {
	unsigned char	recursive:1;
	unsigned char	debug:1;
	unsigned long	expires;
	callback_t	cb;
	void		*arg;
	void		*plat_priv;

	struct timespec set;
	unsigned int count;
};

extern struct timer *timer_new(callback_t cb, unsigned long expires);
extern void timer_add(struct timer *timer);
extern void timer_del(struct timer *timer);
extern void timer_free(struct timer *timer);

static inline struct timer *timer_new_recursive(callback_t cb, unsigned long expires)
{
	struct timer *t = timer_new(cb, expires);

	t->recursive = 1;

	return t;
}


extern struct event_base *m_plat_base[16];

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: HIVE_TIMER_HPP_ */
