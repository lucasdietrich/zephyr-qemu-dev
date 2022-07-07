#include <zephyr.h>

#include "app_fs.h"
#include "app_lua.h"

int main(void)
{
	app_fs_test();
	app_lua_test();

	return 0;
}