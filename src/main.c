#include <zephyr.h>

#include "sub/hello.h"
#include <simplelib/slib.h>

int main(void)
{
	slib_init();

	for (;;) {
		hello();
		k_sleep(K_SECONDS(1));
	}

	return 0;
}