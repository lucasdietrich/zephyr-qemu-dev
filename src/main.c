#include <zephyr.h>

#include "sub/hello.h"

#include <simplelib/slib.h>
#include <myzephyrlib/zlib.h>

int main(void)
{
	slib_init();

	static uint32_t i = 0;
	uint32_t b;
	for (;;) {
		hello();
		myzephyrlib_put(i);
		myzephyrlib_get(&b);
		printk("%u\n", b);
		k_sleep(K_SECONDS(1));

		i++;
	}

	return 0;
}