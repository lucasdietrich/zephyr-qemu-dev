#include <kernel.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

K_MSGQ_DEFINE(msgq, sizeof(uint32_t), 4, 4);

// publisher
void thread(void *_a, void *_b, void *_c)
{
	int ret;
	uint32_t val = 0;
	for (;;) {
		val++;
		ret = k_msgq_put(&msgq, &val, K_NO_WAIT);
		if (ret < 0) {
			LOG_ERR("k_msgq_put failed: %d", ret);
		}
		k_sleep(K_MSEC(1000));
	}
}

K_THREAD_DEFINE(tid, 0x1000, thread, NULL, NULL, NULL, K_PRIO_PREEMPT(8), 0, 0);

K_SEM_DEFINE(sem, 1, 1);
static struct k_work_poll work;
static struct k_poll_event event;
/*
 = K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_MSGQ_DATA_AVAILABLE,
							    K_POLL_MODE_NOTIFY_ONLY,
							    &msgq);
*/

static void work_handler(struct k_work *p_work)
{
	// int ret;
	uint32_t val;

	/* process all available messages */
	while (k_msgq_get(&msgq, &val, K_NO_WAIT) == 0U) {
		LOG_DBG("received val: %u", val);
	}

	/* re-schedule work */
	// k_poll_event_init(&event, K_POLL_TYPE_MSGQ_DATA_AVAILABLE,
	// 		  K_POLL_MODE_NOTIFY_ONLY, &msgq);
	// ret = k_work_poll_submit(&work, &event, 1U, K_FOREVER);
	// LOG_DBG("k_work_poll_submit(%p, %p, 1U, K_FOREVER) = %d", &work, &event, ret);

	k_sem_give(&sem);
}

void main(void)
{
	int ret;

	k_poll_event_init(&event, K_POLL_TYPE_MSGQ_DATA_AVAILABLE,
			  K_POLL_MODE_NOTIFY_ONLY, &msgq);
	k_work_poll_init(&work, work_handler);

	for (;;) {
		k_sem_take(&sem, K_FOREVER);

		
		ret = k_work_poll_submit(&work, &event, 1U, K_FOREVER);
		LOG_DBG("k_work_poll_submit(%p, %p, 1U, K_FOREVER) = %d", &work, &event, ret);

		k_sleep(K_MSEC(1000));
	}
}
