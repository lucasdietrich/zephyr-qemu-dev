#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <net/socket.h>

#define UDP_PORT 4242


void main(void)
{
	int fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (fd < 0) {
		printk("socket");
	}

	{
		struct sockaddr_in sin;

		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = htonl(INADDR_ANY);
		sin.sin_port = htons(UDP_PORT);

		if (bind(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
			printk("bind");
		}

		printk("Listening on UDP port %d\n", UDP_PORT);
	}

	{

#define BUF_SIZE 4096

		char buf[BUF_SIZE];

		for (;;) {
			ssize_t len = recv(fd, buf, BUF_SIZE, 0);

			if (len < 0) {
				printk("recv");
			}

			printk("Received %ld bytes\n", (long)len);
		}
	}
}
