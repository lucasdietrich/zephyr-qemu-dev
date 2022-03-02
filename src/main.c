#include <zephyr.h>

#include "func.h"

int main(void)
{
    for (;;) {
        hello();

        k_sleep(K_MSEC(1000));
    }
}