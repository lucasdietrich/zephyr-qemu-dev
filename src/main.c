#include <kernel.h>

#define MSG_PROVIDER_THREAD_STACK_SIZE 0x400U
#define MSG_CONSUMER_WORKQ_STACK_SIZE 0x400U

#define MSG_PROVIDER_THREAD_PRIO K_PRIO_PREEMPT(8)
#define MSG_CONSUMER_WORKQ_PRIO K_PRIO_PREEMPT(7)
#define MSG_SIZE 16U

static K_THREAD_STACK_DEFINE(provider_thread_stack, MSG_PROVIDER_THREAD_STACK_SIZE);
static K_THREAD_STACK_DEFINE(consumer_workq_stack, MSG_CONSUMER_WORKQ_STACK_SIZE);

typedef char msg_t[MSG_SIZE];

struct triggered_from_msgq_test_item {
	k_tid_t tid;
	struct k_thread msg_provider_thread;
	struct k_work_q msg_consumer_workq;
	struct k_work_poll work;
	char msgq_buf[1][MSG_SIZE];
	struct k_msgq msgq;
	struct k_poll_event event;
};

static struct triggered_from_msgq_test_item triggered_from_msgq_test;

static void msg_provider_thread(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	msg_t msg;

	k_msgq_put(&triggered_from_msgq_test.msgq, &msg, K_NO_WAIT);
}

static void triggered_from_msgq_work_handler(struct k_work *work)
{
	msg_t msg;

	// zassert_true(k_msgq_get(&triggered_from_msgq_test.msgq, &msg, K_NO_WAIT) == 0, NULL);

	k_msgq_get(&triggered_from_msgq_test.msgq, &msg, K_NO_WAIT);
}

static void test_triggered_from_msgq_init(void)
{
	struct triggered_from_msgq_test_item *const ctx = &triggered_from_msgq_test;

	ctx->tid = k_thread_create(&ctx->msg_provider_thread,
				   provider_thread_stack,
				   MSG_PROVIDER_THREAD_STACK_SIZE,
				   msg_provider_thread,
				   NULL, NULL, NULL,
				   MSG_PROVIDER_THREAD_PRIO, 0, K_FOREVER);
	k_work_queue_init(&ctx->msg_consumer_workq);
	k_msgq_init(&ctx->msgq,
		    (char *)ctx->msgq_buf,
		    MSG_SIZE, 1U);
	k_work_poll_init(&ctx->work, triggered_from_msgq_work_handler);
	k_poll_event_init(&ctx->event, K_POLL_TYPE_MSGQ_DATA_AVAILABLE,
			  K_POLL_MODE_NOTIFY_ONLY, &ctx->msgq);

	k_work_queue_start(&ctx->msg_consumer_workq, consumer_workq_stack,
			   MSG_CONSUMER_WORKQ_STACK_SIZE, MSG_CONSUMER_WORKQ_PRIO,
			   NULL);
	k_work_poll_submit_to_queue(&ctx->msg_consumer_workq, &ctx->work,
				    &ctx->event, 1U, K_FOREVER);
}

static void test_triggered_from_msgq_start(void)
{
	k_thread_start(triggered_from_msgq_test.tid);
}


/**
 * @brief Test triggered work item, triggered by a msgq message.
 *
 * @ingroup kernel_workqueue_tests
 *
 * @see k_work_poll_init(), k_work_poll_submit()
 *
 */
static void test_triggered_from_msgq(void)
{
	// TC_PRINT("Starting triggered from msgq test\n");

	// TC_PRINT(" - Initializing kernel objects\n");
	test_triggered_from_msgq_init();

	// TC_PRINT(" - Starting the thread putting the message in the msgq\n");
	test_triggered_from_msgq_start();

	// reset_results();
}

int main(void)
{
	test_triggered_from_msgq();

	return 0;
}
