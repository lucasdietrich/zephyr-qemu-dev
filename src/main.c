#include <zephyr.h>
#include <assert.h>

#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <stdlib.h>
#include <sys/times.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#define DUMMY_STDLIB_FUNCTIONS
#ifdef DUMMY_STDLIB_FUNCTIONS
/* https://man7.org/linux/man-pages/man2/times.2.html */
clock_t _times(struct tms* tms)
{
	__ASSERT(0, "_times not implemented");
	return (clock_t) -1;
}

/* https://man7.org/linux/man-pages/man2/unlink.2.html */
/* http://manpagesfr.free.fr/man/man2/unlink.2.html */
int _unlink(const char *pathname)
{
	__ASSERT(0, "_unlink not implemented");
	return -1;
}

/* https://man7.org/linux/man-pages/man2/link.2.html */
int _link(const char *oldpath, const char *newpath)
{
	__ASSERT(0, "_link not implemented");
	return -1;
}
#endif

int main(void)
{
	int res;
	printk("LUA starting ...\n");

	lua_writestring(LUA_COPYRIGHT, strlen(LUA_COPYRIGHT));
	lua_writeline();

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	luaL_loadstring(L, "print('Hello from LUA running on Zephyr RTOS !')");
	res = lua_pcall(L, 0, LUA_MULTRET, 0);
	lua_close(L);

	printk("LUA done ... : %d\n", res);

	return 0;
}