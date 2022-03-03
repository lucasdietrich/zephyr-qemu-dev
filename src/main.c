#include <kernel.h>

void main(void)
{
	uint32_t i = 0;

	for (;;) {
		i++;

		printk("(%u) Hello world !\n", i);
		k_sleep(K_MSEC(1000));
	}
}
