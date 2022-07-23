#include <zephyr.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include <sys/mem_blocks.h>

SYS_MEM_BLOCKS_DEFINE_STATIC(memblocks, 0x100, 4, 4);

int main(void)
{
	void *blocks[4];
	int ret = sys_mem_blocks_alloc(&memblocks, 2, blocks);
	printk("sys_mem_blocks_alloc : ret: %d\n", ret);

	if (ret == 0) {
		printk("blocks: %p %p\n", blocks[0], blocks[1]);
		sys_mem_blocks_free(&memblocks, 2, blocks);
		printk("sys_mem_blocks_free : ret: %d\n", ret);
	}

	void *alloc;
	ret = sys_mem_blocks_alloc(&memblocks, 1, &alloc);
	printk("sys_mem_blocks_alloc : ret: %d\n", ret);

	if (ret == 0) {
		printk("blocks: %p\n", alloc);
		sys_mem_blocks_free(&memblocks, 1, &alloc);
		printk("sys_mem_blocks_free : ret: %d\n", ret);
	}

	return 0;
}