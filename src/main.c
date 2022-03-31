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
LOG_MODULE_REGISTER(aws_client, LOG_LEVEL_INF);

/*___________________________________________________________________________*/

typedef struct {
	uint8_t *data;
	size_t len;
} buffer_t;

/*___________________________________________________________________________*/

static struct sockaddr_in broker;

static uint8_t rx_buffer[256];
static uint8_t tx_buffer[256];

static uint8_t payload_buffer[CONFIG_CLOUD_AWS_PAYLOAD_BUFFER_SIZE];

static struct mqtt_client client_ctx;

const char mqtt_client_name[] = CONFIG_CLOUD_AWS_DEVICE_NAME;

atomic_t mqtt_connected = ATOMIC_INIT(0);

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

#define TOPIC1 (CONFIG_CLOUD_AWS_DEVICE_NAME "/testTopic1")
#define TOPIC2 (CONFIG_CLOUD_AWS_DEVICE_NAME "/testTopic2")
#define TOPIC3 (CONFIG_CLOUD_AWS_DEVICE_NAME "/testTopic3")

static int subscribe_on_connect(void)
{
	int ret;

	/* suscribe on topics composed on device name and `testTopic` 1 or 2
	 * "zephyr-qemu-aws-device/testTopic1"
	 */

	struct mqtt_topic topics[2] = {
		{
			.topic = {
				.utf8 = TOPIC1,
				.size = sizeof(TOPIC1) - 1
			},
			.qos = CONFIG_CLOUD_AWS_QOS, 
		},
		{
			.topic = {
				.utf8 = TOPIC2,
				.size = sizeof(TOPIC2) - 1
			},
			.qos = CONFIG_CLOUD_AWS_QOS,
		},
	};

	struct mqtt_subscription_list sub_list = {
		.list = topics,
		.list_count = ARRAY_SIZE(topics),
		.message_id = 1u /* first MQTT message */
	};

	LOG_INF("Subscribing to %hu topics", sub_list.list_count);

	ret = mqtt_subscribe(&client_ctx, &sub_list);
	if (ret != 0) {
		LOG_ERR("Failed to subscribe to topics: %d", ret);
	}

	return ret;
}

static const buffer_t *get_payload_buffer(void)
{
	static const buffer_t buf = {
			.data = payload_buffer,
			.len = sizeof(payload_buffer)
	};

	return &buf;
}

static int publish_message(const char *topic, size_t topic_len,
			   uint8_t *payload, size_t payload_len)
{
	int ret;
	struct mqtt_publish_param msg;

	msg.retain_flag = 1U;
	msg.message.topic.topic.utf8 = topic;
	msg.message.topic.topic.size = topic_len;
	msg.message.topic.qos = CONFIG_CLOUD_AWS_QOS;
	msg.message.payload.data = payload;
	msg.message.payload.len = payload_len;

	ret = mqtt_publish(&client_ctx, &msg);
	if (ret != 0) {
		LOG_ERR("Failed to publish message: %d", ret);
	}

	LOG_INF("Publish %u B long message on topic %s", 
		payload_len, log_strdup(topic));
	LOG_HEXDUMP_INF(payload, payload_len, "Publish message");

	return ret;
}

/* https://docs.zephyrproject.org/latest/reference/networking/mqtt.html#c.mqtt_read_publish_payload */
static void handle_published_message(const struct mqtt_publish_message *msg)
{
	uint8_t topic[256];
	size_t topic_size = msg->topic.topic.size;
	size_t message_size = msg->payload.len;

	if (topic_size >= sizeof(topic)) {
		LOG_ERR("Topic is too long %u > %u",
			topic_size,
			sizeof(topic));
		return;
	}

	// read topic
	strncpy((char *)topic, (char *)msg->topic.topic.utf8, topic_size);
	topic[topic_size] = '\0';

	LOG_INF("Received %u B long message on topic %s", message_size, log_strdup(topic));

	// get payload buffer
	const buffer_t *buf = get_payload_buffer();
	size_t received = 0U;
	const bool discarded = message_size > buf->len;
	if (discarded) {
		LOG_WRN("Published messaged is too long for internal buffer %u > %u, discarding ...",
			message_size,
			buf->len);
	}

	while (received < message_size) {
		ssize_t ret;

		if (discarded == false) {
			ret = mqtt_read_publish_payload(&client_ctx,
							buf->data + received,
							buf->len - received);
		} else {
			ret = mqtt_read_publish_payload(&client_ctx,
							buf->data,
							buf->len);
		}

		if (ret < 0) {
			LOG_ERR("Failed to read payload: %d", ret);
			break;
		}

		received += ret;
	}

	LOG_DBG("Receiving message completed");

	if (discarded == false) {
		LOG_HEXDUMP_INF(buf->data, received, "Received message");
	}

	// notify application with the received message
	publish_message(TOPIC3, sizeof(TOPIC3) - 1, buf->data, received);
}

/*___________________________________________________________________________*/

