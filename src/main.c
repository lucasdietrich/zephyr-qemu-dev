#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <net/socket.h>
#include <net/net_core.h>
#include <net/net_if.h>
#include <net/net_mgmt.h>
#include <net/dns_resolve.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(application, LOG_LEVEL_DBG);

struct k_sem dns_cb_sem;

void dns_result_cb(enum dns_resolve_status status,
		   struct dns_addrinfo *info,
		   void *user_data)
{
	LOG_DBG("dns_result_cb");

	char hr_addr[NET_IPV6_ADDR_LEN];
	char *hr_family;
	void *addr;

	switch (status) {
	case DNS_EAI_CANCELED:
		LOG_INF("DNS query was canceled (%x)", (uint32_t) user_data);
		return;
	case DNS_EAI_FAIL:
		LOG_INF("DNS resolve failed (%x)", (uint32_t) user_data);
		return;
	case DNS_EAI_NODATA:
		LOG_INF("Cannot resolve address (%x)", (uint32_t) user_data);
		return;
	case DNS_EAI_ALLDONE:
		LOG_INF("DNS resolving finished (%x)", (uint32_t) user_data);
		return;
	case DNS_EAI_INPROGRESS:
		break;
	default:
		LOG_INF("DNS resolving error (%d)", status);
		return;
	}

	if (!info) {
		return;
	}

	if (info->ai_family == AF_INET) {
		hr_family = "IPv4";
		addr = &net_sin(&info->ai_addr)->sin_addr;
	} else if (info->ai_family == AF_INET6) {
		hr_family = "IPv6";
		addr = &net_sin6(&info->ai_addr)->sin6_addr;
	} else {
		LOG_ERR("Invalid IP address family %d", info->ai_family);
		return;
	}

	LOG_INF("%s %s address: %s", user_data ? (char *)user_data : "<null>",
		hr_family,
		log_strdup(net_addr_ntop(info->ai_family, addr,
					 hr_addr, sizeof(hr_addr))));
	k_sem_give(&dns_cb_sem);
}

void resolve1(const char *hostname)
{
	static uint16_t dns_id;
	int ret;

	k_sem_init(&dns_cb_sem, 0, 1);

	ret = dns_get_addr_info(hostname,
				DNS_QUERY_TYPE_A,
				&dns_id,
				dns_result_cb,
				(void *)hostname,
				1000);
	LOG_INF("ret = %d", ret);

	k_sem_take(&dns_cb_sem, K_FOREVER);
}

void resolve2(const char *hostname)
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

	resolve1("www.zephyrproject.org");
	resolve2("www.google.fr");

	for (;;) {
		k_sleep(K_MSEC(1000));
	}
}
