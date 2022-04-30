#include <myzephyrlib/zlib.h>

#include <kernel.h>

K_MSGQ_DEFINE(q, sizeof(uint32_t), 4U, 4U);

int myzephyrlib_put(uint32_t data)
{
	return k_msgq_put(&q, (void *)&data, K_NO_WAIT);
}

int myzephyrlib_get(uint32_t *data)
{
	return k_msgq_get(&q, data, K_NO_WAIT);
}