#ifndef _SLIB_H_
#define _SLIB_H_

#include <stdint.h>

int slib_init(void);

int slib_put(uint32_t data);

int slib_get(uint32_t *data);

#endif
