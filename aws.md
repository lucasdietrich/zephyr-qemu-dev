# AWS

## Ressources

- [AWS : MQTT](https://docs.aws.amazon.com/fr_fr/iot/latest/developerguide/mqtt.html)
- [Zephyr RTOS : MQTT](https://docs.zephyrproject.org/latest/reference/networking/mqtt.html)
- [AWS : MQTT test client](https://console.aws.amazon.com/iot/home?region=us-east-1#/test)

# Device information

- Device : `zephyr-qemu-aws-device`
- Endpoint : `a31gokdeokxhl8-ats.iot.us-east-1.amazonaws.com`
- [Public certificate](./certs/165a24f1ae6f9a2223b991f158c46e0518d83bca7b5bb62c37ba6a2454066d2e-certificate.pem.crt) : 
    `./certs/165a24f1ae6f9a2223b991f158c46e0518d83bca7b5bb62c37ba6a2454066d2e-certificate.pem.crt`
- [Private key certificate](./certs/165a24f1ae6f9a2223b991f158c46e0518d83bca7b5bb62c37ba6a2454066d2e-private.pem.key)
    `./certs/165a24f1ae6f9a2223b991f158c46e0518d83bca7b5bb62c37ba6a2454066d2e-private.pem.key`
- [RSA Amazon Root CA certificate](./certs/AmazonRootCA1.pem)
    `./certs/AmazonRootCA1.pem`
- [ECC Amazon Root CA certificate](./certs/AmazonRootCA3.pem)
    `./certs/AmazonRootCA3.pem`

## Prerequisites

- MBEDTLS :
  - Min heap : 32k
- Updated timestamp (SNTP)
- IPv4 addr

## Expected output

```
[13/14] Linking C executable zephyr/zephyr.elf
Memory region         Used Size  Region Size  %age Used
             RAM:      471072 B         3 MB     14.97%
        IDT_LIST:          0 GB         2 KB      0.00%
[13/14] To exit from QEMU enter: 'CTRL+a, x'[QEMU] CPU: qemu32,+nx,+pae
SeaBIOS (version zephyr-v1.0.0-0-g31d4e0e-dirty-20200714_234759-fv-az50-zephyr)
Booting from ROM..*** Booting Zephyr OS build zephyr-v2.7.1  ***


[00:00:00.110,000] <inf> net_config: IPv6 address: 2001:db8::1
[00:00:00.110,000] <inf> net_config: IPv6 address: 2001:db8::1
[00:00:00.110,000] <dbg> aws_client.resolve_hostname: Resolving: a31gokdeokxhl8-ats.iot.us-east-1.amazonaws.com
<dbg> aws_client.resolve_hostname: Resolved: 52.1.218.247
<dbg> aws_client.sntp_sync_time: Sending NTP request for current time: 0
<dbg> aws_client.sntp_sync_time: Acquired time from NTP server: 1647293234
<inf> aws_client: UTC time : 2022/03/14 21:27:14
<inf> aws_client: MQTT connected ! 0
<dbg> aws_client.mqtt_event_cb: Client: 14b840, event type: 0
<dbg> aws_client.mqtt_event_cb: Client: 14b840, event type: 1
<inf> aws_client: MQTT disconnected ! 0
```