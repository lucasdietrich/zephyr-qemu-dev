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

/** If Priority is K_PRIO_PREEMPT, with CONFIG_ASSERT=y, "Recursive spinlock" assert fails
 * 
 * Exception: 
 *    ASSERTION FAIL [z_spin_lock_valid(l)] @ WEST_TOPDIR/zephyr/include/spinlock.h:129
 *            Recursive spinlock 0x11300c
 *    k_work_poll_submit(0x10e000, 0x10f8d0, 1U, K_FOREVER) = 0
 * 
 *    EAX: 0x0010b6ef, EBX: 0x0011300c, ECX: 0x001129bc, EDX: 0x0011300c
 *    ESI: 0x00109150, EDI: 0x00112ffc, EBP: 0x00112a08, ESP: 0x001129e0
 *    EFLAGS: 0x00000006 CS: 0x0008 CR3: 0x00119000
 *    call trace:
 *    EIP: 0x00101eee
 *         0x00100325 (0x112ffc)
 *         0x00109ac0 (0x10e000)
 *         0x00100c48 (0x10f840)
 *    >>> ZEPHYR FATAL ERROR 4: Kernel panic on CPU 0
 *    Current thread: 0x10f840 (unknown)
 *    Halting system
 */
K_THREAD_DEFINE(tid, 0x1000, thread, NULL, NULL, NULL, K_PRIO_COOP(8), 0, -1);

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
	uint32_t val;

	/* process all available messages */
	while (k_msgq_get(&msgq, &val, K_NO_WAIT) == 0U) {
		LOG_DBG("received val: %u", val);
	}

	/* re-schedule work */
	// k_poll_event_init(&event, K_POLL_TYPE_MSGQ_DATA_AVAILABLE,
	// 		  K_POLL_MODE_NOTIFY_ONLY, &msgq);
	int ret = k_work_poll_submit(&work, &event, 1U, K_FOREVER);
	LOG_DBG("k_work_poll_submit(%p, %p, 1U, K_FOREVER) = %d", &work, &event, ret);
}

void main(void)
{
	int ret;

	k_poll_event_init(&event, K_POLL_TYPE_MSGQ_DATA_AVAILABLE,
			  K_POLL_MODE_NOTIFY_ONLY, &msgq);
	k_work_poll_init(&work, work_handler);

	k_thread_start(tid);

	ret = k_work_poll_submit(&work, &event, 1U, K_FOREVER);
	LOG_DBG("k_work_poll_submit(%p, %p, 1U, K_FOREVER) = %d", &work, &event, ret);
}
