#include <pthread.h>

#include <lock.h>
#include <platform.h>

void lock_init(struct lock *lock)
{
	pthread_spinlock_t *sl = plat_alloc(sizeof(*sl));

	pthread_spin_init(sl, 0);
	lock->priv = sl;
}

void lock(struct lock *lock)
{
	pthread_spinlock_t *sl = (pthread_spinlock_t *) lock->priv;
	pthread_spin_lock(sl);
}

void unlock(struct lock *lock)
{
	pthread_spinlock_t *sl = (pthread_spinlock_t *) lock->priv;
	pthread_spin_unlock(sl);
}


void loc_destroy(struct lock *lock)
{
	pthread_spinlock_t *sl = (pthread_spinlock_t *) lock->priv;
	pthread_spin_destroy(sl);
	free((void *)sl);
}
