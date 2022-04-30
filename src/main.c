#include <zephyr.h>

#include "sub/hello.h"
#include <simplelib/slib.h>

int main(void)
{
	slib_init();

	static uint32_t i = 0;
	uint32_t b;
	for (;;) {
		hello();
		slib_put(i);
		slib_get(&b);
		printk("%u\n", b);
		k_sleep(K_SECONDS(1));

		i++;
	}

	return 0;
}