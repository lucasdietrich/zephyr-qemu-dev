#include <simplelib/slib.h>

#include <zephyr.h>

#include <stdio.h>

K_MSGQ_DEFINE(q, sizeof(uint32_t), 4U, 4U);

int slib_init(void)
{
	printf("Hello from slib !\n");

	return 0;
}

int slib_put(uint32_t data)
{
	return k_msgq_put(&q, (void *)&data, K_NO_WAIT);
}

int slib_get(uint32_t *data)
{
	return k_msgq_get(&q, data, K_NO_WAIT);
}