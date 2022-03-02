#include "func.h"

#include <kernel.h>

void hello(void)
{
    printk("[%u] Hello from QEMU !\n", k_uptime_get_32());
}