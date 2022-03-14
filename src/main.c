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

#include <net/tls_credentials.h>

#include "certs/certs.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(aws_client, LOG_LEVEL_DBG);

/*___________________________________________________________________________*/

// if all certificates are PEM, one TAG number is enough
#define TLS_TAG_DEVICE_CERTIFICATE 1
#define TLS_TAG_DEVICE_PRIVATE_KEY 1
#define TLS_TAG_AWS_CA_CERTIFICATE 1

static sec_tag_t sec_tls_tags[] = {
	TLS_TAG_DEVICE_CERTIFICATE,
};

static int setup_credentials(void)
{
	int ret;

	ret = tls_credential_add(TLS_TAG_DEVICE_CERTIFICATE,
				 TLS_CREDENTIAL_SERVER_CERTIFICATE,
				 public_cert,
				 sizeof(public_cert));
	if (ret < 0) {
		LOG_ERR("Failed to add device certificate: %d", ret);
		goto exit;
	}

	ret = tls_credential_add(TLS_TAG_DEVICE_PRIVATE_KEY,
				 TLS_CREDENTIAL_PRIVATE_KEY,
				 private_key,
				 sizeof(private_key));
	if (ret < 0) {
		LOG_ERR("Failed to add device private key: %d", ret);
		goto exit;
	}

	ret = tls_credential_add(TLS_TAG_AWS_CA_CERTIFICATE,
				 TLS_CREDENTIAL_CA_CERTIFICATE,
				 ca_cert,
				 sizeof(ca_cert));
	if (ret < 0) {
		LOG_ERR("Failed to add device private key: %d", ret);
		goto exit;
	}
exit:
	return ret;
}

/*___________________________________________________________________________*/

static struct sockaddr_in broker;

static uint8_t rx_buffer[4096];
static uint8_t tx_buffer[4096];

static struct mqtt_client client_ctx;

const char mqtt_client_name[] = "zephyr_qemu_aws_mqtt_client";

static void mqtt_event_cb(struct mqtt_client *client,
			  const struct mqtt_evt *evt)
{
	LOG_DBG("Client: %x, event type: %hhx",
		(uint32_t)client, (uint8_t)evt->type);

	// switch(evt->type) {

	// }
}

void aws_client_thread(void )
{
	int rc;
	struct pollfd fds[1];

	// broker is properly defined at this point
	mqtt_client_init(&client_ctx);

	client_ctx.broker = &broker;
	client_ctx.evt_cb = mqtt_event_cb;

	client_ctx.client_id.utf8 = (uint8_t *) mqtt_client_name;
	client_ctx.client_id.size = sizeof(mqtt_client_name) - 1;
	client_ctx.password = NULL;
	client_ctx.user_name = NULL;

	client_ctx.protocol_version = MQTT_VERSION_3_1_1;
	
	client_ctx.rx_buf = rx_buffer;
	client_ctx.rx_buf_size = sizeof(rx_buffer);
	client_ctx.tx_buf = tx_buffer;
	client_ctx.tx_buf_size = sizeof(tx_buffer);

	// setup TLS
	client_ctx.transport.type = MQTT_TRANSPORT_SECURE;
	struct mqtt_sec_config *const tls_config = &client_ctx.transport.tls.config;

	tls_config->peer_verify = TLS_PEER_VERIFY_REQUIRED;
	tls_config->cipher_list = NULL;
	tls_config->sec_tag_list = sec_tls_tags;
	tls_config->sec_tag_count = ARRAY_SIZE(sec_tls_tags);
	tls_config->hostname = CONFIG_CLOUD_AWS_HOSTNAME;

	// connect
	rc = mqtt_connect(&client_ctx);
	if (rc != 0) {
		LOG_ERR("Failed to connect to broker: %d", rc);
		return;
	}

	LOG_INF("MQTT connected ! %d", 0);

	for (uint32_t i = 0; i < 10; i++) {
		mqtt_input(&client_ctx);
		mqtt_live(&client_ctx);
		k_sleep(K_SECONDS(1));
	}

	rc = mqtt_disconnect(&client_ctx);
	if (rc != 0) {
		LOG_ERR("Failed to disconnect from broker: %d", rc);
		return;
	}

	LOG_INF("MQTT disconnected ! %d", 0);

	// fds[0].fd = client_ctx.transport.tls.sock;

	for (;;) {

	}
}

/*___________________________________________________________________________*/

void show_time(void)
{
	struct timespec tspec;
	struct tm time_infos;

	clock_gettime(CLOCK_REALTIME, &tspec);

	gmtime_r(&tspec.tv_sec, &time_infos);

	LOG_INF("UTC time : %04d/%02d/%02d %02d:%02d:%02d",
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
		// copy address to broker
		memcpy(&broker, ai->ai_addr,
		       MAX(ai->ai_addrlen,
			   sizeof(struct sockaddr_storage)));
		
		broker.sin_port = htons(CONFIG_CLOUD_AWS_PORT);

		char addr_str[INET_ADDRSTRLEN];
		zsock_inet_ntop(AF_INET,
				&broker.sin_addr,
				addr_str,
				sizeof(addr_str));
		LOG_DBG("Resolved: %s", log_strdup(addr_str));
	}

	zsock_freeaddrinfo(ai);
}

/*___________________________________________________________________________*/

void main(void)
{
	dns_init_resolver();

	resolve_hostname(CONFIG_CLOUD_AWS_HOSTNAME);

	sntp_sync_time();

	show_time();

	setup_credentials();

	aws_client_thread();
}
