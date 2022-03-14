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