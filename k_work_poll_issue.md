# Excpected output:

```
Booting from ROM..*** Booting Zephyr OS build zephyr-v2.7.2  ***
ASSERTION FAIL [z_spin_lock_valid(l)] @ WEST_TOPDIR/zephyr/include/spinlock.h:142
        Recursive spinlock 0x10a130
```

Where :

![](./pics/issue_k_work_submit_to_queue.png)