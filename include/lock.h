#ifndef HIVE_LOCK_H_
#define HIVE_LOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

struct lock {
	volatile void	*priv;
};

extern void lock_init(struct lock *lock);
extern void lock(struct lock *lock);
extern void unlock(struct lock *lock);
extern void lock_destroy(struct lock *lock);

#ifdef __cplusplus
}
#endif


#endif /* end of include guard: HIVE_LOCK_H_ */
