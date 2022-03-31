/* READ https://docs.zephyrproject.org/3.0.0/reference/kernel/synchronization/condvar.html */

#include <kernel.h>

void thread(void *_a, void *_b, void *_c);

K_MUTEX_DEFINE(mutex);
K_CONDVAR_DEFINE(condvar);
bool initialized = false;

K_THREAD_DEFINE(thread1, 0x400, thread, NULL, NULL, NULL, K_PRIO_PREEMPT(8), 0, 1000);
K_THREAD_DEFINE(thread2, 0x400, thread, NULL, NULL, NULL, K_PRIO_PREEMPT(8), 0, 2000);
K_THREAD_DEFINE(thread3, 0x400, thread, NULL, NULL, NULL, K_PRIO_PREEMPT(8), 0, 4000);

void main(void)
{
	int ret;

	uint32_t i = 0;

	k_sleep(K_SECONDS(3));

	k_mutex_lock(&mutex, K_FOREVER);
	initialized = true;
	ret = k_condvar_broadcast(&condvar);
	k_mutex_unlock(&mutex);

	printk("(%p) condvar broadcast, ret %d\n", k_current_get(), ret);

	for (;;) {
		i++;

		printk("(%u) Hello world !\n", i);
		k_sleep(K_MSEC(1000));
	}
}

void thread(void *_a, void *_b, void *_c)
{
	int ret;
	k_mutex_lock(&mutex, K_FOREVER);
	if (initialized == false) {
		printk("(%p) Waiting for condvar\n", k_current_get());
		ret = k_condvar_wait(&condvar, &mutex, K_FOREVER);
		printk("(%p) k_condvar_wait ret %d\n", k_current_get(), ret);
	} else {
		printk("(%p) Already initialized, not waiting for condvar\n",
		       k_current_get());
	}
	k_mutex_unlock(&mutex);	

	for (;;) {
		printk("(%p) working ...\n", k_current_get());
		k_sleep(K_MSEC(1000));
	}
}