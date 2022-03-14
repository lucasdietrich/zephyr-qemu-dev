#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include <time.h>
#include <net/sntp.h>
#include <posix/time.h>

#include <net/mqtt.h>

#include <net/socket.h>
#include <net/net_core.h>
#include <net/net_if.h>
#include <net/net_mgmt.h>
#include <net/dns_resolve.h>

#include "certs/certs.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(aws_client, LOG_LEVEL_DBG);

void show_time(void)
{
	struct timespec tspec;
	struct tm time_infos;

	clock_gettime(CLOCK_REALTIME, &tspec);

	gmtime_r(&tspec.tv_sec, &time_infos);

	LOG_INF("UTC time : %04d/%02d/%02d %02d:%02d:%02d\n",
		time_infos.tm_year + 1900, time_infos.tm_mon + 1,
		time_infos.tm_mday, time_infos.tm_hour,
		time_infos.tm_min, time_infos.tm_sec);
}

int sntp_sync_time(void)
{
	int rc;
	struct sntp_time sntp_time;
        struct timespec tspec;

	LOG_DBG("Sending NTP request for current time: %d", 0);

	rc = sntp_simple("time.google.com", SYS_FOREVER_MS, &sntp_time);
	if (rc == 0) {
		tspec.tv_sec = sntp_time.seconds;
		tspec.tv_nsec = ((uint64_t)sntp_time.fraction * 
				 (1000LU * 1000LU * 1000LU)) >> 32;

		clock_settime(CLOCK_REALTIME, &tspec);
		
		LOG_DBG("Acquired time from NTP server: %u", 
			(uint32_t)tspec.tv_sec);

	} else {
		LOG_ERR("Failed to acquire SNTP, code %d\n", rc);
	}
	return rc;
}

void resolve_hostname(const char *hostname)
{
	int ret = 0U;
	struct zsock_addrinfo *ai = NULL;

	LOG_DBG("Resolving: %s", log_strdup(hostname));

	const struct zsock_addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
		.ai_protocol = 0
	};
	ret = zsock_getaddrinfo(hostname, "80", &hints, &ai);
	if (ret != 0) {
		LOG_ERR("failed to resolve hostname err = %d (errno = %d)",
			ret, errno);
	} else {
		char addr_str[INET_ADDRSTRLEN];
		zsock_inet_ntop(AF_INET,
				&((struct sockaddr_in *)ai->ai_addr)->sin_addr,
				addr_str,
				sizeof(addr_str));

		// log addr_str
		LOG_DBG("Resolved: %s", addr_str);
	}

	zsock_freeaddrinfo(ai);
}

void aws_client_thread(void )
{
	for (;;) {

	}
}

void main(void)
{
	dns_init_resolver();

	resolve_hostname(CONFIG_CLOUD_AWS_HOSTNAME);

	sntp_sync_time();

	show_time();

	aws_client_thread();
}
