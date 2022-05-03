#include <kernel.h>

#define OBJ_PROVIDER_THREAD_STACK_SIZE 0x400U
#define OBJ_CONSUMER_WORKQ_STACK_SIZE 0x400U

#define OBJ_PROVIDER_THREAD_PRIO K_PRIO_PREEMPT(8)
#define OBJ_CONSUMER_WORKQ_PRIO K_PRIO_PREEMPT(7)
#define OBJ_SIZE 16U

static K_THREAD_STACK_DEFINE(provider_thread_stack, OBJ_PROVIDER_THREAD_STACK_SIZE);
static K_THREAD_STACK_DEFINE(consumer_workq_stack, OBJ_CONSUMER_WORKQ_STACK_SIZE);

typedef char obj_t[OBJ_SIZE];

struct triggered_from_fifo_test_item {
	k_tid_t tid;
	struct k_thread obj_provider_thread;
	struct k_work_q obj_consumer_workq;
	struct k_work_poll work;
	struct k_fifo fifo;
	struct k_poll_event event;
	uint32_t fifo_item;
};

static struct triggered_from_fifo_test_item triggered_from_fifo_test;

static void obj_provider_thread(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	k_fifo_put(&triggered_from_fifo_test.fifo,
		   &triggered_from_fifo_test.fifo_item);
}

static void triggered_from_fifo_work_handler(struct k_work *work)
{
	uint32_t *obj;
	obj = k_fifo_get(&triggered_from_fifo_test.fifo, K_NO_WAIT);
}

static void test_triggered_from_fifo_init(void)
{
	struct triggered_from_fifo_test_item *const ctx = &triggered_from_fifo_test;

	ctx->tid = k_thread_create(&ctx->obj_provider_thread,
				   provider_thread_stack,
				   OBJ_PROVIDER_THREAD_STACK_SIZE,
				   obj_provider_thread,
				   NULL, NULL, NULL,
				   OBJ_PROVIDER_THREAD_PRIO, 0, K_FOREVER);
	k_work_queue_init(&ctx->obj_consumer_workq);
	k_fifo_init(&ctx->fifo);
	k_work_poll_init(&ctx->work, triggered_from_fifo_work_handler);
	k_poll_event_init(&ctx->event, K_POLL_TYPE_FIFO_DATA_AVAILABLE,
			  K_POLL_MODE_NOTIFY_ONLY, &ctx->fifo);

	k_work_queue_start(&ctx->obj_consumer_workq, consumer_workq_stack,
			   OBJ_CONSUMER_WORKQ_STACK_SIZE, OBJ_CONSUMER_WORKQ_PRIO,
			   NULL);
	k_work_poll_submit_to_queue(&ctx->obj_consumer_workq, &ctx->work,
				    &ctx->event, 1U, K_FOREVER);
}

static void test_triggered_from_fifo_start(void)
{
	k_thread_start(triggered_from_fifo_test.tid);
}


/**
 * @brief Test triggered work item, triggered by a fifo message.
 *
 * @ingroup kernel_workqueue_tests
 *
 * @see k_work_poll_init(), k_work_poll_submit()
 *
 */
static void test_triggered_from_fifo(void)
{
	// TC_PRINT("Starting triggered from fifo test\n");

	// TC_PRINT(" - Initializing kernel objects\n");
	test_triggered_from_fifo_init();

	// TC_PRINT(" - Starting the thread putting the message in the fifo\n");
	test_triggered_from_fifo_start();

	// reset_results();
}

int main(void)
{
	test_triggered_from_fifo();

	return 0;
}
