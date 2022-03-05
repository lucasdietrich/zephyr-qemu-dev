#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <net/socket.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(application, LOG_LEVEL_DBG);

#define HOSTNAME "www.google.fr"

void main(void)
{
	int ret = 0U;
	struct zsock_addrinfo *ai = NULL;

	for (;;) {		
		const struct zsock_addrinfo hints = { .ai_family = AF_INET };
		ret = zsock_getaddrinfo(HOSTNAME, NULL, &hints, &ai);
		if (ret != 0) {
			LOG_ERR("failed to resolve hostname err = %d", ret);
		} else {
		// convert addrinfo structure ai_addr to string
			char addr_str[INET_ADDRSTRLEN];
			zsock_inet_ntop(AF_INET, (const void *)ai->ai_addr, addr_str, sizeof(addr_str));

			// log addr_str
			LOG_INF("converted addrinfo structure to string: %s", addr_str);
		}

		zsock_freeaddrinfo(ai);

		k_sleep(K_SECONDS(25));
	}
}
