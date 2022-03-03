#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <net/socket.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(application, LOG_LEVEL_DBG);

#define UDP_PORT 4242
#define BUF_SIZE 4096

void main(void)
{
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		LOG_ERR("socket failed: %d", fd);
	}

	struct sockaddr_in sin;

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(UDP_PORT);

	if (bind(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		LOG_ERR("bind failed: %d", errno);
		close(fd);
		return;
	}

	LOG_INF("Listening on UDP port %d", UDP_PORT);

	char buf[BUF_SIZE];

	for (;;) {
		ssize_t len = recv(fd, buf, BUF_SIZE, 0);
		if (len < 0) {
			LOG_ERR("recv failed: %d", errno);
		}

		LOG_INF("Received %d bytes", len);
		LOG_HEXDUMP_INF(buf, len, "Received data :");
	}
}
