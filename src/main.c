#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include <net/socket.h>
#include <net/net_core.h>
#include <net/net_if.h>
#include <net/net_mgmt.h>
#include <net/dns_resolve.h>

#include <net/http_client.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/*___________________________________________________________________________*/



int main(void)
{
	for (;;) {
		printk("Hello world!\n");
		k_sleep(K_SECONDS(1));
	}
}