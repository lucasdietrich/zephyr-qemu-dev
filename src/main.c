#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <time.h>

#include <net/mqtt.h>

#include <net/socket.h>
#include <net/net_core.h>
#include <net/net_if.h>
#include <net/net_mgmt.h>
#include <net/dns_resolve.h>

#include "certs/certs.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(aws_client, LOG_LEVEL_DBG);

void resolve(const char *hostname)
{
	int ret = 0U;
	struct zsock_addrinfo *ai = NULL;

	const struct zsock_addrinfo hints = { .ai_family = AF_INET, .ai_socktype = SOCK_STREAM, .ai_protocol = 0 };
	ret = zsock_getaddrinfo(hostname, "80", &hints, &ai);
	if (ret != 0) {
		LOG_ERR("failed to resolve hostname err = %d (errno = %d)", ret, errno);
	} else {
		char addr_str[INET_ADDRSTRLEN];
		zsock_inet_ntop(AF_INET, &((struct sockaddr_in*) ai->ai_addr)->sin_addr, addr_str, sizeof(addr_str));

		// log addr_str
		LOG_INF("resolve: %s", addr_str);
	}

	zsock_freeaddrinfo(ai);
}

void main(void)
{
	dns_init_resolver();

	resolve("www.zephyrproject.org");

	for (;;) {
		k_sleep(K_MSEC(1000));
	}
}
