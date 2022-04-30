#include "utils.h"

uint32_t utils_genrdm(void)
{
	static uint32_t i = 0;

	i = 100000U + (i * 7777U) + (i * i);

	return i;
}