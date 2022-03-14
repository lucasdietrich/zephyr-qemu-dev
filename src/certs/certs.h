#ifndef _DEVICE_CERTS_H_
#define _DEVICE_CERTS_H_

static const char ca_cert[] = {
	#include "ca_cert.cer"
};

static const char private_key[] = {
	#include "private_key.cer"
};

static const char public_cert[] = {
	#include "public_cert.cer"
};

#endif /* _DEVICE_CERTS_H_ */