static const char *mqtt_evt_str[] = {
	"MQTT_EVT_CONNACK",
	"MQTT_EVT_DISCONNECT",
	"MQTT_EVT_PUBLISH",
	"MQTT_EVT_PUBACK",
	"MQTT_EVT_PUBREC",
	"MQTT_EVT_PUBREL",
	"MQTT_EVT_PUBCOMP",
	"MQTT_EVT_SUBACK",
	"MQTT_EVT_UNSUBACK",
	"MQTT_EVT_PINGRESP",
};

static const char *mqtt_evt_get_str(enum mqtt_evt_type evt_type)
{
	static const char *unknown = "mqtt_evt_type<UNKNOWN>";
	const char *str = unknown;

	if (evt_type < ARRAY_SIZE(mqtt_evt_str)) {
		str = mqtt_evt_str[evt_type];
	} else {
		LOG_WRN("Unknown event type: %d", evt_type);
	}
	return str;
}

static void mqtt_event_cb(struct mqtt_client *client,
			  const struct mqtt_evt *evt)
{
	LOG_INF("Client: %x, event type: %hhx [ %s ]",
		(uint32_t)client,
		(uint8_t)evt->type,
		log_strdup(mqtt_evt_get_str(evt->type)));

	switch (evt->type) {
	case MQTT_EVT_CONNACK:
	{
		/* subsribe on few topics on connection */
		subscribe_on_connect();
		break;
	}

	case MQTT_EVT_DISCONNECT:
		break;

	case MQTT_EVT_PUBLISH:
	{
		const struct mqtt_publish_message *msg =
			&evt->param.publish.message;

		handle_published_message(msg);
		break;
	}

	case MQTT_EVT_PUBACK:
		break;

	case MQTT_EVT_PUBREC:
		break;

	case MQTT_EVT_PUBREL:
		break;

	case MQTT_EVT_PUBCOMP:
		break;

	case MQTT_EVT_SUBACK:
	{
		LOG_INF("Subscription acknowledged %d",
			evt->param.suback.message_id);
		break;
	}

	case MQTT_EVT_UNSUBACK:
	{
		LOG_INF("Unsubscription acknowledged %d",
			evt->param.unsuback.message_id);
		break;
	}

	case MQTT_EVT_PINGRESP:
		break;
	}
}

/*___________________________________________________________________________*/


static void aws_client_setup(void)
{
	// broker is properly defined at this point
	mqtt_client_init(&client_ctx);

	client_ctx.broker = &broker;
	client_ctx.evt_cb = mqtt_event_cb;

	client_ctx.client_id.utf8 = (uint8_t *)mqtt_client_name;
	client_ctx.client_id.size = sizeof(mqtt_client_name) - 1;
	client_ctx.password = NULL;
	client_ctx.user_name = NULL;

	// client_ctx.keepalive = CONFIG_MQTT_KEEPALIVE;

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
}

static int aws_client_try_connect(void)
{
	int ret;
	uint32_t tries = 3U;

	while (tries > 0) {
		LOG_DBG("MQTT %u try connect", (uint32_t) &client_ctx);
		ret = mqtt_connect(&client_ctx);
		if (ret == 0) {
			LOG_INF("MQTT %u connected !", (uint32_t) &client_ctx);
			break;
		} else {
			LOG_ERR("Failed to connect: %d", ret);
			k_sleep(K_SECONDS(5));
			tries--;
		}
	}

	return ret;
}

void aws_client_thread(void )
{
	int rc;
	struct pollfd fds[1];

	aws_client_setup();
	
	rc = aws_client_try_connect();
	if (rc != 0) {
		goto cleanup;
	}

	fds[0].fd = client_ctx.transport.tcp.sock;
	fds[0].events = POLLIN | POLLHUP | POLLERR;

	int timeout = mqtt_keepalive_time_left(&client_ctx);
	for (;;) {
		rc = poll(fds, 1, timeout);
		if (rc >= 0) {
			if ((fds[0].revents & POLLIN) != 0) {
				rc = mqtt_input(&client_ctx);
				if (rc != 0) {
					LOG_ERR("Failed to read MQTT input: %d", rc);
					break;
				}
			} else if ((fds[0].revents & (POLLHUP | POLLERR)) != 0) {
				LOG_ERR("MQTT %x connection error/closed", (uint32_t)&client_ctx);
				break;
			} else {
				LOG_DBG("MQTT %x poll timeout", (uint32_t)&client_ctx);
			}

			rc = mqtt_live(&client_ctx);
			if ((rc != 0) && (rc != -EAGAIN)) {
				LOG_ERR("Failed to live MQTT: %d", rc);
				break;
			}
		} else {
			LOG_ERR("poll failed: %d", rc);
			break;
		}
	}

cleanup:
	rc = mqtt_disconnect(&client_ctx);
	if (rc != 0) {
		LOG_ERR("Failed to disconnect from broker: %d", rc);
		return;
	}

	LOG_INF("MQTT disconnected ! %d", 0);

	close(fds[0].fd);
	fds[0].fd = -1;

	LOG_INF("MQTT cleanup %d", rc);
	return;
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
